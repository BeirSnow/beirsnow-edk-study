/** @file
 *
This is a device manager driver might export data to the HII protocol to be
later utilized by the Setup Protocol.

Copyright (c) 2018 Loongson Technology Corporation Limited (www.loongson.cn).
All intellectual property rights(Copyright, Patent and Trademark) reserved.

Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "UpdateFw.h"
#include <Password.h>
#include <Library/FileExplorerLib.h>
#include <Library/FileHandleLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/LsServiceDxe.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DisplayProtocol.h>
#include <Guid/SetupPassword.h>

CHAR16                                VariableName[] = L"UpdateFw";
EFI_HANDLE                            DriverHandle;
DEVICE_MANAGER_LIST_PRIVATE_DATA      *mPrivateData = NULL;
EDKII_FORM_DISPLAY_ENGINE_PROTOCOL    *mFormDisplay;
UINTN                                 mCount = 0;

HII_VENDOR_DEVICE_PATH  mHiiVendorDevicePath0 = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    UPDATE_FW_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.

  @param  This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Request                A null-terminated Unicode string in
                                 <ConfigRequest> format.
  @param  Progress               On return, points to a character in the Request
                                 string. Points to the string's null terminator if
                                 request was successful. Points to the most recent
                                 '&' before the first failing name/value pair (or
                                 the beginning of the string if the failure is in
                                 the first name/value pair) if the request was not
                                 successful.
  @param  Results                A null-terminated Unicode string in
                                 <ConfigAltResp> format which has all values filled
                                 in for the names in the Request string. String to
                                 be allocated by the called function.

  @retval EFI_SUCCESS            The Results is filled with the requested values.
  @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.

**/
EFI_STATUS
EFIAPI
ExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                              Status;
  UINTN                                   BufferSize;
  DEVICE_MANAGER_LIST_PRIVATE_DATA        *PrivateData;
  EFI_HII_CONFIG_ROUTING_PROTOCOL         *HiiConfigRouting;
  EFI_STRING                              ConfigRequest;
  EFI_STRING                              ConfigRequestHdr;
  UINTN                                   Size;
  CHAR16                                  *StrPointer;
  BOOLEAN                                 AllocatedRequest;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Initialize the local variables.
  //
  ConfigRequestHdr  = NULL;
  ConfigRequest     = NULL;
  Size              = 0;
  *Progress         = Request;
  AllocatedRequest  = FALSE;

  PrivateData = DEVICE_MANAGER_PRIVATE_FROM_THIS (This);
  HiiConfigRouting = PrivateData->HiiConfigRouting;
  //
  // Get Buffer Storage data from EFI variable.
  // Try to get the current setting from variable.
  //
  BufferSize = sizeof (UPDATE_FIRMWARE_CONFIGURATION);

  Status = gRT->GetVariable (
                  VariableName,
                  &gDeviceListFormSetGuid,
                  NULL,
                  &BufferSize,
                  &PrivateData->Configuration
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  if (Request == NULL) {
    //
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&gDeviceListFormSetGuid, VariableName, PrivateData->DriverHandle);
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
    ConfigRequestHdr = NULL;
  } else {
    //
    // Set Request to the unified request string.
    //
    ConfigRequest = Request;
    //
    // Check whether Request includes Request Element.
    //
    if (StrStr (Request, L"OFFSET") == NULL) {
      //
      // Check Request Element does exist in Reques String
      //
      StrPointer = StrStr (Request, L"PATH");
      if (StrPointer == NULL) {
        return EFI_INVALID_PARAMETER;
      }
      if (StrStr (StrPointer, L"&") == NULL) {
        Size = (StrLen (Request) + 32 + 1) * sizeof (CHAR16);
        ConfigRequest    = AllocateZeroPool (Size);
        ASSERT (ConfigRequest != NULL);
        AllocatedRequest = TRUE;
        UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", Request, (UINT64)BufferSize);
      }
    }
  }
  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  Status = HiiConfigRouting->BlockToConfig (
                                HiiConfigRouting,
                                ConfigRequest,
                                (UINT8 *) &PrivateData->Configuration,
                                BufferSize,
                                Results,
                                Progress
                                );
  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
  }

  if (ConfigRequestHdr != NULL) {
    FreePool (ConfigRequestHdr);
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
}


