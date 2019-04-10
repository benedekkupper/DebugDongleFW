/**
  ******************************************************************************
  * @file    vcp_if.h
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-03-10
  * @brief   USB Virtual COM Port interface header
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
#ifndef __VCP_IF_H
#define __VCP_IF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <usbd_cdc.h>
#include <xpd_usart.h>

#define VCP_OUT_DATA_SIZE   128
#define VCP_IN_DATA_SIZE    128

typedef enum
{
    VCP_BUFFER_EMPTY = 0,
    VCP_BUFFER_FULL,
    VCP_BUFFER_RECEIVING,
    VCP_BUFFER_TRANSMITTING
}VCP_BufferStatusType;

typedef struct {
    USBD_CDC_IfHandleType CdcIf;
    USART_HandleType Uart;
    uint8_t OutData[2][VCP_OUT_DATA_SIZE / 2];
    VCP_BufferStatusType OutStatus[2];
    uint16_t OutLength;
    uint8_t InData[VCP_IN_DATA_SIZE];
    uint16_t Index;
}VCP_HandleType;

extern const USBD_CDC_AppType vcpApp;

void VCP_Periodic(VCP_HandleType *vcp);

#ifdef __cplusplus
}
#endif

#endif /* __VCP_IF_H */
