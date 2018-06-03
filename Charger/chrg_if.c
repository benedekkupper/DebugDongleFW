/**
  ******************************************************************************
  * @file    chrg_if.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   Power Supply and Battery Charger HID interface implementation
  *
  *  @verbatim
  *
  * ===================================================================
  *                USB Power Supply and Charger Control
  * ===================================================================
  *  This interface implements a Power Supply and Battery Charger
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
#include <chrg_if.h>
#include <hid_usage_power.h>

static const uint16_t LiCharged_mV = 4200;
static const uint16_t LiDischarge_mV = 2900;

/** @brief HID report descriptor of chrg_if */
__ALIGN_BEGIN static const uint8_t ChargerReport[] __ALIGN_END =
{
#if 1
HID_USAGE_PAGE_POWER_DEVICE,
    HID_USAGE_PS_UPS,
    HID_COLLECTION_APPLICATION,

        /* USB input */
        HID_USAGE_PS_INPUT,
        HID_COLLECTION_PHYSICAL,

            HID_REPORT_ID(1),

            /* nominal voltage */
            HID_USAGE_PS_CONFIGVOLTAGE,
            HID_REPORT_SIZE(16),
            HID_REPORT_COUNT(1),
            HID_LOGICAL_MIN_16(0),
            HID_LOGICAL_MAX_16(6000),
            HID_UNIT_VOLT,
            HID_UNIT_EXPONENT(-3),
            HID_FEATURE(Const_Var_Abs),

            /* TODO Good, Overload */

        HID_END_COLLECTION,

        /* Power converter */
        HID_USAGE_PS_POWER_CONVERTER,
        HID_COLLECTION_PHYSICAL,

            HID_REPORT_ID(2),

            /* output */
            HID_USAGE_PS_OUTPUT,
            HID_COLLECTION_PHYSICAL,

                /* output voltage */
                HID_USAGE_PS_CONFIGVOLTAGE,
                HID_REPORT_SIZE(16),
                HID_REPORT_COUNT(1),
                HID_LOGICAL_MIN_16(0),
                HID_LOGICAL_MAX_16(6000),
                HID_UNIT_VOLT,
                HID_UNIT_EXPONENT(-3),
                HID_FEATURE(Const_Var_Abs),

                HID_USAGE_PS_USED,
                HID_USAGE_PS_BUCK,
                HID_REPORT_SIZE(1),
                HID_REPORT_COUNT(2),
                HID_LOGICAL_MIN_8(0),
                HID_LOGICAL_MAX_8(1),
                HID_FEATURE(Const_Var_Abs),

                /* Padding */
                HID_REPORT_SIZE(1),
                HID_REPORT_COUNT(6),
                HID_LOGICAL_MIN_8(0),
                HID_LOGICAL_MAX_8(1),
                HID_FEATURE(Const_Arr_Abs),

                /* Measured output current */
                HID_USAGE_PS_CURRENT,
                HID_REPORT_SIZE(16),
                HID_REPORT_COUNT(1),
                HID_LOGICAL_MIN_16(0),
                HID_LOGICAL_MAX_16(1000),
                HID_UNIT_AMPERE,
                HID_UNIT_EXPONENT(-3),
                HID_INPUT(Const_Var_Abs),

            HID_END_COLLECTION,

        HID_END_COLLECTION,

        /* TODO Power summary */

        /* Battery charging */
        HID_USAGE_PS_BATTERY_SYSTEM,
        HID_COLLECTION_PHYSICAL,

            HID_USAGE_PS_CHARGER,
            HID_COLLECTION_PHYSICAL,

                HID_REPORT_ID(3),

                /* desired charge current */
                HID_USAGE_PS_CONFIGCURRENT,
                HID_REPORT_SIZE(16),
                HID_REPORT_COUNT(1),
                HID_LOGICAL_MIN_16(0),
                HID_LOGICAL_MAX_16(1000),
                HID_UNIT_AMPERE,
                HID_UNIT_EXPONENT(-3),
                HID_FEATURE(Const_Var_Abs),

            HID_END_COLLECTION,

            HID_USAGE_PS_BATTERY,
            HID_COLLECTION_PHYSICAL,

                HID_REPORT_ID(4),

                /* Measured battery voltage */
                HID_USAGE_PS_VOLTAGE,
                HID_REPORT_SIZE(16),
                HID_REPORT_COUNT(1),
                HID_LOGICAL_MIN_16(0),
                HID_LOGICAL_MAX_16(4500),
                HID_UNIT_VOLT,
                HID_UNIT_EXPONENT(-3),
                HID_INPUT(Const_Var_Abs),

                /* Measured battery charge current */
                HID_USAGE_PS_CURRENT,
                HID_REPORT_SIZE(16),
                HID_REPORT_COUNT(1),
                HID_LOGICAL_MIN_16(0),
                HID_LOGICAL_MAX_16(1000),
                HID_UNIT_AMPERE,
                HID_UNIT_EXPONENT(-3),
                HID_INPUT(Const_Var_Abs),

HID_USAGE_PAGE_BATTERY_SYSTEM,
                /* battery capacities */
                HID_USAGE_BS_DESIGN_CAP,
                HID_REPORT_SIZE(16),
                HID_REPORT_COUNT(1),
                HID_LOGICAL_MIN_16(0),
                HID_LOGICAL_MAX_16(1000),
                HID_UNIT_AMPERE_PER_SEC,
                HID_UNIT_EXPONENT(-3),
                HID_FEATURE(Const_Var_Abs),

                HID_USAGE_BS_REMAINING_CAP,
                HID_REPORT_SIZE(16),
                HID_REPORT_COUNT(1),
                HID_LOGICAL_MIN_16(0),
                HID_LOGICAL_MAX_16(1000),
                HID_UNIT_AMPERE_PER_SEC,
                HID_UNIT_EXPONENT(-3),
                HID_INPUT(Const_Var_Abs),

                HID_USAGE_BS_FULLY_CHARGED,
                HID_USAGE_BS_FULLY_DISCHARGED,
HID_USAGE_PAGE_POWER_DEVICE,
                HID_USAGE_PS_PRESENT,
                HID_USAGE_PS_OVERTEMP,
                HID_REPORT_SIZE(1),
                HID_REPORT_COUNT(4),
                HID_LOGICAL_MIN_8(0),
                HID_LOGICAL_MAX_8(1),
                HID_INPUT(Const_Var_Abs | Volatile_Flag),

                /* Padding */
                HID_REPORT_SIZE(1),
                HID_REPORT_COUNT(4),
                HID_LOGICAL_MIN_8(0),
                HID_LOGICAL_MAX_8(1),
                HID_INPUT(Const_Arr_Abs),

            HID_END_COLLECTION,

        HID_END_COLLECTION,

    HID_END_COLLECTION,
#endif /* 1 */
};

