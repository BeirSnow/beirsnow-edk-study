#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "../DrawBasic/DrawRect.h"
#include "../Font/DisplayFont.h"

typedef struct {
  UINTN StartX;
  UINTN StartY;
  UINTN Width;
  UINTN Height;
  UINTN FrameWidth;
} BUTTON_AREA;

typedef enum {
  BUTTON_RECT_BLOCK_NORMAL
} BUTTON_STYLE;

typedef struct {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FrameColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FilledColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FontColor;
} BUTTON_COLOR;

typedef EFI_STATUS(* BUTTON_CALLBACK)(VOID *);

typedef struct {
  BOOLEAN         Status; /*0:Not Selected 1:Selected*/
  CHAR16          *Str;
  BUTTON_AREA     Area;
  BUTTON_STYLE    Style;
  BUTTON_COLOR    NotSlctColor;
  BUTTON_COLOR    SlctColor;
  BUTTON_CALLBACK Callback;
} BUTTON_INFO;

EFI_STATUS
ButtonConstruct (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  BUTTON_INFO                  *Button
  );

#endif