/**
  This function processes the results of changes in configuration.

  @param  This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Configuration          A null-terminated Unicode string in <ConfigResp>
                                 format.
  @param  Progress               A pointer to a string filled in with the offset of
                                 the most recent '&' before the first failing
                                 name/value pair (or the beginning of the string if
                                 the failure is in the first name/value pair) or
                                 the terminating NULL if all was successful.

  @retval EFI_SUCCESS            The Results is processed successfully.
  @retval EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.

**/
EFI_STATUS
EFIAPI
RouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  DEVICE_MANAGER_LIST_PRIVATE_DATA       *PrivateData;
  EFI_HII_CONFIG_ROUTING_PROTOCOL  *HiiConfigRouting;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PrivateData = DEVICE_MANAGER_PRIVATE_FROM_THIS (This);
  HiiConfigRouting = PrivateData->HiiConfigRouting;
  *Progress = Configuration;
  //
  // Get Buffer Storage data from EFI variable
  //
  BufferSize = sizeof (UPDATE_FIRMWARE_CONFIGURATION);
  Status = gRT->GetVariable (
            VariableName,
            &gDeviceListFormSetGuid,
            NULL,
            &BufferSize,
            &PrivateData->Configuration
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
  //
  BufferSize = sizeof (UPDATE_FIRMWARE_CONFIGURATION);
  Status = HiiConfigRouting->ConfigToBlock (
                               HiiConfigRouting,
                               Configuration,
                               (UINT8 *) &PrivateData->Configuration,
                               &BufferSize,
                               Progress
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Store Buffer Storage back to EFI variable
  //
  Status = gRT->SetVariable(
                  VariableName,
                  &gDeviceListFormSetGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof (UPDATE_FIRMWARE_CONFIGURATION),
                  &PrivateData->Configuration
                  );

  return Status;
}

/**
  This function converts an input device structure to a Unicode string.

  @param DevPath      A pointer to the device path structure.

  @return             A new allocated Unicode string that represents the device path.

**/
CHAR16 *
UiDevicePathToStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  )
{
  EFI_STATUS                       Status;
  CHAR16                           *ToText;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;

  if (DevPath == NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  (VOID **) &DevPathToText
                  );
  ASSERT_EFI_ERROR (Status);
  ToText = DevPathToText->ConvertDevicePathToText (
                            DevPath,
                            FALSE,
                            TRUE
                            );
  ASSERT (ToText != NULL);
  return ToText;
}

/**
  Extract filename from device path. The returned buffer is allocated using AllocateCopyPool.
  The caller is responsible for freeing the allocated buffer using FreePool().

  @param DevicePath       Device path.

  @return                 A new allocated string that represents the file name.

**/
CHAR16 *
ExtractFileNameFromDevicePath (
  IN   EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  CHAR16          *String;
  CHAR16          *MatchString;
  CHAR16          *LastMatch;
  CHAR16          *FileName;
  UINTN           Length;

  ASSERT(DevicePath != NULL);

  String = UiDevicePathToStr(DevicePath);
  MatchString = String;
  LastMatch   = String;
  FileName    = NULL;

  while(MatchString != NULL){
    LastMatch   = MatchString + 1;
    MatchString = StrStr(LastMatch,L"\\");
  }

  Length = StrLen(LastMatch);
  FileName = AllocateCopyPool ((Length + 1) * sizeof(CHAR16), LastMatch);
  if (FileName != NULL) {
    *(FileName + Length) = 0;
  }

  FreePool(String);

  return FileName;
}

