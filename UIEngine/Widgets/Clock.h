#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "../DrawBasic/DrawBasic.h"
#include "../Font/DisplayFont.h"
#include <Library/UefiRuntimeServicesTableLib.h>

#define CLOCK_WIDTH  GetStrWidth(L"00:00:00")
#define CLOCK_HEIGHT GetStrHeight(L"00:00:00")

typedef struct {
  UINTN StartX;
  UINTN StartY;
} CLOCK_AREA;

typedef struct {
  CLOCK_AREA                    Area;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FontColor;
} CLOCK_INFO;

EFI_STATUS
ClockConstruct (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  CLOCK_INFO                   *Clock,
  EFI_EVENT                    *TimeEvent
  );

EFI_STATUS
ClockDestruct (
  EFI_EVENT  *TimeEvent
  );

#endif
