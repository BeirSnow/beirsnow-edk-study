/** @file

This is a device manager driver might export data to the HII protocol to be
later utilized by the Setup Protocol.

Copyright (c) 2018 Loongson Technology Corporation Limited (www.loongson.cn).
All intellectual property rights(Copyright, Patent and Trademark) reserved.

Copyright (c) 2007 - 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  NVDataStruc.h

Abstract:

  NVData structure used by the sample driver
**/

#ifndef _NVDATASTRUC_H_
#define _NVDATASTRUC_H_

#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/HiiFormMapMethodGuid.h>
#include <Guid/DriverSampleHii.h>
#include <Guid/ZeroGuid.h>

#define CONFIGURATION_VARSTORE_ID          0x1234
#define KEY_VALUE_TRIGGER_FORM_OPEN_ACTION 0x1117
#define KEY_VALUE_UPDATE_FW              0x110D
#define LABEL_FORM_MAIN_START              0xfffc
#define LABEL_FORM_MAIN_END                0xfffd

#define UPDATE_FW_FORMSET_GUID {\
  0xa6f25c17, 0x0a47, 0x53ef, { 0xbe, 0xe6, 0xc8, 0x17, 0xfc, 0xf1, 0x95, 0x36 } \
}

typedef struct {
  UINT64        Reserved;
} UPDATE_FIRMWARE_CONFIGURATION;

#endif
