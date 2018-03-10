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
    /* LIGHT_SENSOR_PIN:
     * analog input */
    /* VBAT_PIN:
     * analog input, Vbat = Vmeas * 470 / (130 + 470) */
    /* ICHARGE_PIN:
     * analog input, I = Vmeas * 540 / (1.5 * R=680) */
    {
        .Mode = GPIO_MODE_ANALOG,
        .Pull = GPIO_PULL_FLOAT,
        .AlternateMap = GPIO_ADC_AF,
    },
    /* USB_PWR_PIN:
     * 1 - USB disconnected
     * 0 - USB power present */
    {
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_PULL_FLOAT,
    },
    /* MODE_SWITCH_PIN:
     * ON  = GND
     * OFF = PULL */
    {
        .Mode = GPIO_MODE_EXTI,
        .Pull = GPIO_PULL_UP,
        .ExtI = {
            .Edge       = EDGE_RISING_FALLING,
            .Reaction   = REACTION_IT,
        },
    },
    /* CHARGER_CURRENT_PIN:
     * 1 - 500mA limit
     * 0 - limit based on resistor (800mA)
     * X - 100mA limit */
    /* CHARGER_CTRL_PIN:
     * 1 - Charger is enabled
     * 0 - Charger is disabled */
    /* VOUT_SELECT_PIN:
     * 1 - 5V
     * 0 - 3.3V */
    {
        .Mode = GPIO_MODE_OUTPUT,
        .Pull = GPIO_PULL_FLOAT,
        .Output.Type  = GPIO_OUTPUT_PUSHPULL,
        .Output.Speed = HIGH,
    },
    /* USER_LED_PIN:
     * 0 - LED ON */
    /* CHARGER_STATUS_PIN:
     * 1 - Charger inactive
     * 0 - Charging ongoing
     * Set to 0 to turn on red LED */
    {
        .Mode = GPIO_MODE_OUTPUT,
        .Pull = GPIO_PULL_FLOAT,
        .Output.Type  = GPIO_OUTPUT_OPENDRAIN,
        .Output.Speed = LOW,
    },
    /* UART_TX_PIN:
     * used for UART Tx */
    /* UART_RX_PIN:
     * used for UART Rx */
    {
        .Mode = GPIO_MODE_ALTERNATE,
        .Pull = GPIO_PULL_FLOAT,
        .Output.Type  = GPIO_OUTPUT_PUSHPULL,
        .Output.Speed = VERY_HIGH,
        .AlternateMap = GPIO_USART2_AF1
    },
    /* USB_DP_PIN:
     * used for USB */
    /* USB_DM_PIN:
     * used for USB */
    {
        .Mode = GPIO_MODE_ALTERNATE,
        .Pull = GPIO_PULL_FLOAT,
        .Output.Type  = GPIO_OUTPUT_PUSHPULL,
        .Output.Speed = VERY_HIGH,
        .AlternateMap = GPIO_USB_AF2
    },
};

void EXTI4_15_IRQHandler(void);

void EXTI4_15_IRQHandler(void)
{
    EXTI_vIRQHandler(MODE_SWITCH_LINE);
}
