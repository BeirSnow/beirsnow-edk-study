#ifndef __DRAW_RECT_H__
#define __DRAW_RECT_H__

#include "DrawBasic.h"
#include "DrawLine.h"

typedef struct {
  POINT_T Start;
  UINTN   Width;
  UINTN   Height;
  UINTN   FrameWidth;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FrameColor;
} RECT_INFO;

typedef enum {
  NORMAL_RECT_BLK,
  VER_GRADIENT,
  VER_REVERSE_GRADIENT,
  HOR_GRADIENT,
  HOR_REVERSE_GRADIENT
} RECT_BLOCK_STYLE;

typedef struct {
  POINT_T           Start;
  UINTN             Width;
  UINTN             Height;
  UINTN             FrameWidth;
  RECT_BLOCK_STYLE  Style;
  UINT32            Gradient;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FrameColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FilledColor;
} RECT_BLOCK_INFO;

/**
  Draw a rect
**/
EFI_STATUS
DrawRect (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN RECT_INFO                     *RectInfo
  );

/**
  Draw a rect block
**/
EFI_STATUS
DrawRectBlock (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN RECT_BLOCK_INFO               *RectBlkInfo
  );

#endif
