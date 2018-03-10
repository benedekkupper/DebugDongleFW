/**
  ******************************************************************************
  * @file    usbd_config.h
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   DebugDongle USB Device library configuration
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
#ifndef __USBD_CONFIG_H_
#define __USBD_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup USBD
 * @{ */

/** @addtogroup USBD_Exported_Macros USBD Exported Macros
 * @{ */

/* 2 - VCP, 1 - Sensor HID, 1 - Power HID */
#define USBD_MAX_IF_COUNT           4

/* Large enough for complete configuration descriptor as well as strings */
#define USBD_EP0_BUFFER_SIZE        256

/* No HID OUT data, all use-cases are treated as Feature, through Ctrl EP */
#define USBD_HID_OUT_SUPPORT        0

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CONFIG_H_ */
