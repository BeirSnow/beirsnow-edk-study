#include "KeyEvent.h"

/*Locate SimpleTextInputEx protocol*/
EFI_STATUS
LocateSimpleTextInputEx(
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  **SimpleTextInputEx
)
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        *Handles;
  UINTN                             HandleCount;
  UINTN                             HandleIndex;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->HandleProtocol(
                    Handles[HandleIndex],
                    &gEfiSimpleTextInputExProtocolGuid,
                    (VOID **)SimpleTextInputEx
                    );
    if (EFI_ERROR(Status)) {
      continue;
    } else {
      return EFI_SUCCESS;
    }
  }

  return Status;
}

/*Get key*/
EFI_STATUS
GetKeyEx(
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInputEx,
  UINT16                             *ScanCode,
  UINT16                             *UniChar,
  UINT32                             *ShiftState,
  EFI_KEY_TOGGLE_STATE               *ToggleState
)
{
  EFI_STATUS      Status;
  EFI_KEY_DATA    KeyData;
  UINTN           Index;

  gBS->WaitForEvent(1, &(SimpleTextInputEx->WaitForKeyEx), &Index);

  Status = SimpleTextInputEx->ReadKeyStrokeEx(SimpleTextInputEx, &KeyData);

  if (!EFI_ERROR(Status)) {
    *ScanCode = KeyData.Key.ScanCode;
    *UniChar = KeyData.Key.UnicodeChar;
    *ShiftState = KeyData.KeyState.KeyShiftState;
    *ToggleState = KeyData.KeyState.KeyToggleState;
    return EFI_SUCCESS;
  }

  return Status;
}
