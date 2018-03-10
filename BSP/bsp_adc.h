/**
  ******************************************************************************
  * @file    bsp_adc.h
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   DebugDongle BSP for ADC conversions
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
#ifndef __BSP_ADC_H_
#define __BSP_ADC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <xpd_adc.h>
#include <xpd_tim.h>
#include <bsp_io.h>

#define ADC_TRIGGER_SRC     ADC_TRIGGER_TIM3_TRGO

extern TIM_HandleType *const adTrg;

extern ADC_HandleType *const adc;

void BSP_ADC_Bind(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ADC_H_ */
