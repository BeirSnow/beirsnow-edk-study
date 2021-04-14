#include  "UI_Engine.h"
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiRuntimeServicesTableLib.h>

/**
  For debug
**/
VOID
PrintModeInfo (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode
  )
{
  CHAR16 *PixelFormatBuf[5] = {
    L"RGBA",
    L"BGRA",
    L"BitMask",
    L"BltOnly",
  };
  Print (L"Mode->Mode                       = %d\n", Mode->Mode);
  Print (L"Mode->Info->HorizontalResolution = %d\n", Mode->Info->HorizontalResolution);
  Print (L"Mode->Info->VerticalResolution   = %d\n", Mode->Info->VerticalResolution);
  Print (L"Mode->Info->PixelFormat          = %s\n", PixelFormatBuf[Mode->Info->PixelFormat]);
  Print (L"Mode->FrameBufferBase            = 0x%lx\n", Mode->FrameBufferBase);
  Print (L"Mode->FrameBufferSize            = 0x%lx\n", Mode->FrameBufferSize);
}
//
// Main
//
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  OUT EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE Mode;
  UINTN                                 FrameWidth;
  UINTN                                 FrameHeight;
  BOOLEAN Flag = FALSE;

  //
  // Install font
  //
  Status = InstallDMFontPkg ();
  RETURN_ERROR_STATUS (Status);

  //
  // Locate GraphicsOutput protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  (VOID **)&GraphicsOutput
                  );
  RETURN_ERROR_STATUS (Status);

  //
  // Get info
  //
  Status = GetFrameInfo (GraphicsOutput, &Mode);
  RETURN_ERROR_STATUS (Status);

  PrintModeInfo (&Mode);

  FrameWidth = Mode.Info->HorizontalResolution;
  FrameHeight = Mode.Info->VerticalResolution;

  //
  // Clear screen
  //
  Status = ClearScreen (GraphicsOutput);
  RETURN_ERROR_STATUS (Status);

  //
  // Draw background, no alpha
  //
  RECT_BLOCK_INFO RectBlockInfo = {
    {0, 0},
    FrameWidth / 20,
    FrameHeight / 20,
    1,
    NORMAL_RECT_BLK,
    0,
    PIXEL_COLOR_GRAY,
    PIXEL_COLOR_WHITE
  };

  UINTN i, j;
  for (i = 0; i < 20; i++) {
    for (j = 0; j < 20; j++) {
      RectBlockInfo.Start.X = RectBlockInfo.Width * j;
      RectBlockInfo.Start.Y = RectBlockInfo.Height * i;
      Status = DrawRectBlock (
          GraphicsOutput,
          &RectBlockInfo
          );
      RETURN_ERROR_STATUS (Status);
    }
  }

  //
  // Draw label
  //
  UINTN X = 7;
  LABEL_INFO  Label = {
    L"TEST-LABEL",
    {X, 15, 10, 10, 2},
    LABEL_RECT_BLOCK_NORMAL,
    {{0xd6, 0xd9, 0x7f, 0}, {0x68, 0xe8, 0xed, 20}, {0xd6, 0xd9, 0x7f, 0}},
  };
  for (i = 0; i < 8; i++) {
    Status = LabelConstruct (
        GraphicsOutput,
        &Label
        );
    RETURN_ERROR_STATUS (Status);
    X += Label.Area.Width;
    X += 5;
    Label.Area.StartX = X;
  }

  //
  // Draw button
  //
  X = 17;
  BUTTON_INFO  Button = {
    0,
    L"TEST-BOTTON ▼",
    {X, 45, 10, 10, 2},
    BUTTON_RECT_BLOCK_NORMAL,
    {{0xcc, 0xa5, 0xe8, 0}, {0xcb, 0xd2, 0xdf, 15}, {0xcc, 0xa5, 0xe8, 0}},
    {{0xcb, 0xd2, 0xdf, 0}, {0xcc, 0xa5, 0xe8, 15}, {0xcb, 0xd2, 0xdf, 0}},
    NULL
  };
  for (i = 0; i < 7; i++) {
    Status = ButtonConstruct (
        GraphicsOutput,
        &Button
        );
    RETURN_ERROR_STATUS (Status);
    X += Button.Area.Width;
    X += 5;
    Button.Area.StartX = X;
    Button.Status = !Button.Status;
  }
#if 0
  //
  // Draw clock
  //
  X += Button.Area.Width;
  X += 5;

  EFI_EVENT     TimeEvent;

  CLOCK_INFO Clock = {
    {X, 10},
    {31, 97, 236, 0}
  };

  Status = ClockConstruct (
    GraphicsOutput,
    &Clock,
    &TimeEvent
  );
  RETURN_ERROR_STATUS (Status);
#endif
  //
  // Draw pop window 2
  //
  POP_WINDOW_INFO PopWindow1 = {
    {
      L"伍兹行不行?",
      L"行",
      L"不行"
    },
    {250, 200, 200, 100, 3},
    POP_RECT_BLOCK_NORMAL,
    {{115, 167, 255, 15}, {148, 146, 6, 15}, {48, 200, 120, 15}}
  };
  Status = PopWindowConstruct (
    GraphicsOutput,
    &PopWindow1,
    &Flag
  );
  RETURN_ERROR_STATUS (Status);
#if 0
  Status = ClockDestruct (&TimeEvent);
  RETURN_ERROR_STATUS (Status);
#endif
  //
  // Unload font
  //
  Status = UnLoadFont ();
  RETURN_ERROR_STATUS (Status);

  return EFI_SUCCESS;
}
