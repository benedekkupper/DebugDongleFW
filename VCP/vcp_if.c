/**
  ******************************************************************************
  * @file    vcp_if.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   USB Virtual COM Port interface implementation
  *
  *  @verbatim
  *
  * ===================================================================
  *                         USB Virtual COM Port
  * ===================================================================
  *  This interface implements a virtual COM port using USB CDC class
  *  and an UART peripheral of the device. The received USB packets
  *  are transferred on UART using a 2-page buffer, one is receiving
  *  the USB OUT endpoint data, the other is used by the UART Tx DMA.
  *  The received UART bytes are put in a circular buffer by the Rx DMA
  *  and are monitored by a timer callback. New received bytes are
  *  sent over the USB IN endpoint.
  *  @endverbatim
  *
  * Copyright (c) 2018 Benedek Kupper
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */
#include <bsp_usart.h>
#include <vcp_if.h>

static void VCP_Open(void* itf, USBD_CDC_LineCodingType * line);
static void VCP_Close(void* itf);
static void VCP_USB_ReceiveNew(void* itf, uint8_t* pbuf, uint16_t length);
static void VCP_USB_TransmitNew(void* itf, uint8_t* pbuf, uint16_t length);

static void VCP_UART_Transmitted(void * handle);

const USBD_CDC_AppType vcpApp =
{
    .Name           = "VCP Interface",
    .Open           = VCP_Open,
    .Close          = VCP_Close,
    .Received       = VCP_USB_ReceiveNew,
    .Transmitted    = VCP_USB_TransmitNew,
};

/**
 * @brief  This function sets up the bidirectional USB-UART communication.
 * @param  itf: callback sender interface
 * @param  line: serial port line coding parameters
 */
static void VCP_Open(void* itf, USBD_CDC_LineCodingType * line)
{
    VCP_HandleType *vcp = container_of(itf, VCP_HandleType, CdcIf);
    static UART_InitType serialConfig = {
            .Baudrate      = 115200,
            .Directions    = USART_DIR_TX_RX,
            .DataSize      = 8,
            .StopBits      = USART_STOPBITS_1,
            .SingleSample  = DISABLE,
            .Parity        = USART_PARITY_NONE,
            .FlowControl   = UART_FLOWCONTROL_NONE,
            .OverSampling8 = ENABLE,
            .HalfDuplex    = DISABLE,
    };

    serialConfig.Baudrate = line->DTERate;
    serialConfig.DataSize = line->DataBits;

    /* set the Stop bit */
    if (line->CharFormat == 2)
    {
        serialConfig.StopBits = USART_STOPBITS_2;
    }
    else
    {
        serialConfig.StopBits = USART_STOPBITS_1;
    }

    /* set the parity bit */
    switch (line->ParityType)
    {
        case 1:
            serialConfig.Parity = USART_PARITY_ODD;
            break;
        case 2:
            serialConfig.Parity = USART_PARITY_EVEN;
            break;
        default:
            serialConfig.Parity = USART_PARITY_NONE;
            break;
    }

    /* Initialize UART with the current configuration, reset DMAs */
    USART_vInitAsync(&vcp->Uart, &serialConfig);
    DMA_vStop(vcp->Uart.DMA.Transmit);
    DMA_vStop(vcp->Uart.DMA.Receive);

    /* Subscribe to UART transmit complete callback */
    vcp->Uart.Callbacks.Transmit = VCP_UART_Transmitted;

    /* Page 0 is initialized for OUT endpoint reception */
    vcp->OutStatus[0] = VCP_BUFFER_RECEIVING;
    vcp->OutStatus[1] = VCP_BUFFER_EMPTY;
    (void) USBD_CDC_Receive(itf, vcp->OutData[0], VCP_OUT_DATA_SIZE / 2);

    /* Start circular buffer reception with DMA for IN endpoint */
    vcp->Index = 0;
    USART_FLAG_CLEAR(&vcp->Uart, RXNE);
    (void) USART_eReceive_DMA(&vcp->Uart, vcp->InData, VCP_IN_DATA_SIZE);
}

/**
 * @brief  This function shuts down the UART subsystem.
 * @param  itf: callback sender interface
 */
