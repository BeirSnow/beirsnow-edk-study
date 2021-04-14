#ifndef __DRAW_CIRCLE_H__
#define __DRAW_CIRCLE_H__

#include "DrawBasic.h"

EFI_STATUS
DrawCircle (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  UINTN                           CenterX,
  UINTN                           CenterY,
  UINTN                           Radius,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Color
  );

#endif
