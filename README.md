# DebugDongle

This repository contains the embedded firmware of the DebugDongle,
a USB power supply and serial port for prototype debugging. 

## Features
- Bidirectional UART connection is relayed to the USB serial port.
- Power is supplied to an external board:
Either the USB voltage or the output of the 3.3V step-down converter.
The selection can be made by an onboard switch or by the USB HID interface.
The voltage level is indicated by a dedicated LED.
- The onboard Li-ion battery charger IC can charge a connected battery.
The charging is managed by the software and can be supervised by the USB HID interface.
Indicator LEDs give visual feedback on the USB power's presence and the ongoing charging.
- An independent USB HID sensor interface provides core voltage and temperature,
and ambient illuminance measurements.
- The bootloader's DFU interface is mapped on the USB device. The DFU updater client
can use the same interface in application mode to send the device to update mode.

## Target hardware
The firmware is written for an [STM32F042F6](http://www.st.com/en/microcontrollers/stm32f042f6.html) 
device (TSSOP-20 package, 32kB flash, 6kB SRAM, 48MHz clock).
By changing the BSP layer it is possible to port the application to any STM32 device
which is supported by *STM32_XPD*.

## External dependencies

This firmware relies on the following external sources:
1. The **CMSIS** and **STM32F0_XPD** libraries are imported from [STM32_XPD](https://github.com/IntergatedCircuits/STM32_XPD)
2. The USB device library (including CDC, HID and DFU classes) are imported from [USBDevice](https://github.com/IntergatedCircuits/USBDevice)
3. The [DFU bootloader](https://github.com/IntergatedCircuits/DfuBootloader)  is 
used with the following parameters:
`FLASH_APP_ADDRESS=0x08002000, FLASH_APP_SIZE=24568, BSP=STM32F0x2, FLASH_BYTE_PROGRAM_TIME_us=30, FLASH_TOTAL_ERASE_TIME_ms=480, USBD_VID=0xFFFF, USBD_PID=0xFFFF, VDD_VALUE_mV=3300, core=cm0`

Built with GCC ARM tools.
