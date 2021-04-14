#ifndef __DISPLAY_FONT_H__
#define __DISPLAY_FONT_H__

#include "../DrawBasic/DrawBasic.h"
#include "InstallDMFont.h"

UINTN
GetStrWidth (
  IN CHAR16 *Str
  );

UINTN
GetStrHeight (
  IN CHAR16 *Str
  );

/**
  Display font
**/
EFI_STATUS
DisplayFont (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  CHAR16                        *Str,
  IN  EFI_FONT_DISPLAY_INFO         *StrInfo,
  UINTN                             PosX,
  UINTN                             PosY
  );

/**
  Display font
**/
EFI_STATUS
DisplayFont2 (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  CHAR16                        *Str,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Color,
  UINTN                             PosX,
  UINTN                             PosY
  );

#endif
