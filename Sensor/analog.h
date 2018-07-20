/**
  ******************************************************************************
  * @file    analog.h
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   DebugDongle analog measurements header
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
#ifndef ANALOG_H_
#define ANALOG_H_

#include <stdint.h>

typedef struct
{
    int32_t Vdd_mV;
    int32_t Vbat_mV;
    int32_t Ichrg_mA;
    int32_t Iout_mA;
    int32_t temp_C;
    int32_t light_lx;
}AnalogMeasurementsType;

void Analog_Init(void);
void Analog_Deinit(void);
#if (HW_REV > 0xA)
void Analog_IoutConfig(int Enabled);
#endif
void Analog_Halt(void);
void Analog_Resume(void);
const AnalogMeasurementsType * Analog_GetValues(void);

#endif /* ANALOG_H_ */
