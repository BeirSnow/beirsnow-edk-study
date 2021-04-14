#ifndef __DRAW_LINE_H__
#define __DRAW_LINE_H__

#include "DrawBasic.h"

/**
  Draw a horizontal line
**/
EFI_STATUS
DrawLineHor (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  POINT_T                          StartPoint,
  UINTN                            Length,
  UINTN                            Width,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Color
  );

/**
  Draw a vertical line
**/
EFI_STATUS
DrawLineVer (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  POINT_T                          StartPoint,
  UINTN                            Length,
  UINTN                            Width,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Color
  );

/**
  Draw a skew line
**/
EFI_STATUS
DrawLineSkew (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  POINT_T                          StartPoint,
  POINT_T                          EndPoint,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Color
  );

#endif
