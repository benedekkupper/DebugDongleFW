/**
  ******************************************************************************
  * @file    bsp_usart.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   DebugDongle BSP for USART communication
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
#include <bsp_io.h>
#include <bsp_usart.h>
#include <xpd_nvic.h>

void DMA1_Channel4_5_IRQHandler(void);

DMA_HandleType dmauat;
DMA_HandleType dmauar;

USART_HandleType hvcp_uart, *const vcp_uart = &hvcp_uart;

/* UART dependencies initialization */
static void BSP_VCP_UART_Init(void * handle)
{
    DMA_InitType dmaSetup =
    {
        .Priority                 = MEDIUM,
        .Mode                     = DMA_MODE_NORMAL,
        .Memory.DataAlignment     = DMA_ALIGN_BYTE,
        .Memory.Increment         = ENABLE,
        .Peripheral.DataAlignment = DMA_ALIGN_BYTE,
        .Peripheral.Increment     = DISABLE,
        .Direction                = DMA_MEMORY2PERIPH,
    };

    /* GPIO settings */
    GPIO_vInitPin(UART_TX_PIN, UART_TX_CFG);
    GPIO_vInitPin(UART_RX_PIN, UART_RX_CFG);

    /* DMA settings */
    DMA_vInit(&dmauat, &dmaSetup);

    dmaSetup.Direction = DMA_PERIPH2MEMORY;
    dmaSetup.Mode      = DMA_MODE_CIRCULAR;

    DMA_vInit(&dmauar, &dmaSetup);

    /* Interrupt lines configuration */
    NVIC_SetPriorityConfig(DMA1_Channel4_5_IRQn, 0, 0);
    NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);
}

/* UART dependencies deinitialization */
static void BSP_VCP_UART_Deinit(void * handle)
{
    GPIO_vDeinitPin(UART_TX_PIN);
    GPIO_vDeinitPin(UART_RX_PIN);

    DMA_vDeinit(&dmauat);
    DMA_vDeinit(&dmauar);
    NVIC_DisableIRQ(DMA1_Channel4_5_IRQn);
}

/* UART DMA interrupt handling */
void DMA1_Channel4_5_IRQHandler(void)
{
    DMA_vIRQHandler(&dmauat);
    DMA_vIRQHandler(&dmauar);
}

void BSP_VCP_UART_Bind(void)
{
    USART_INST2HANDLE(vcp_uart, USART2);
    vcp_uart->Callbacks.DepInit = BSP_VCP_UART_Init;
    vcp_uart->Callbacks.DepDeinit = BSP_VCP_UART_Deinit;
    vcp_uart->DMA.Transmit = &dmauat;
    vcp_uart->DMA.Receive  = &dmauar;

    DMA_INST2HANDLE(&dmauat, DMA1_Channel4);
    DMA_INST2HANDLE(&dmauar, DMA1_Channel5);
}
