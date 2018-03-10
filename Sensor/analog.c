/**
  ******************************************************************************
  * @file    analog.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   DebugDongle analog measurements implementation
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
#include <analog.h>
#include <bsp_adc.h>

/** @brief ADC peripheral settings */
static const ADC_InitType adcSettings =
{
    .ContinuousDMARequests  = ENABLE,
    .ContinuousMode         = DISABLE,
    .DiscontinuousCount     = 0,
    .EndFlagSelection       = ADC_EOC_SEQUENCE,
    .LeftAlignment          = DISABLE,
    .Resolution             = ADC_RESOLUTION_12BIT,
    .ScanDirection          = ADC_SCAN_FORWARD,
    .Trigger.Source         = ADC_TRIGGER_SRC,
    .Trigger.Edge           = EDGE_RISING,
    .LPAutoWait             = DISABLE,
    .LPAutoPowerOff         = ENABLE,
};

/* On STM32F0 channel order is according to the channel number */
typedef enum
{
    ADCH_LIGHT_SENSOR = 0,
    ADCH_VBAT,
    ADCH_ICHARGE,
    ADCH_TEMP,
    ADCH_VREFINT,
    ADCH_COUNT
}ADCHType;

/** @brief ADC channels configuration */
static const ADC_ChannelInitType adcChannels[] =
{
    {
        .Number     = LIGHT_SENSOR_CH,
        .SampleTime = ADC_SAMPLETIME_239p5
    },
    {
        .Number     = VBAT_CH,
        .SampleTime = ADC_SAMPLETIME_239p5
    },
    {
        .Number     = ICHARGE_CH,
        .SampleTime = ADC_SAMPLETIME_239p5
    },
    {
        .Number     = ADC1_TEMPSENSOR_CHANNEL,
        .SampleTime = ADC_SAMPLETIME_239p5
    },
    {
        .Number     = ADC1_VREFINT_CHANNEL,
        .SampleTime = ADC_SAMPLETIME_239p5
    },
};

static uint16_t conversions[ADCH_COUNT];
static AnalogMeasurementsType measurements;

/**
 * @brief Provide measurement results.
 * @return reference of the measured values
 */
const AnalogMeasurementsType * Analog_GetValues(void)
{
    return &measurements;
}

/**
 * @brief Convert the ADC conversions into physical measurement values
 *        after the end of a conversion sequence.
 * @param handle: unused
 */
static void analogConvertMeasured(void * handle)
{
    /* Internal channels converted with driver */
    measurements.Vdd_mV   = ADC_lCalcVDDA_mV(conversions[ADCH_VREFINT]);
    measurements.temp_C   = ADC_lCalcTemp_C(conversions[ADCH_TEMP]);

    /* Voltage divider: Vmeas = Vbat * R2=470 / (R1=130 + R2=470) */
    measurements.Vbat_mV  = ADC_lCalcExt_mV(conversions[ADCH_VBAT]) * (130 + 470) / 470;

    /*  I = Vmeas * 540 / (1.5 * R=680) */
    measurements.Ichrg_mA = ADC_lCalcExt_mV(conversions[ADCH_ICHARGE]) * 54 / 102;

    /* lx = (Vmeas / R=10K) * 500 / 300 */
    measurements.light_lx = ADC_lCalcExt_mV(conversions[ADCH_LIGHT_SENSOR]) * 5 / 30;
}

/**
 * @brief Initializes the ADC, its trigger timer and the DMA transfer.
 */
void Analog_Init(void)
{
    ADC_vInit(adc, &adcSettings);
    ADC_eCalibrate(adc, FALSE);

    ADC_vChannelConfig(adc, adcChannels, sizeof(adcChannels)/sizeof(adcChannels[0]));

    adc->Callbacks.ConvComplete = analogConvertMeasured;

    /* trigger timer settings */
    {
        TIM_InitType stp;
        TIM_MasterConfigType mstr;
        stp.Prescaler           = TIM_ulClockFreq_Hz(adTrg) / 1000; /* clock at 1 kHz */
        stp.Period              = 10; /* trigger with 100 Hz */
        stp.Mode                = TIM_COUNTER_UP;
        stp.ClockDivision       = CLK_DIV1;
        stp.RepetitionCounter   = 0;
        TIM_vInit(adTrg, &stp);

        mstr.MasterSlaveMode    = DISABLE;
        mstr.MasterTrigger      = TIM_TRGO_UPDATE;
        TIM_vMasterConfig(adTrg, &mstr);
    }

    /* Actual conversions only start by the trigger */
    ADC_eStart_DMA(adc, conversions);
}

/**
 * @brief Shuts down the peripherals related to analog measurements.
 */
void Analog_Deinit(void)
{
    TIM_vDeinit(adTrg);
    ADC_vDeinit(adc);
}

/**
 * @brief Halts measurements.
 */
void Analog_Halt(void)
{
    TIM_vCounterStop(adTrg);
}

/**
 * @brief Resumes measurements.
 */
void Analog_Resume(void)
{
    TIM_vCounterStart(adTrg);
}