/**
  Update the firmware

  @param FilePath    Point to the file path.

  @retval TRUE   Exit caller function.
  @retval FALSE  Not exit caller function.
**/
BOOLEAN
EFIAPI
UpdateFw (
  IN EFI_DEVICE_PATH_PROTOCOL    *FilePath
  )
{
  EFI_STATUS                         Status;
  CHAR16                             *FileName;
  EFI_INPUT_KEY                      Key;
  EFI_FILE_PROTOCOL                  *FileRoot;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL    *SimpleFileSystem;
  EFI_FILE_PROTOCOL                  *FileHandle;
  UINTN                              FileSize = 0;
  VOID                               *FileBuffer = NULL;
  EFI_LS_SERVICE_PROTOCOL            *LsService = NULL;

  //
  // Locate SimpleFileSystemProtocol
  //
  Status = gBS->LocateProtocol(
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  (VOID**)&SimpleFileSystem
                  );
  ASSERT_EFI_ERROR(Status);
  //
  // Locate LsServiceProtocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiLsServiceProtocolGuid,
                  NULL,
                  (void **)&LsService
                  );
  ASSERT_EFI_ERROR(Status);
  //
  // Get firmware file name
  //
  FileName = NULL;
  FileName = ExtractFileNameFromDevicePath(FilePath);

  if (FileName != NULL) {
    do {
      CreatePopUp (
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
        &Key,
        L"",
        L"Update the firmware?",
        L"Press 'Y' to confirm 'N'/'ESC' to ignore",
        L"",
        NULL
        );
    } while (((Key.UnicodeChar | 0x20) != (L'Y' | 0x20)) &&
             ((Key.UnicodeChar | 0x20) != (L'N' | 0x20)) &&
             (Key.ScanCode != SCAN_ESC));

    if ((Key.UnicodeChar | 0x20) == (L'Y' | 0x20)) {
      //
      // Read firmware to buffer
      //
      Status = SimpleFileSystem->OpenVolume (SimpleFileSystem, &FileRoot);
      ASSERT_EFI_ERROR(Status);

      Status = FileRoot->Open (
                           FileRoot,
                           &FileHandle,
                           (CHAR16*)FileName,
                           EFI_FILE_MODE_READ,
                           0
                           );
      ASSERT_EFI_ERROR(Status);

      Status = FileHandleGetSize (FileHandle, &FileSize);
      ASSERT_EFI_ERROR(Status);

      FileBuffer = AllocatePool (FileSize);

      Status = FileHandle->Read (
                             FileHandle,
                             &FileSize,
                             FileBuffer
                             );
      //
      // Clear screen
      //
      mFormDisplay->ExitDisplay();
      //
      // Update firmware
      //
      LsService->CpuSpi.UpDate(&LsService->CpuSpi, EfiDataWidthUint8, FileSize, 0, FileBuffer);
      //
      // Reset system
      //
      gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
    }
  }

  return FALSE;
}

