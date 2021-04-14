#ifndef __INSTALL_DM_FONT_H__
#define __INSTALL_DM_FONT_H__

#include  <Library/UefiLib.h>
#include  <Library/HiiLib.h>
#include  <Library/BaseMemoryLib.h>
#include  <Library/MemoryAllocationLib.h>

#define WIDE_FONT_WIDTH       16
#define WIDE_FONT_HEIGHT      19
#define WIDE_FONT_OFFSET_X    0
#define WIDE_FONT_OFFSET_Y    0
#define WIDE_FONT_ADVANCE_X   17
#define NARROW_FONT_WIDTH     8
#define NARROW_FONT_HEIGHT    19
#define NARROW_FONT_OFFSET_X  0
#define NARROW_FONT_OFFSET_Y  0
#define NARROW_FONT_ADVANCE_X 8

extern UINT32            gSimpleFontBytes;
extern UINT32            mNarrowFontSize;
extern EFI_NARROW_GLYPH  gUsStdNarrowGlyphData[];
extern EFI_WIDE_GLYPH    gSimpleFontWideGlyphData[];

EFI_STATUS
InstallDMFontPkg (
  VOID
  );

EFI_STATUS
UnLoadFont (
  VOID
  );

#endif