static uint8_t inputsel = 0;

/** @brief HID IN report #2 internal layout */
typedef struct {
    uint16_t mA;
}__packed Charger_InOutputType;

/** @brief HID IN report #2 buffer */
struct {
    uint8_t id;
    Charger_InOutputType output;
}__packed vout_input = {
    .id = 2,
};

/** @brief HID IN report #4 internal layout */
typedef struct {
    uint16_t mV;
    uint16_t mA;
    uint16_t remcap;
    union {
        struct {
            uint8_t charged : 1;
            uint8_t discharged : 1;
            uint8_t present : 1;
            uint8_t overheat : 1;
            uint8_t : 4;
        };
        uint8_t b;
    };
}__packed Charger_InBatteryType;

/** @brief HID IN report #4 buffer */
struct {
    uint8_t id;
    Charger_InBatteryType battery;
}__packed chrg_input = {
    .id = 4,
    .battery.b = 0,
};

/** @brief HID Feature report #1 layout */
typedef struct {
    uint16_t mV;
}__packed Charger_FtUsbType;

/** @brief HID Feature report #2 layout */
typedef struct {
    uint16_t mV;
    union {
        struct {
            uint8_t used : 1;
            uint8_t buck : 1;
            uint8_t : 6;
        };
        uint8_t b;
    };
}__packed Charger_FtOutType;

/** @brief HID Feature report #3 layout */
typedef struct {
    uint16_t mA;
}__packed Charger_FtChargerType;

/** @brief HID Feature report #4 layout */
typedef struct {
    uint16_t capacity;
}__packed Charger_FtBatteryType;

