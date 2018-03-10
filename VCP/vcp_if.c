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

#define VCP_OUT_DATA_SIZE   128
#define VCP_IN_DATA_SIZE    128

UART_InitType SerialConfig = {
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

typedef enum
{
    BUFFER_EMPTY = 0,
    BUFFER_FULL,
    BUFFER_RECEIVING,
    BUFFER_TRANSMITTING
}BufferStatusType;

/* This structure is used for data transfer management
 * between the two communication channels */
struct {
    uint8_t OutData[2][VCP_OUT_DATA_SIZE / 2];
    BufferStatusType OutStatus[2];
    uint16_t OutLength;
    uint8_t InData[VCP_IN_DATA_SIZE];
    uint16_t Index;
}VCP_Memory;

static void VCP_Init(void);
static void VCP_Deinit(void);
static void VCP_USB_Control(USB_SetupRequestType * req, uint8_t* pbuf);
static void VCP_USB_ReceiveNew(uint8_t* pbuf, uint16_t length);
static void VCP_USB_TransmitNew(uint8_t* pbuf, uint16_t length);

static void VCP_UART_Transmitted(void * handle);

const USBD_CDC_AppType vcpApp =
{
    .Name           = "VCP Interface",
    .Init           = VCP_Init,
    .Deinit         = VCP_Deinit,
    .Control        = VCP_USB_Control,
    .Received       = VCP_USB_ReceiveNew,
    .Transmitted    = VCP_USB_TransmitNew,
};

USBD_CDC_IfHandleType hvcp_if = {
    .App = &vcpApp,
    .Base.AltCount = 1,
}, *const vcp_if = &hvcp_if;

/**
 * @brief  This function sets up the bidirectional USB-UART communication.
 */
static void VCP_Init(void)
{
    /* Initialize UART with the current configuration, reset DMAs */
    USART_vInitAsync(vcp_uart, &SerialConfig);
    DMA_vStop(vcp_uart->DMA.Transmit);
    DMA_vStop(vcp_uart->DMA.Receive);

    /* Subscribe to UART transmit complete callback */
    vcp_uart->Callbacks.Transmit = VCP_UART_Transmitted;

    /* Page 0 is initialized for OUT endpoint reception */
    VCP_Memory.OutStatus[0] = BUFFER_RECEIVING;
    VCP_Memory.OutStatus[1] = BUFFER_EMPTY;
    (void) USBD_CDC_Receive(vcp_if, VCP_Memory.OutData[0], VCP_OUT_DATA_SIZE / 2);

    /* Start circular buffer reception with DMA for IN endpoint */
    VCP_Memory.Index = 0;
    USART_FLAG_CLEAR(vcp_uart, RXNE);
    (void) USART_eReceive_DMA(vcp_uart, VCP_Memory.InData, VCP_IN_DATA_SIZE);
}

/**
 * @brief  This function shuts down the UART subsystem.
 */
static void VCP_Deinit(void)
{
    USART_vDeinit(vcp_uart);
}

/**
 * @brief  Manage the CDC class requests
 * @param  req: Setup request
 * @param  pbuf: Buffer containing command data (request parameters)
 */
static void VCP_USB_Control(USB_SetupRequestType * req, uint8_t* pbuf)
{
    switch (req->Request)
    {
        /* Sets the UART configuration */
        case CDC_REQ_SET_LINE_CODING:
        {
            USBD_CDC_LineCodingType* line = (USBD_CDC_LineCodingType*)pbuf;

            SerialConfig.Baudrate = line->DTERate;
            SerialConfig.DataSize = line->DataBits;

            /* set the Stop bit */
            if (line->CharFormat == 2)
            {
                SerialConfig.StopBits = USART_STOPBITS_2;
            }
            else
            {
                SerialConfig.StopBits = USART_STOPBITS_1;
            }

            /* set the parity bit */
            switch (line->ParityType)
            {
                case 1:
                    SerialConfig.Parity = USART_PARITY_ODD;
                    break;
                case 2:
                    SerialConfig.Parity = USART_PARITY_EVEN;
                    break;
                default:
                    SerialConfig.Parity = USART_PARITY_NONE;
                    break;
            }
            VCP_Init();
            break;
        }

        /* Returns the current UART configuration */
        case CDC_REQ_GET_LINE_CODING:
        {
            USBD_CDC_LineCodingType* line = (USBD_CDC_LineCodingType*)pbuf;

            line->DTERate    = SerialConfig.Baudrate;
            line->CharFormat = SerialConfig.StopBits;
            line->DataBits   = SerialConfig.DataSize;
            line->ParityType = (SerialConfig.Parity == USART_PARITY_ODD) ?
                    1 : SerialConfig.Parity;
            break;
        }

        default:
            break;
    }
}

/**
 * @brief  Data received over USB OUT endpoint are sent over UART by this function.
 * @param  pbuf: Buffer of received data
 * @param  length: Number of data received (in bytes)
 */
static void VCP_USB_ReceiveNew(uint8_t * pbuf, uint16_t length)
{
    uint8_t page = (VCP_Memory.OutStatus[0] == BUFFER_RECEIVING) ? 0 : 1;

    /* If UART transmission is ongoing on other page */
    if (VCP_Memory.OutStatus[1 - page] == BUFFER_TRANSMITTING)
    {
        /* Just indicate buffer status */
        VCP_Memory.OutStatus[page] = BUFFER_FULL;
        VCP_Memory.OutLength       = length;
    }
    else
    {
        /* Switch pages, start transfer on both */
        VCP_Memory.OutStatus[page] = BUFFER_TRANSMITTING;
        (void) USART_eTransmit_DMA(vcp_uart, VCP_Memory.OutData[page], length);

        VCP_Memory.OutStatus[1 - page] = BUFFER_RECEIVING;
        (void) USBD_CDC_Receive(vcp_if, VCP_Memory.OutData[1 - page], VCP_OUT_DATA_SIZE / 2);
    }
}

/**
 * @brief  This function starts new UART transmission and USB reception
 *         if a full transmit buffer is available.
 * @param  handle: unused
 */
static void VCP_UART_Transmitted(void * handle)
{
    uint8_t page = (VCP_Memory.OutStatus[0] == BUFFER_TRANSMITTING) ? 0 : 1;

    /* The current page has been transferred over UART */
    VCP_Memory.OutStatus[page] = BUFFER_EMPTY;

    /* If other page is full already */
    if (VCP_Memory.OutStatus[1 - page] == BUFFER_FULL)
    {
        /* Switch pages, start transfer on both */
        VCP_Memory.OutStatus[1 - page] = BUFFER_TRANSMITTING;
        (void) USART_eTransmit_DMA(vcp_uart, VCP_Memory.OutData[1 - page], VCP_Memory.OutLength);

        VCP_Memory.OutStatus[page] = BUFFER_RECEIVING;
        (void) USBD_CDC_Receive(vcp_if, VCP_Memory.OutData[page], VCP_OUT_DATA_SIZE / 2);
    }
}

/**
 * @brief  This function transmits recently received UART data over USB.
 * @param  pbuf: unused
 * @param  length: unused
 */
static void VCP_USB_TransmitNew(uint8_t * pbuf, uint16_t length)
{
    /* Determine the buffer index of the UART DMA */
    uint16_t rxIndex = VCP_IN_DATA_SIZE - DMA_usGetStatus(vcp_uart->DMA.Receive);

    /* If the UART RX index is ahead, transmit the new data */
    if (VCP_Memory.Index < rxIndex)
    {
        if(USBD_E_OK == USBD_CDC_Transmit(vcp_if,
                &VCP_Memory.InData[VCP_Memory.Index], rxIndex - VCP_Memory.Index))
        {
            VCP_Memory.Index = rxIndex;
        }
    }
    /* If the USB IN index is ahead, the buffer has wrapped, transmit until the end */
    else if (VCP_Memory.Index > rxIndex)
    {
        if (USBD_E_OK == USBD_CDC_Transmit(vcp_if,
                &VCP_Memory.InData[VCP_Memory.Index], VCP_IN_DATA_SIZE - VCP_Memory.Index))
        {
            VCP_Memory.Index = 0;
        }
    }
}

/**
 * @brief  This function should be called periodically from timer callback.
 *         It requests new USB IN transfer if new UART data has been received.
 */
void VCP_Periodic(void)
{
    if (vcp_if->Base.Device->ConfigSelector != 0)
    {
        /* Transmit the received UART data periodically */
        VCP_USB_TransmitNew(NULL, 0);
    }
}

