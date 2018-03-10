/**
  ******************************************************************************
  * @file    bsp_io.h
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
#ifndef __BSP_IO_H_
#define __BSP_IO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <xpd_gpio.h>

/* ADC channels */
#define LIGHT_SENSOR_CH     1
#define VBAT_CH             4
#define ICHARGE_CH          9

/* GPIO pins */
#define LIGHT_SENSOR_PIN    GPIOA, 1
#define VBAT_PIN            GPIOA, 4
#define ICHARGE_PIN         GPIOB, 1
#define LIGHT_SENSOR_CFG    (&BSP_IOCfg[0])
#define VBAT_CFG            (&BSP_IOCfg[0])
#define ICHARGE_CFG         (&BSP_IOCfg[0])

#define CHARGER_CURRENT_PIN GPIOA, 6
#define CHARGER_CURRENT_CFG (&BSP_IOCfg[3])

#define CHARGER_CTRL_PIN    GPIOF, 1
#define CHARGER_CTRL_CFG    (&BSP_IOCfg[3])

#define USER_LED_PIN        GPIOB, 8
#define USER_LED_CFG        (&BSP_IOCfg[4])

#define VOUT_SELECT_PIN     GPIOF, 0
#define VOUT_SELECT_CFG     (&BSP_IOCfg[3])

#define CHARGER_STATUS_PIN  GPIOA, 5
#define CHARGER_STATUS_CFG  (&BSP_IOCfg[4])

#define USB_PWR_PIN         GPIOA, 0
#define USB_PWR_CFG         (&BSP_IOCfg[1])

/* EXTI mode */
#define MODE_SWITCH_IRQN    EXTI4_15_IRQn
#define MODE_SWITCH_LINE    7
#define MODE_SWITCH_PIN     GPIOA, 7
#define MODE_SWITCH_CFG     (&BSP_IOCfg[2])

#define UART_TX_PIN         GPIOA, 2
#define UART_RX_PIN         GPIOA, 3
#define UART_TX_CFG         (&BSP_IOCfg[5])
#define UART_RX_CFG         (&BSP_IOCfg[5])

#define USB_DP_PIN          GPIOA, 12
#define USB_DM_PIN          GPIOA, 11
#define USB_DP_CFG          (&BSP_IOCfg[6])
#define USB_DM_CFG          (&BSP_IOCfg[6])


extern const GPIO_InitType BSP_IOCfg[];

#ifdef __cplusplus
}
#endif

#endif /* __BSP_IO_H_ */
