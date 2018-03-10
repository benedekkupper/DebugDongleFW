/**
  ******************************************************************************
  * @file    bsp_adc.c
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
#include <bsp_adc.h>
#include <xpd_nvic.h>

DMA_HandleType hdmaadc, *const dmaadc = &hdmaadc;

TIM_HandleType hadTrg, *const adTrg = &hadTrg;

ADC_HandleType hadc, *const adc = &hadc;

static const DMA_InitType dmaSetup =
{
    .Priority                 = LOW,
    .Mode                     = DMA_MODE_CIRCULAR,
    .Memory.DataAlignment     = DMA_ALIGN_HALFWORD,
    .Memory.Increment         = ENABLE,
    .Peripheral.DataAlignment = DMA_ALIGN_HALFWORD,
    .Peripheral.Increment     = DISABLE,
    .Direction                = DMA_PERIPH2MEMORY,
};

static void adcinit(void * handle)
{
    /* Clock settings - Max 14Mhz is allowed */
    ADC_vClockConfig(ADC_CLOCKSOURCE_PCLK_DIV4);

    /* GPIO settings */
    GPIO_vInitPin(LIGHT_SENSOR_PIN, LIGHT_SENSOR_CFG);
    GPIO_vInitPin(VBAT_PIN,         VBAT_CFG);
    GPIO_vInitPin(ICHARGE_PIN,      ICHARGE_CFG);

    /* DMA settings */
    DMA_vInit(dmaadc, &dmaSetup);

    NVIC_SetPriorityConfig(DMA1_Channel1_IRQn, 0, 3);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

static void adcdeinit(void * handle)
{
    TIM_vCounterStop(adTrg);

    DMA_vDeinit(dmaadc);
    NVIC_DisableIRQ(DMA1_Channel1_IRQn);
}

void DMA1_Channel1_IRQHandler(void)
{
    DMA_vIRQHandler(dmaadc);
}

void BSP_ADC_Bind(void)
{
    ADC_INST2HANDLE(adc, ADC1);
    adc->Callbacks.DepInit = adcinit;
    adc->Callbacks.DepDeinit = adcdeinit;
    adc->DMA.Conversion = dmaadc;

    DMA_INST2HANDLE(dmaadc, DMA1_Channel1);
    TIM_INST2HANDLE(adTrg, TIM3);
}
