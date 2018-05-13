/**
  ******************************************************************************
  * @file    bsp_io.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   DebugDongle BSP for I/O pins
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

const GPIO_InitType BSP_IOCfg[] =
{
    /* analog input */
    {
        .Mode = GPIO_MODE_ANALOG,
        .Pull = GPIO_PULL_FLOAT,
        .AlternateMap = GPIO_ADC_AF,
    },
    /* floating input */
    {
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_PULL_FLOAT,
    },
    /* floating EXTI input */
    {
        .Mode = GPIO_MODE_EXTI,
        .Pull = GPIO_PULL_FLOAT,
        .ExtI = {
            .Edge       = EDGE_RISING_FALLING,
            .Reaction   = REACTION_IT,
        },
    },
    /* floating output */
    {
        .Mode = GPIO_MODE_OUTPUT,
        .Pull = GPIO_PULL_FLOAT,
        .Output.Type  = GPIO_OUTPUT_PUSHPULL,
        .Output.Speed = HIGH,
    },
    /* floating OD output */
    {
        .Mode = GPIO_MODE_OUTPUT,
        .Pull = GPIO_PULL_FLOAT,
        .Output.Type  = GPIO_OUTPUT_OPENDRAIN,
        .Output.Speed = LOW,
    },
    /* UART */
    {
        .Mode = GPIO_MODE_ALTERNATE,
        .Pull = GPIO_PULL_FLOAT,
        .Output.Type  = GPIO_OUTPUT_PUSHPULL,
        .Output.Speed = VERY_HIGH,
        .AlternateMap = GPIO_USART2_AF1
    },
    /* USB */
    {
        .Mode = GPIO_MODE_ALTERNATE,
        .Pull = GPIO_PULL_FLOAT,
        .Output.Type  = GPIO_OUTPUT_PUSHPULL,
        .Output.Speed = VERY_HIGH,
        .AlternateMap = GPIO_USB_AF2
    },
};

void EXTI0_1_IRQHandler(void);

void EXTI0_1_IRQHandler(void)
{
    EXTI_vIRQHandler(VOUT_SELECT_LINE);
}
