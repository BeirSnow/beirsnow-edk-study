#include  "Clock.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL  *gGraphicsOutput;
CLOCK_INFO                    *gClock;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL *gBackground;

VOID
Int64ToUnic (
  INT64      Num,
  OUT CHAR16 *Str,
  INT64      Radix
  )
{
  CHAR16 Index[] = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  UINT64 NumTemp;
  CHAR16 CharTemp;
  INT32  i = 0, j, k;

  if (Radix == 10 && Num < 0) {
    NumTemp = (UINT64)(-Num);
    Str[i++] = L'-';
  } else {
    NumTemp = (UINT64)Num;
  }

  do {
    Str[i++] = Index[NumTemp % (UINT64)Radix];
    NumTemp /= Radix;
  } while (NumTemp);

  Str[i] = L'\0';

  if (Str[0] == L'-') {
    k = 1;
  } else {
    k = 0;
  }
  for (j = k; j <= (i - 1) / 2; j++) {
    CharTemp = Str[j];
    Str[j] = Str[i - 1 + k - j];
    Str[i - 1 + k - j] = CharTemp;
  }
}

EFI_STATUS
FormatTime (
  IN EFI_TIME   *Time,
  OUT CHAR16    *TimeStr
  )
{
  CHAR16 *Hour   = AllocatePool (sizeof(CHAR16) * 3);
  if (Hour == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CHAR16 *Minute = AllocatePool (sizeof(CHAR16) * 3);
  if (Hour == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CHAR16 *Second = AllocatePool (sizeof(CHAR16) * 3);
  if (Hour == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Hour
  //
  Int64ToUnic (Time->Hour, Hour, 10);
  if (Time->Hour < 10) {
    StrCat (TimeStr, L"0");
  }
  StrCat (TimeStr, Hour);
  StrCat (TimeStr, L":");

  //
  // Minute
  //
  Int64ToUnic (Time->Minute, Minute, 10);
  if (Time->Minute < 10) {
    StrCat (TimeStr, L"0");
  }
  StrCat (TimeStr, Minute);
  StrCat (TimeStr, L":");

  //
  // Second
  //
  Int64ToUnic (Time->Second, Second, 10);
  if (Time->Second < 10) {
    StrCat (TimeStr, L"0");
  }
  StrCat (TimeStr, Second);

  FreePool (Hour);
  FreePool (Minute);
  FreePool (Second);

  return EFI_SUCCESS;
}

VOID
DrawClock (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS   Status;
  EFI_TIME     Time;

  //
  // Check params
  //
  if (gGraphicsOutput == NULL) {
    ASSERT (FALSE);
  }
  //
  // Time init
  //
  CHAR16 *TimeStr = AllocatePool (sizeof(CHAR16) * 10);
  ZeroMem (TimeStr, sizeof(CHAR16) * 10);

  gRT->GetTime(&Time, NULL);

  Status = FormatTime (&Time, TimeStr);
  ASSERT (!EFI_ERROR(Status));
  //
  // Recover background
  //
  Status = gGraphicsOutput->Blt (
                             gGraphicsOutput,
                             gBackground,
                             EfiBltBufferToVideo,
                             0, 0,
                             gClock->Area.StartX,
                             gClock->Area.StartY,
                             CLOCK_WIDTH,
                             CLOCK_HEIGHT,
                             0
                             );
  ASSERT (!EFI_ERROR(Status));
  //
  // Display time
  //
  Status = DisplayFont2 (
    gGraphicsOutput,
    TimeStr,
    gClock->FontColor,
    gClock->Area.StartX,
    gClock->Area.StartY
    );

  ASSERT (!EFI_ERROR(Status));

  FreePool (TimeStr);
}

EFI_STATUS
ClockConstruct (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  IN OUT CLOCK_INFO            *Clock,
  EFI_EVENT                    *TimeEvent
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  gGraphicsOutput = GraphicsOutput;
  gClock          = Clock;

  gBackground = AllocatePool (sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * CLOCK_WIDTH * CLOCK_HEIGHT);
  ZeroMem (gBackground, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * CLOCK_WIDTH * CLOCK_HEIGHT);
  //
  // Get clock background
  //
  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             gBackground,
                             EfiBltVideoToBltBuffer,
                             Clock->Area.StartX,
                             Clock->Area.StartY,
                             0, 0,
                             CLOCK_WIDTH,
                             CLOCK_HEIGHT,
                             0
                             );
  RETURN_ERROR_STATUS (Status);

  Status = gBS->CreateEvent(
                  EVT_NOTIFY_SIGNAL | EVT_TIMER,
                  TPL_CALLBACK,
                  DrawClock,
                  NULL,
                  TimeEvent
                  );
  RETURN_ERROR_STATUS (Status);

  Status = gBS->SetTimer(
                  *TimeEvent,
                  TimerPeriodic,
                  1000 * 1000
                  );
  RETURN_ERROR_STATUS (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
ClockDestruct (
  EFI_EVENT  *TimeEvent
  )
{
  EFI_STATUS Status;

  Status = gBS->CloseEvent (*TimeEvent);
  RETURN_ERROR_STATUS (Status);

  //
  // Recover background
  //
  Status = gGraphicsOutput->Blt (
                             gGraphicsOutput,
                             gBackground,
                             EfiBltBufferToVideo,
                             0, 0,
                             gClock->Area.StartX,
                             gClock->Area.StartY,
                             CLOCK_WIDTH,
                             CLOCK_HEIGHT,
                             0
                             );
  ASSERT (!EFI_ERROR(Status));

  FreePool (gBackground);

  return EFI_SUCCESS;
}


