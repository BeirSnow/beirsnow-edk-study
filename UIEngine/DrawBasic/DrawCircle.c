#include "DrawCircle.h"

EFI_STATUS
DrawCircle (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  UINTN                           CenterX,
  UINTN                           CenterY,
  UINTN                           Radius,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Color
  )
{
  EFI_STATUS Status;
  INT32 X, Y, D;

  //
  // Check params
  //
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Radius == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (Radius > CenterX + 1 || Radius > CenterY + 1) {
    return EFI_INVALID_PARAMETER;
  }

  if (CenterX + Radius >= GraphicsOutput->Mode->Info->HorizontalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  if (CenterY + Radius >= GraphicsOutput->Mode->Info->VerticalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  Y = (INT32)(Radius);
  D = 3 - ((INT32)(Radius) << 1);

  Status =  PutPixel (GraphicsOutput, CenterX, CenterY + Y, Color);        /*0,-1*/
  Status += PutPixel (GraphicsOutput, CenterX, CenterY - Y, Color);        /*0,1*/
  Status += PutPixel (GraphicsOutput, CenterX + Y, CenterY, Color);
  Status += PutPixel (GraphicsOutput, CenterX - Y, CenterY, Color);

  for (X = 1; X < Y; X++) {
    Status += PutPixel (GraphicsOutput, CenterX + X, CenterY + Y, Color);
    Status += PutPixel (GraphicsOutput, CenterX + X, CenterY - Y, Color);
    Status += PutPixel (GraphicsOutput, CenterX - X, CenterY - Y, Color);
    Status += PutPixel (GraphicsOutput, CenterX - X, CenterY + Y, Color);
    Status += PutPixel (GraphicsOutput, CenterX + Y, CenterY + X, Color);
    Status += PutPixel (GraphicsOutput, CenterX + Y, CenterY - X, Color);
    Status += PutPixel (GraphicsOutput, CenterX - Y, CenterY - X, Color);
    Status += PutPixel (GraphicsOutput, CenterX - Y, CenterY + X, Color);
    if (D < 0) {
      D += ((X << 2) + 6);
    } else {
      D += (((X - (Y--)) << 2) + 10);
    }
  }
  Status += PutPixel (GraphicsOutput, CenterX + Y, CenterY + Y,Color);
  Status += PutPixel (GraphicsOutput, CenterX + Y, CenterY - Y,Color);
  Status += PutPixel (GraphicsOutput, CenterX - Y, CenterY - Y,Color);
  Status += PutPixel (GraphicsOutput, CenterX - Y, CenterY + Y,Color);

  return Status;
}