/**
  This function processes the results of changes in configuration.

  @param  This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Action                 Specifies the type of action taken by the browser.
  @param  QuestionId             A unique value which is sent to the original
                                 exporting driver so that it can identify the type
                                 of data to expect.
  @param  Type                   The type of value for the question.
  @param  Value                  A pointer to the data being sent to the original
                                 exporting driver.
  @param  ActionRequest          On return, points to the action requested by the
                                 callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the
                                 variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the
                                 callback.

**/
EFI_STATUS
EFIAPI
DriverCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *File;
  UINTN                     Size;
  PASSWORDSETUP             mPassword;
  CHAR16                    Password[PASSWORD_LENGTH];
  CHAR16                    PasswdVarName[] = L"PasswdData";
  EFI_PASSWORD_PROTOCOL     *PasswordInterfanceSetup = NULL;
  CHAR16                    *String;

  if (((Value == NULL) && (Action != EFI_BROWSER_ACTION_FORM_OPEN) && (Action != EFI_BROWSER_ACTION_FORM_CLOSE))||
      (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Action) {
    case EFI_BROWSER_ACTION_CHANGING:
      if (QuestionId == KEY_VALUE_UPDATE_FW) {
        //
        // Setup Password check
        //
        Size = sizeof (PASSWORDSETUP);
        Status = gRT->GetVariable (
                        PasswdVarName,
                        &gPasswordSetupFormSetGuid,
                        NULL,
                        &Size,
                        &mPassword
                        );
        if (Status == EFI_NOT_FOUND) {
          DEBUG((EFI_D_ERROR, "PlatformBdsEnterFrontPage, no password.\n"));
        } else {
          Status = gBS->LocateProtocol (
                          &gEfiPasswordProtocolGuid,
                          NULL,
                          (VOID **)&PasswordInterfanceSetup
                          );
          ASSERT_EFI_ERROR (Status);

          if ((PasswordInterfanceSetup != NULL)
              && (mPassword.Password[0] != CHAR_NULL)
              && (PasswordInterfanceSetup->CheckPassword != NULL)) {
            ZeroMem (&Password[0], PASSWORD_LENGTH * sizeof (CHAR16));
            if (mCount == 0) {
              String = HiiGetString (mPrivateData->HiiHandle, STRING_TOKEN (STR_CHECK_PASSWORD_INPUT_1), NULL);
              PasswordInterfanceSetup->CheckPassword (
                                         String,
                                         &Password[0]
                                         );
            } else if (mCount == 1) {
              String = HiiGetString (mPrivateData->HiiHandle, STRING_TOKEN (STR_CHECK_PASSWORD_INPUT_2), NULL);
              PasswordInterfanceSetup->CheckPassword (
                                         String,
                                         &Password[0]
                                         );
            } else if (mCount == 2) {
              String = HiiGetString (mPrivateData->HiiHandle, STRING_TOKEN (STR_CHECK_PASSWORD_INPUT_3), NULL);
              PasswordInterfanceSetup->CheckPassword (
                                         String,
                                         &Password[0]
                                         );
            }
            if (StrCmp (&Password[0], &mPassword.Password[0]) == 0) {
              mCount = 0;
              ChooseFile (NULL, NULL, UpdateFw, &File);
            } else {
              if (mCount >= 2) {
                //
                // Clear screen
                //
                mFormDisplay->ExitDisplay();
                String = HiiGetString (mPrivateData->HiiHandle, STRING_TOKEN (STR_CHECK_PASSWORD_SHUTDOWN), NULL);
                PasswordInterfanceSetup->CheckPassword (
                                           String,
                                           NULL
                                           );
                gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);
              }
              mCount++;
            }
            break;
          }
        }
        ChooseFile (NULL, NULL, UpdateFw, &File);
      }
      break;
    default:
      break;
  }

  return EFI_SUCCESS;
}

