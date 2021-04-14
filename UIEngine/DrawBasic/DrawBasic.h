#ifndef __DRAW_BASIC_H__
#define __DRAW_BASIC_H__

#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/DebugLib.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Library/BaseMemoryLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/GraphicsOutput.h>
#include  <Protocol/HiiFont.h>

#define RETURN_ERROR_STATUS(Status)                \
  if (EFI_ERROR(Status)) {                         \
    return Status;                                 \
  }

#define PIXEL_COLOR_BLUE     {255, 0,   0,   0}
#define PIXEL_COLOR_GREEN    {0,   255, 0,   0}
#define PIXEL_COLOR_RED      {0,   0,   255, 0}
#define PIXEL_COLOR_YELLOW   {0,   255, 255, 0}
#define PIXEL_COLOR_WHITE    {255, 255, 255, 0}
#define PIXEL_COLOR_GRAY     {125, 125, 125, 0}
#define PIXEL_COLOR_CYAN     {210, 210, 50,  0}
#define PIXEL_COLOR_ORANGE   {40,  110, 240, 0}
#define PIXEL_COLOR_XXX      {164, 158,  52, 0}

typedef struct {
  INT32 X;
  INT32 Y;
}POINT_T;

EFI_STATUS
GetFrameInfo (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput,
  OUT EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE  *Mode
  );

EFI_STATUS
PutPixel (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  UINTN                           X,
  UINTN                           Y,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Color
  );

/**
  Fill screen with black
**/
EFI_STATUS
ClearScreen (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput
  );
#endif
