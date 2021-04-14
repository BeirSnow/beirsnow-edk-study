#ifndef __KEY_EVENT_H__
#define __KEY_EVENT_H__

#include  <Library/UefiBootServicesTableLib.h>

/*Locate SimpleTextInputEx protocol*/
EFI_STATUS
LocateSimpleTextInputEx (
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  **SimpleTextInputEx
);

/*Get key*/
EFI_STATUS
GetKeyEx (
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInputEx,
  UINT16                             *ScanCode,
  UINT16                             *UniChar,
  UINT32                             *ShiftState,
  EFI_KEY_TOGGLE_STATE               *ToggleState
);

#endif