static void VCP_Close(void* itf)
{
    VCP_HandleType *vcp = container_of(itf, VCP_HandleType, CdcIf);
    USART_vDeinit(&vcp->Uart);
}

/**
 * @brief  Data received over USB OUT endpoint are sent over UART by this function.
 * @param  itf: callback sender interface
 * @param  pbuf: Buffer of received data
 * @param  length: Number of data received (in bytes)
 */
static void VCP_USB_ReceiveNew(void* itf, uint8_t * pbuf, uint16_t length)
{
    VCP_HandleType *vcp = container_of(itf, VCP_HandleType, CdcIf);
    uint8_t page = (vcp->OutStatus[0] == VCP_BUFFER_RECEIVING) ? 0 : 1;

    /* If UART transmission is ongoing on other page */
    if (vcp->OutStatus[1 - page] == VCP_BUFFER_TRANSMITTING)
    {
        /* Just indicate buffer status */
        vcp->OutStatus[page] = VCP_BUFFER_FULL;
        vcp->OutLength       = length;
    }
    else
    {
        /* Switch pages, start transfer on both */
        vcp->OutStatus[page] = VCP_BUFFER_TRANSMITTING;
        (void) USART_eTransmit_DMA(&vcp->Uart, vcp->OutData[page], length);

        vcp->OutStatus[1 - page] = VCP_BUFFER_RECEIVING;
        (void) USBD_CDC_Receive(itf, vcp->OutData[1 - page], VCP_OUT_DATA_SIZE / 2);
    }
}

/**
 * @brief  This function starts new UART transmission and USB reception
 *         if a full transmit buffer is available.
 * @param  handle: unused
 */
static void VCP_UART_Transmitted(void * handle)
{
    VCP_HandleType *vcp = container_of(handle, VCP_HandleType, Uart);
    uint8_t page = (vcp->OutStatus[0] == VCP_BUFFER_TRANSMITTING) ? 0 : 1;

    /* The current page has been transferred over UART */
    vcp->OutStatus[page] = VCP_BUFFER_EMPTY;

    /* If other page is full already */
    if (vcp->OutStatus[1 - page] == VCP_BUFFER_FULL)
    {
        /* Switch pages, start transfer on both */
        vcp->OutStatus[1 - page] = VCP_BUFFER_TRANSMITTING;
        (void) USART_eTransmit_DMA(handle, vcp->OutData[1 - page], vcp->OutLength);

        vcp->OutStatus[page] = VCP_BUFFER_RECEIVING;
        (void) USBD_CDC_Receive(&vcp->CdcIf, vcp->OutData[page], VCP_OUT_DATA_SIZE / 2);
    }
}

/**
 * @brief  This function transmits recently received UART data over USB.
 * @param  itf: callback sender interface
 * @param  pbuf: unused
 * @param  length: unused
 */
static void VCP_USB_TransmitNew(void* itf, uint8_t * pbuf, uint16_t length)
{
    VCP_HandleType *vcp = container_of(itf, VCP_HandleType, CdcIf);
    /* Determine the buffer index of the UART DMA */
    uint16_t rxIndex = VCP_IN_DATA_SIZE - DMA_usGetStatus(vcp->Uart.DMA.Receive);

    /* If the UART RX index is ahead, transmit the new data */
    if (vcp->Index < rxIndex)
    {
        if(USBD_E_OK == USBD_CDC_Transmit(itf,
                &vcp->InData[vcp->Index], rxIndex - vcp->Index))
        {
            vcp->Index = rxIndex;
        }
    }
    /* If the USB IN index is ahead, the buffer has wrapped, transmit until the end */
    else if (vcp->Index > rxIndex)
    {
        if (USBD_E_OK == USBD_CDC_Transmit(itf,
                &vcp->InData[vcp->Index], VCP_IN_DATA_SIZE - vcp->Index))
        {
            vcp->Index = 0;
        }
    }
}

/**
 * @brief  This function should be called periodically from timer callback.
 *         It requests new USB IN transfer if new UART data has been received.
 */
void VCP_Periodic(VCP_HandleType *vcp)
{
    if (vcp->CdcIf.LineCoding.DataBits != 0)
    {
        /* Transmit the received UART data periodically */
        VCP_USB_TransmitNew(&vcp->CdcIf, NULL, 0);
    }
}

