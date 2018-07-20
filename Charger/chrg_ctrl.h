/**
  ******************************************************************************
  * @file    chrg_ctrl.h
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   DebugDongle charger controls header
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
#ifndef __CHRG_CTRL_H_
#define __CHRG_CTRL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <analog.h>
#include <xpd_usb.h>

typedef enum
{
    Ichg_0mA,
    Ichg_100mA,
    Ichg_500mA,
    Ichg_800mA
}ChargeCurrentType;

typedef enum
{
#if (HW_REV > 0xA)
    Vout_off = 0,
#endif
    Vout_3V3,
    Vout_5V,
}OutputVoltageType;

void Charger_Init(void);
void Charger_SetConfig(void);
void Charger_ClearConfig(void);

int Charger_GetCurrent_mA(void);
int Charger_GetVoltage_mV(void);

void Charger_SetType(USB_ChargerType UsbCharger);
void Charger_SetCurrent(ChargeCurrentType CurrentLevel);

void Output_SetVoltage(OutputVoltageType Voltage);
OutputVoltageType Output_GetVoltage(void);

boolean_t Charger_UsbPowerPresent(void);

#ifdef __cplusplus
}
#endif

#endif /* __CHRG_CTRL_H_ */
