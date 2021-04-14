#ifndef __POP_WINDOW_H__
#define __POP_WINDOW_H__

#include "Button.h"
#include "../Font/DisplayFont.h"
#include "../DrawBasic/DrawBasic.h"
#include "../EventSupport/KeyEvent.h"

typedef struct {
  UINTN StartX;
  UINTN StartY;
  UINTN Width;
  UINTN Height;
  UINTN FrameWidth;
} POP_WINDOW_AREA;

typedef enum {
  POP_RECT_BLOCK_NORMAL
} POP_WINDOW_STYLE;

typedef struct {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FrameColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FilledColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FontColor;
} POP_WINDOW_COLOR;

typedef EFI_STATUS(* POP_WINDOW_CALLBACK)(VOID);

typedef struct {
  CHAR16 *NoteStr;
  CHAR16 *AcceptStr;
  CHAR16 *CancelStr;
} POP_WINDOW_STR;

typedef struct {
  POP_WINDOW_STR      PopWindowStr;
  POP_WINDOW_AREA     Area;
  POP_WINDOW_STYLE    Style;
  POP_WINDOW_COLOR    Color;
} POP_WINDOW_INFO;

EFI_STATUS
PopWindowConstruct (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  IN  POP_WINDOW_INFO          *PopWindow,
  OUT BOOLEAN                  *SlctFlag
  );

#endif