/** @brief HID Feature reports buffer */
struct {
    Charger_FtUsbType usb;
    Charger_FtOutType out;
    Charger_FtChargerType charger;
    Charger_FtBatteryType battery;
}__packed chrg_feature = {
    .usb.mV = 5000,
    .out.mV = 5000,
    .out.buck = 0,
    .out.used = 1,
    .charger.mA = 100,
    .battery.capacity = 0,
};

/**
 * @brief Returns a requested feature report (through the CTRL endpoint).
 * @param reportId: The feature report's ID
 */
static void Charger_GetReport(uint8_t reportId)
{
    switch (reportId)
    {
        case 1:
        {
            USBD_HID_ReportIn(chrg_if,
                    (uint8_t*)&chrg_feature.usb,
                    sizeof(chrg_feature.usb));
            break;
        }
        case 2:
        {
            OutputVoltageType conf = Output_GetVoltage();

            if (conf == Vout_off)
            {
                /* Not possible to set with switch, values are set */
            }
            else if (conf == Vout_5V)
            {
                chrg_feature.out.buck = 0;
                chrg_feature.out.mV = 5000;
            }
            else
            {
                chrg_feature.out.buck = 1;
                chrg_feature.out.mV = (uint16_t)Analog_GetValues()->Vdd_mV;
            }

            USBD_HID_ReportIn(chrg_if,
                    (uint8_t*)&chrg_feature.out,
                    sizeof(chrg_feature.out));
            break;
        }
        case 3:
        {
            USBD_HID_ReportIn(chrg_if,
                    (uint8_t*)&chrg_feature.charger,
                    sizeof(chrg_feature.charger));
            break;
        }
        case 4:
        {
            USBD_HID_ReportIn(chrg_if,
                    (uint8_t*)&chrg_feature.battery,
                    sizeof(chrg_feature.battery));
            break;
        }
        default: /* Invalid ID, return the entire feature report */
        {
            USBD_HID_ReportIn(chrg_if,
                    (uint8_t*)&chrg_feature,
                    sizeof(chrg_feature));
            break;
        }
    }
}

/**
 * @brief Applies the output feature report's parameters on the device.
 * @param out: the input report
 */
static void Charger_SetOutReport(Charger_FtOutType *out)
{
    /* output voltage change:
     * 5V if voltage is higher than 4.5V
     * and Buck converter is disabled on output */
    if (out->used == 0)
    {
        Output_SetVoltage(Vout_off);
        chrg_feature.out.mV = 0;
        chrg_feature.out.buck = 0;
        chrg_feature.out.used = 0;
    }
    else if ((out->mV > 4500) && (out->buck == 0))
    {
        Output_SetVoltage(Vout_5V);
        chrg_feature.out.mV = 5000;
        chrg_feature.out.buck = 0;
        chrg_feature.out.used = 1;
    }
    else
    {
        Output_SetVoltage(Vout_3V3);
        chrg_feature.out.mV = (uint16_t)Analog_GetValues()->Vdd_mV;
        chrg_feature.out.buck = 1;
        chrg_feature.out.used = 1;
    }
}

/**
 * @brief Applies the charger feature report's parameters on the device.
 * @param charger: the input report
 */
static void Charger_SetChargerReport(Charger_FtChargerType *charger)
{
    /* charge current change */
    if (charger->mA > 500)
    {
        Charger_SetCurrent(Ichg_800mA);
        chrg_feature.charger.mA = 800;
    }
    else if (charger->mA > 100)
    {
        Charger_SetCurrent(Ichg_500mA);
        chrg_feature.charger.mA = 500;
    }
    else if (charger->mA > 0)
    {
        Charger_SetCurrent(Ichg_100mA);
        chrg_feature.charger.mA = 100;
    }
    else
    {
        Charger_SetCurrent(Ichg_0mA);
        chrg_feature.charger.mA = 0;
    }
}

/**
 * @brief Applies the battery feature report's parameters on the device.
 * @param battery: the input report
 */
static void Charger_SetBatteryReport(Charger_FtBatteryType *battery)
{
    /* Copy input data */
    chrg_feature.battery = *battery;

}

/**
 * @brief Sets the device configuration according to the received feature report.
 * @param data: element 0 is the ID, the rest is the data to set
 * @param length: 1 + length of the report
 */
