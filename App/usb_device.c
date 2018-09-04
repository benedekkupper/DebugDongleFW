/**
  ******************************************************************************
  * @file    usb_device.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   DebugDongle USB Device definition and configuration
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
#include <xpd_pwr.h>
#include <usbd.h>

#include <vcp_if.h>
#include <chrg_if.h>
#include <sens_if.h>

#include <usbd_dfu.h>
/* DFU interface is initialized by bootloader */
USBD_DFU_IfHandleType __attribute__((section (".dfuSharedSection"))) hdfu_if;
USBD_DFU_IfHandleType *const dfu_if = &hdfu_if;


/** @brief USB device configuration */
const USBD_DescriptionType hdev_cfg = {
    .Vendor = {
        .Name           = "IntergatedCircuits",
        .ID             = USBD_VID,
    },
    .Product = {
        .Name           = "DebugDongle",
        .ID             = USBD_PID,
        .Version.bcd    = 0x0100 | HW_REV,
    },
#if (USBD_SERIAL_BCD_SIZE > 0)
    .SerialNumber       = (USBD_SerialNumberType*)DEVICE_ID_REG,
#endif
    .Config = {
        .Name           = "DebugDongle",
        .MaxCurrent_mA  = 500,
        .RemoteWakeup   = 0,
        .SelfPowered    = 0,
    },
}, *const dev_cfg = &hdev_cfg;

/** @brief USB device handle */
USBD_HandleType hUsbDevice, *const UsbDevice = &hUsbDevice;

/**
 * @brief Disables output paths and enters low power mode.
 * @param devHandle
 */
static void usbSuspendCallback(void * devHandle)
{
    Charger_Suspend();
}

/**
 * @brief Restores run mode and enables output paths.
 * @param devHandle
 */
static void usbResumeCallback(void * devHandle)
{
    Charger_Resume();
}

/**
 * @brief This function handles the setup of the USB device:
 *         - Assigns endpoints to USB interfaces
 *         - Mounts the interfaces on the device
 *         - Sets up the USB device
 *         - Determines the USB port type
 *         - Establishes logical connection with the host
 */
void UsbDevice_Init(void)
{
    USB_ChargerType usbPort;

    /* Initialize the device */
    USBD_Init(UsbDevice, dev_cfg);

    /* Set the available current limit based on the USB connection type */
    usbPort = USB_eChargerDetect(UsbDevice);
    Charger_SetType(usbPort);

    switch (usbPort)
    {
        /* No host is present, don't setup the interfaces */
        case USB_BCD_DEDICATED_CHARGING_PORT:
        case USB_BCD_PS2_PROPRIETARY_PORT:
            break;

        default:
        {
            /* All fields of Config have to be properly set up */
            vcp_if->Config.InEpNum  = 0x81;
            vcp_if->Config.OutEpNum = 0x01;
            vcp_if->Config.NotEpNum = 0x8F;

            chrg_if->Config.InEp.Num = 0x82;

            sens_if->Config.InEp.Num = 0x83;

            USBD_DFU_AppInit(dfu_if, 250); /* Detach can be carried out within 250 ms */

            /* Mount the interfaces to the device */
            USBD_DFU_MountInterface(dfu_if, UsbDevice);
            USBD_CDC_MountInterface(vcp_if, UsbDevice);
            USBD_HID_MountInterface(chrg_if, UsbDevice);
            USBD_HID_MountInterface(sens_if, UsbDevice);

            UsbDevice->Callbacks.Suspend = usbSuspendCallback;
            UsbDevice->Callbacks.Resume = usbResumeCallback;
            /* After charger detection the device connection can be made */
            USBD_Connect(UsbDevice);
            break;
        }
    }
}

/**
 * @brief Shuts down the USB peripheral.
 */
void UsbDevice_Deinit(void)
{
    USBD_Deinit(UsbDevice);
}
