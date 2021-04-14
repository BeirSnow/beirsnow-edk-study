#include  "UI_Engine.h"
#include  "Font/InstallDMFont.h"

EFI_STATUS
GetFrameInfo (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput,
  OUT EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE  *Mode
  )
{
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (Mode, GraphicsOutput->Mode, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE));

  return EFI_SUCCESS;
}

EFI_STATUS
PutPixel (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  UINTN                           X,
  UINTN                           Y,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Color
  )
{
  EFI_STATUS Status;
  //
  // Check params
  //
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (X >= GraphicsOutput->Mode->Info->HorizontalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  if (Y >= GraphicsOutput->Mode->Info->VerticalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             Color,
                             EfiBltVideoFill,
                             0,0,
                             X,Y,
                             1,1,
                             0
                             );
  return Status;
}

/**
  Fill screen with black
**/
EFI_STATUS
ClearScreen (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput
  )
{
  EFI_STATUS Status;

  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Black
  //
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL BltColor = {0, 0, 0, 0};

  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             &BltColor,
                             EfiBltVideoFill,
                             0, 0,
                             0, 0,
                             GraphicsOutput->Mode->Info->HorizontalResolution,
                             GraphicsOutput->Mode->Info->VerticalResolution,
                             0
                             );
  return Status;
}
