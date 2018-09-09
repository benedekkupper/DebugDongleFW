/**
  ******************************************************************************
  * @file    sens_if.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   Sensor HID interface implementation
  *
  *  @verbatim
  *
  * ===================================================================
  *                       USB Sensor Collection
  * ===================================================================
  *  This interface implements a USB Sensor Collection
  *  control using the Power Device HID page definitions. The IN report
  *  contains the battery voltage and current measurements as well as
  *  status flags. Separate Feature reports are available to get the
  *  USB input voltage and to get and set the Vout voltage, the
  *  charging current and the nominal battery capacity. Feature reports
  *  can be transferred only via the control endpoint.
  *  @endverbatim
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
#include <sens_if.h>
#include <analog.h>
#include <hid/usage_sensor.h>
#include <string.h>

#define REPORT_INTERVAL         100

#define TEMP_SCALER             100

#define SENR_TEMP
#define SENR_LIGHT
#define SENR_VOLT

/** @brief HID report descriptor of sens_if */
__ALIGN_BEGIN static const uint8_t SensorReport[] __ALIGN_END =
{
#if 1
    /* Sensor usage page context */
    HID_USAGE_PAGE_SENSOR,
    HID_USAGE_SENSOR_TYPE_COLLECTION,
    HID_COLLECTION_APPLICATION(

#ifdef SENR_TEMP
        /* Temperature */
        HID_USAGE_SENSOR_TYPE_ENVIRONMENTAL_TEMPERATURE,
        HID_COLLECTION_PHYSICAL(

#ifdef SENR_TEMP_REPSTATE
            /* Reporting state property */
            HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(5),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION_LOGICAL,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_WAKE,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_WAKE,
                HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_WAKE,
                HID_FEATURE(Data_Arr_Abs),
            HID_END_COLLECTION,
#endif /* SENR_TEMP_REPSTATE */

            /* Report interval property */
            HID_USAGE_SENSOR_PROPERTY_REPORT_INTERVAL,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_32(0xFFFFFFFF),
            HID_REPORT_SIZE(32),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),

            /* Modifier property of temperature (maximum value) */
            HID_USAGE_SENSOR_DATA(
                    HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE,
                    HID_USAGE_SENSOR_DATA_MOD_MAX),
            HID_LOGICAL_MIN_16(-32767),
            HID_LOGICAL_MAX_16(32767),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(-2),
            HID_FEATURE(Data_Var_Abs),

            /* Modifier property of temperature (minimum value) */
            HID_USAGE_SENSOR_DATA(
                    HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE,
                    HID_USAGE_SENSOR_DATA_MOD_MIN),
            HID_LOGICAL_MIN_16(-32767),
            HID_LOGICAL_MAX_16(32767),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(-2),
            HID_FEATURE(Data_Var_Abs),

#ifdef SENR_TEMP_STATE
            /* Sensor state (global) */
            HID_USAGE_SENSOR_STATE,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(6),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION_LOGICAL,
                HID_USAGE_SENSOR_STATE_UNKNOWN,
                HID_USAGE_SENSOR_STATE_READY,
                HID_USAGE_SENSOR_STATE_NOT_AVAILABLE,
                HID_USAGE_SENSOR_STATE_NO_DATA,
                HID_USAGE_SENSOR_STATE_INITIALIZING,
                HID_USAGE_SENSOR_STATE_ACCESS_DENIED,
                HID_USAGE_SENSOR_STATE_ERROR,
                HID_INPUT(Data_Arr_Abs),
            HID_END_COLLECTION,
#endif /* SENR_TEMP_STATE */

#ifdef SENR_TEMP_EVENT
            /* Sensor event settings */
            HID_USAGE_SENSOR_EVENT,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_8(16),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(1),
            HID_COLLECTION_LOGICAL,
                HID_USAGE_SENSOR_EVENT_UNKNOWN,
                HID_USAGE_SENSOR_EVENT_STATE_CHANGED,
                HID_USAGE_SENSOR_EVENT_PROPERTY_CHANGED,
                HID_USAGE_SENSOR_EVENT_DATA_UPDATED,
                HID_USAGE_SENSOR_EVENT_POLL_RESPONSE,
                HID_USAGE_SENSOR_EVENT_CHANGE_SENSITIVITY,
                HID_USAGE_SENSOR_EVENT_MAX_REACHED,
                HID_USAGE_SENSOR_EVENT_MIN_REACHED,
                HID_USAGE_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_UPWARD,
                HID_USAGE_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_DOWNWARD,
                HID_USAGE_SENSOR_EVENT_LOW_THRESHOLD_CROSS_UPWARD,
                HID_USAGE_SENSOR_EVENT_LOW_THRESHOLD_CROSS_DOWNWARD,
                HID_USAGE_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_UPWARD,
                HID_USAGE_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_DOWNWARD,
                HID_USAGE_SENSOR_EVENT_PERIOD_EXCEEDED,
                HID_USAGE_SENSOR_EVENT_FREQUENCY_EXCEEDED,
                HID_USAGE_SENSOR_EVENT_COMPLEX_TRIGGER,
                HID_INPUT(Data_Arr_Abs),
            HID_END_COLLECTION,
#endif /* SENR_TEMP_EVENT */

            /* Temperature sensor input */
            HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE,
            HID_LOGICAL_MIN_16(-32767),
            HID_LOGICAL_MAX_16(32767),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(-2),
            HID_UNIT_KELVIN,
            /* HID defined unit used */
            HID_INPUT(Data_Var_Abs),

        ),
#endif /* SENR_TEMP */

#ifdef SENR_LIGHT
        /* Ambient light */
        HID_USAGE_SENSOR_TYPE_LIGHT_AMBIENTLIGHT,
        HID_COLLECTION_PHYSICAL(

            /* Report interval property */
            HID_USAGE_SENSOR_PROPERTY_REPORT_INTERVAL,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_32(0xFFFFFFFF),
            HID_REPORT_SIZE(32),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),

            /* Modifier property of illuminance (maximum value) */
            HID_USAGE_SENSOR_DATA(
                    HID_USAGE_SENSOR_DATA_LIGHT_ILLUMINANCE,
                    HID_USAGE_SENSOR_DATA_MOD_MAX),
            HID_LOGICAL_MIN_16(0),
            HID_LOGICAL_MAX_16(0xFFFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),

            /* Modifier property of illuminance (minimum value) */
            HID_USAGE_SENSOR_DATA(
                    HID_USAGE_SENSOR_DATA_LIGHT_ILLUMINANCE,
                    HID_USAGE_SENSOR_DATA_MOD_MIN),
            HID_LOGICAL_MIN_16(0),
            HID_LOGICAL_MAX_16(0xFFFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),

            /* Illuminance sensor input */
            HID_USAGE_SENSOR_DATA_LIGHT_ILLUMINANCE,
            HID_LOGICAL_MIN_16(0),
            HID_LOGICAL_MAX_16(0xFFFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_UNIT_LUX,
            HID_INPUT(Data_Var_Abs),

        ),
#endif /* SENR_LIGHT */

#ifdef SENR_VOLT
        /* System voltage */
        HID_USAGE_SENSOR_TYPE_ELECTRICAL_VOLTAGE,
        HID_COLLECTION_PHYSICAL(

            /* Report interval property */
            HID_USAGE_SENSOR_PROPERTY_REPORT_INTERVAL,
            HID_LOGICAL_MIN_8(0),
            HID_LOGICAL_MAX_32(0xFFFFFFFF),
            HID_REPORT_SIZE(32),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(0),
            HID_FEATURE(Data_Var_Abs),

            /* Modifier property of voltage (maximum value) */
            HID_USAGE_SENSOR_DATA(
                    HID_USAGE_SENSOR_DATA_ELECTRICAL_VOLTAGE,
                    HID_USAGE_SENSOR_DATA_MOD_MAX),
            HID_LOGICAL_MIN_16(0),
            HID_LOGICAL_MAX_16(0xFFFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(-3),
            HID_FEATURE(Data_Var_Abs),

            /* Modifier property of voltage (minimum value) */
            HID_USAGE_SENSOR_DATA(
                    HID_USAGE_SENSOR_DATA_ELECTRICAL_VOLTAGE,
                    HID_USAGE_SENSOR_DATA_MOD_MIN),
            HID_LOGICAL_MIN_16(0),
            HID_LOGICAL_MAX_16(0xFFFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(-3),
            HID_FEATURE(Data_Var_Abs),

            /* Voltage sensor input */
            HID_USAGE_SENSOR_DATA_ELECTRICAL_VOLTAGE,
            HID_LOGICAL_MIN_16(0),
            HID_LOGICAL_MAX_16(0xFFFF),
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_UNIT_EXPONENT(-3),
            HID_UNIT_VOLT,
            HID_INPUT(Data_Var_Abs),

        ),
#endif /* SENR_VOLT */

    ),
#endif /* 1 */
};

/** @brief HID Input report */
typedef struct {
#ifdef SENR_TEMP
#ifdef SENR_TEMP_REPSTATE
    uint8_t repstate;
#endif
#ifdef SENR_TEMP_STATE
    uint8_t state;
#endif
#ifdef SENR_TEMP_EVENT
    uint8_t event;
#endif
    int16_t temp;
#endif
#ifdef SENR_LIGHT
    uint16_t illum;
#endif
#ifdef SENR_VOLT
    uint16_t volt;
#endif
}__packed Sensor_InReportType;

/** @brief HID Feature report */
struct {
#ifdef SENR_TEMP
    struct {
        uint32_t interval;
        int16_t max;
        int16_t min;
    }temp;
#endif
#ifdef SENR_LIGHT
    struct {
        uint32_t interval;
        uint16_t max;
        uint16_t min;
    }illum;
#endif
#ifdef SENR_VOLT
    struct {
        uint32_t interval;
        uint16_t max;
        uint16_t min;
    }volt;
#endif
}__packed sens_feature = {
#ifdef SENR_TEMP
    { REPORT_INTERVAL, 150 * TEMP_SCALER, -50 * TEMP_SCALER },
#endif
#ifdef SENR_LIGHT
    { REPORT_INTERVAL, 10000, 0 },
#endif
#ifdef SENR_VOLT
    { REPORT_INTERVAL, 10 * 1000, 0 },
#endif
};

/**
 * @brief Sends the IN report
 */
static void Sensor_SendInput(void)
{
    Sensor_InReportType sens_input;
    const AnalogMeasurementsType * meas = Analog_GetValues();

#ifdef SENR_TEMP
    sens_input.temp  = ( int16_t)meas->temp_C * TEMP_SCALER;
#endif
#ifdef SENR_LIGHT
    sens_input.illum = (uint16_t)meas->light_lx;
#endif
#ifdef SENR_VOLT
    sens_input.volt  = (uint16_t)meas->Vdd_mV;
#endif

    USBD_HID_ReportIn(sens_if, (uint8_t*)&sens_input, sizeof(sens_input));
}
/**
 * @brief Sends the Feature report through the control EP.
 * @param itf: callback sender interface
 * @param type: requested report's type
 * @param reportId: unused
 */
static void Sensor_GetReport(void* itf, USBD_HID_ReportType type, uint8_t reportId)
{
    if (type == HID_REPORT_INPUT)
    {
        /* Update IN report and send through Ctrl pipe */
        Sensor_SendInput();
    }
    else
    {
        USBD_HID_ReportIn(itf, (uint8_t*)&sens_feature, sizeof(sens_feature));
    }
}

/**
 * @brief Sets the new value of the Feature report based on the received data.
 * @param itf: callback sender interface
 * @param type: report type (here always FEATURE)
 * @param data: the new report value to set
 * @param length: size of the report
 */
static void Sensor_SetReport(void* itf, USBD_HID_ReportType type, uint8_t * data, uint16_t length)
{
    memcpy((uint8_t*)&sens_feature, data, length);
}

/**
 * @brief Provides the measured input report data for transmission
 */
void Sensor_Periodic(void)
{
    if (sens_if->Base.Device->ConfigSelector != 0)
    {
        static uint8_t msCounter = 0;

        if (++msCounter >= REPORT_INTERVAL)
        {
            Sensor_SendInput();
            msCounter = 0;
        }
    }
}

/** @brief Sensors HID Application */
const USBD_HID_AppType sensApp =
{
    .Name       = "DebugDongle Sensor Collection",
    .Init       = Analog_Resume,
    .Deinit     = Analog_Halt,
    .SetReport  = Sensor_SetReport,
    .GetReport  = Sensor_GetReport,
    .Report     = {
            .Desc = SensorReport,
            .Length = sizeof(SensorReport),
    },
};

/** @brief Sensors HID Interface (and reference) */
USBD_HID_IfHandleType hsens_if = {
    .App = &sensApp,
    .Base.AltCount = 1,
    .Config.InEp.Size = sizeof(Sensor_InReportType),
    .Config.InEp.Interval_ms = REPORT_INTERVAL,
}, *const sens_if = &hsens_if;