static void Charger_SetReport(uint8_t * data, uint16_t length)
{
    /* First data element is the report ID */
    switch (data[0])
    {
        case 1: /* Nothing to set for USB power source */
            break;

        case 2:
            Charger_SetOutReport((Charger_FtOutType*)&data[1]);
            break;

        case 3:
            Charger_SetChargerReport((Charger_FtChargerType*)&data[1]);
            break;

        case 4:
            Charger_SetBatteryReport((Charger_FtBatteryType*)&data[1]);
            break;

        default:
            break;
    }
}

/**
 * @brief Provides the input report data for transmission
 */
void Charger_Periodic(void)
{
    if (chrg_if->Base.Device->ConfigSelector != 0)
    {
        chrg_input.battery.mV = (uint16_t)Charger_GetVoltage_mV();
        chrg_input.battery.mA = (uint16_t)Charger_GetCurrent_mA();

        /* If charging enabled, but no current */
        if ((chrg_feature.charger.mA > 0) && (chrg_input.battery.mA == 0))
        {
            /* Disconnected charger voltage = 4.1V */
            if ((chrg_input.battery.mV > 4080) && (chrg_input.battery.mV > 4120))
            {
                chrg_input.battery.present = 0;
                chrg_input.battery.charged = 0;
                chrg_input.battery.discharged = 0;
                chrg_input.battery.overheat = 0;
                chrg_input.battery.remcap = 0;
            }
            else /* Battery is connected, but charging is complete */
            {
                chrg_input.battery.present = 1;
                chrg_input.battery.charged = 1;
                chrg_input.battery.discharged = 0;
                chrg_input.battery.overheat = 0;
            }
        }
        else
        {
            chrg_input.battery.present = 1;
            chrg_input.battery.charged = 0;

            /* reduce charge current when overheated */
            if ((Analog_GetValues()->temp_C > 50) &&
                (chrg_feature.charger.mA >= 500) &&
                (chrg_input.battery.mA > 300))
            {
                Charger_FtChargerType reduced;
                reduced.mA = 100;
                Charger_SetChargerReport(&reduced);

                chrg_input.battery.discharged = 0;
                chrg_input.battery.overheat = 1;
            }
            else if (chrg_input.battery.mV < LiDischarge_mV)
            {
                chrg_input.battery.discharged = 1;
                chrg_input.battery.overheat = 0;
            }
            else
            {
                chrg_input.battery.discharged = 0;
                chrg_input.battery.overheat = 0;
            }
        }

        /* convert Vbat to remaining capacity */
        if (chrg_input.battery.present != 0)
        {
            /* TODO Different characteristics apply for a charged battery
             * than a discharged */
            {
                /* Simple linear approximation for now */
                int remcap = (int)chrg_feature.battery.capacity *
                ((int)chrg_input.battery.mV - (int)LiDischarge_mV) /
                ((int)LiCharged_mV - (int)LiDischarge_mV);

                chrg_input.battery.remcap = (uint16_t)remcap;
            }
        }

        vout_input.output.mA = Analog_GetValues()->Iout_mA;

        /* Send report through IN pipe */
        if ((inputsel++ & 1) != 0)
        {
            USBD_HID_ReportIn(chrg_if,
                        (uint8_t*)&chrg_input, sizeof(chrg_input));
        }
        else
        {
            USBD_HID_ReportIn(chrg_if,
                        (uint8_t*)&vout_input, sizeof(vout_input));
        }
    }
}

/** @brief Charger HID Application */
const USBD_HID_AppType chrgApp =
{
    .Name       = "Battery Charging Supervisor",
    .Init       = Charger_SetConfig,
    .Deinit     = Charger_ClearConfig,
    .SetReport  = Charger_SetReport,
    .GetReport  = Charger_GetReport,
    .Report     = {
            .Desc = ChargerReport,
            .Length = sizeof(ChargerReport),
            .IDs = 4,
    },
};

/** @brief Charger HID Interface (and reference) */
USBD_HID_IfHandleType hchrg_if = {
    .App = &chrgApp,
    .Base.AltCount = 1,
    .Config.InEp.Size       = sizeof(chrg_input),
    .Config.InEp.Interval   = 10,
}, *const chrg_if = &hchrg_if;
