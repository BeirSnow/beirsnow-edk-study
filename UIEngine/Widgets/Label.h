#ifndef __LABEL_H__
#define __LABEL_H__

#include "../DrawBasic/DrawRect.h"
#include "../Font/DisplayFont.h"

typedef struct {
  UINTN StartX;
  UINTN StartY;
  UINTN Width;
  UINTN Height;
  UINTN FrameWidth;
} LABEL_AREA;

typedef enum {
  LABEL_RECT_BLOCK_NORMAL
} LABEL_STYLE;

typedef struct {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FrameColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FilledColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FontColor;
} LABEL_COLOR;

typedef EFI_STATUS(* LABEL_CALLBACK)(VOID);

typedef struct {
  CHAR16         *Str;
  LABEL_AREA     Area;
  LABEL_STYLE    Style;
  LABEL_COLOR    Color;
} LABEL_INFO;

EFI_STATUS
LabelConstruct (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  LABEL_INFO                   *Label
  );

#endif
