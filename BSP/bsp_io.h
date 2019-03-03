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
#if (HW_REV > 0xA)
#define IOUT_CH             1
#define LIGHT_SENSOR_CH     7
#else
#define LIGHT_SENSOR_CH     1
#endif
#define VBAT_CH             4
#define ICHARGE_CH          9

/* GPIO pins */
#if (HW_REV > 0xA)
#define IOUT_PIN            PA1
#define LIGHT_SENSOR_PIN    PA7
#else
#define LIGHT_SENSOR_PIN    PA1
#endif
#define VBAT_PIN            PA4
#define ICHARGE_PIN         PB1
#if (HW_REV > 0xA)
#define IOUT_CFG            (&BSP_IOCfg[0])
#endif
#define LIGHT_SENSOR_CFG    (&BSP_IOCfg[0])
#define VBAT_CFG            (&BSP_IOCfg[0])
#define ICHARGE_CFG         (&BSP_IOCfg[0])
#if (HW_REV > 0xA)
#define IOUT_CTRL_CFG       (&BSP_IOCfg[3])
#endif

#define CHARGER_CURRENT_PIN PA6
#define CHARGER_CURRENT_CFG (&BSP_IOCfg[3])

#define CHARGER_CTRL_PIN    PF1
#define CHARGER_CTRL_CFG    (&BSP_IOCfg[3])

#define USER_LED_PIN        PB8
#define USER_LED_CFG        (&BSP_IOCfg[4])

#define VOUT_SELECT_PIN     PF0
#define VOUT_SELECT_OUT_CFG (&BSP_IOCfg[3])

/* EXTI mode */
#if (HW_REV > 0xA)
#define VOUT_SELECT         EXTI0_1_IRQ
#define VOUT_SELECT_LINE    0
#define VOUT_SELECT_IN_CFG  (&BSP_IOCfg[2])
#else
#define VOUT_SELECT         EXTI4_15_IRQ
#define VOUT_SELECT_LINE    7
#define MODE_SWITCH_PIN     PA7
#define MODE_SWITCH_CFG     (&BSP_IOCfg[2])
#endif
#define _CONCAT(A,B)        A##B
#define IRQN(LINE)          (_CONCAT(LINE,n))
#define HANDLER(LINE)       void _CONCAT(LINE,Handler)(void)

#define CHARGER_STATUS_PIN  PA5
#define CHARGER_STATUS_CFG  (&BSP_IOCfg[1])

#define USB_PWR_PIN         PA0
#define USB_PWR_CFG         (&BSP_IOCfg[1])

#define UART_TX_PIN         PA2
#define UART_RX_PIN         PA3
#define UART_TX_CFG         (&BSP_IOCfg[5])
#define UART_RX_CFG         (&BSP_IOCfg[5])

#define USB_DP_PIN          PA12
#define USB_DM_PIN          PA11
#define USB_DP_CFG          (&BSP_IOCfg[6])
#define USB_DM_CFG          (&BSP_IOCfg[6])


extern const GPIO_InitType BSP_IOCfg[];

#ifdef __cplusplus
}
#endif

#endif /* __BSP_IO_H_ */