/**
  Main entry for this driver.

  @param ImageHandle     Image handle this driver.
  @param SystemTable     Pointer to SystemTable.

  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
UpdateFwInit (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_HII_HANDLE                  HiiHandle;
  EFI_HII_CONFIG_ROUTING_PROTOCOL *HiiConfigRouting;
  CHAR16                          *NewString;
  UINTN                           BufferSize;
  UPDATE_FIRMWARE_CONFIGURATION     *Configuration;
  BOOLEAN                         ActionFlag;
  EFI_STRING                      ConfigRequestHdr;

  //
  // Initialize the local variables.
  //
  ConfigRequestHdr = NULL;
  NewString        = NULL;
  //
  // Initialize driver private data
  //
  mPrivateData = AllocateZeroPool (sizeof (DEVICE_MANAGER_LIST_PRIVATE_DATA));
  if (mPrivateData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mPrivateData->Signature = DEVICE_MANAGER_PRIVATE_SIGNATURE;

  mPrivateData->ConfigAccess.ExtractConfig = ExtractConfig;
  mPrivateData->ConfigAccess.RouteConfig = RouteConfig;
  mPrivateData->ConfigAccess.Callback = DriverCallback;
  //
  // Locate ConfigRouting protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiConfigRoutingProtocolGuid,
                  NULL,
                  (VOID **) &HiiConfigRouting
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Locate FormDisplayEngineProtocol
  //
  Status = gBS->LocateProtocol (
                  &gEdkiiFormDisplayEngineProtocolGuid,
                  NULL,
                  (VOID **) &mFormDisplay
                  );
  ASSERT_EFI_ERROR(Status);

  mPrivateData->HiiConfigRouting = HiiConfigRouting;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mHiiVendorDevicePath0,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mPrivateData->ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  mPrivateData->DriverHandle = DriverHandle;
  //
  // Publish our HII data
  //
  HiiHandle = HiiAddPackages (
                &gDeviceListFormSetGuid,
                DriverHandle,
                UpdateFwStrings,
                UpdateFwVfrBin,
                NULL
                );
  if (HiiHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mPrivateData->HiiHandle = HiiHandle;
  //
  // Update the device path string.
  //
  NewString = ConvertDevicePathToText((EFI_DEVICE_PATH_PROTOCOL*)&mHiiVendorDevicePath0, FALSE, FALSE);
  if (HiiSetString (HiiHandle, STRING_TOKEN (STR_DEVICE_PATH), NewString, NULL) == 0) {
    UpdateFwUnload (ImageHandle);
    return EFI_OUT_OF_RESOURCES;
  }
  if (NewString != NULL) {
    FreePool (NewString);
  }
  //
  // Initialize configuration data
  //
  Configuration = &mPrivateData->Configuration;
  ZeroMem (Configuration, sizeof (UPDATE_FIRMWARE_CONFIGURATION));
  //
  // Try to read the config EFI variable first
  //
  ConfigRequestHdr = HiiConstructConfigHdr (&gDeviceListFormSetGuid, VariableName, DriverHandle);
  ASSERT (ConfigRequestHdr != NULL);

  BufferSize = sizeof (UPDATE_FIRMWARE_CONFIGURATION);
  Status = gRT->GetVariable (VariableName, &gDeviceListFormSetGuid, NULL, &BufferSize, Configuration);
  if (EFI_ERROR (Status)) {
    //
    // Store zero data Buffer Storage to EFI variable
    //
    Status = gRT->SetVariable(
                    VariableName,
                    &gDeviceListFormSetGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (UPDATE_FIRMWARE_CONFIGURATION),
                    Configuration
                    );
    if (EFI_ERROR (Status)) {
      UpdateFwUnload (ImageHandle);
      return Status;
    }

    ActionFlag = HiiSetToDefaults (ConfigRequestHdr, EFI_HII_DEFAULT_CLASS_STANDARD);
    if (!ActionFlag) {
      UpdateFwUnload (ImageHandle);
      return EFI_INVALID_PARAMETER;
    }
  } else {
    ActionFlag = HiiValidateSettings (ConfigRequestHdr);
    if (!ActionFlag) {
      UpdateFwUnload (ImageHandle);
      return EFI_INVALID_PARAMETER;
    }
  }
  FreePool (ConfigRequestHdr);

  return EFI_SUCCESS;
}

/**
  Unloads the application and its installed protocol.

  @param[in]  ImageHandle       Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
**/
EFI_STATUS
EFIAPI
UpdateFwUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  ASSERT (mPrivateData != NULL);

  if (DriverHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           DriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mHiiVendorDevicePath0,
           &gEfiHiiConfigAccessProtocolGuid,
           &mPrivateData->ConfigAccess,
           NULL
           );
    DriverHandle = NULL;
  }

  if (mPrivateData->HiiHandle != NULL) {
    HiiRemovePackages (mPrivateData->HiiHandle);
  }

  FreePool (mPrivateData);
  mPrivateData = NULL;

  return EFI_SUCCESS;
}
