#include  "Button.h"

EFI_STATUS
ButtonConstruct (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  IN OUT BUTTON_INFO           *Button
  )
{
  EFI_STATUS       Status;
  RECT_BLOCK_STYLE RectStyle;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FrameColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FilledColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FontColor;
  //
  // Check params
  //
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  switch (Button->Style) {
    case BUTTON_RECT_BLOCK_NORMAL:
      RectStyle = NORMAL_RECT_BLK;
      break;
    default:
      break;
  }
  //
  // Correct Width and Height
  //
  Button->Area.Width  = MAX(Button->Area.Width, GetStrWidth(Button->Str) + 2 * Button->Area.FrameWidth + 10);
  Button->Area.Height = MAX(Button->Area.Height, GetStrHeight(Button->Str) + 2 * Button->Area.FrameWidth);
  //
  // RectBlock info init
  //
  if (Button->Status == 0) {
    FrameColor  = Button->NotSlctColor.FrameColor;
    FilledColor = Button->NotSlctColor.FilledColor;
  } else {
    FrameColor  = Button->SlctColor.FrameColor;
    FilledColor = Button->SlctColor.FilledColor;
  }
  RECT_BLOCK_INFO RectBlockInfo = {
    {Button->Area.StartX, Button->Area.StartY},
    Button->Area.Width,
    Button->Area.Height,
    Button->Area.FrameWidth,
    RectStyle,
    0,
    FrameColor,
    FilledColor
  };
  //
  // Display rect block
  //
  Status = DrawRectBlock (
             GraphicsOutput,
             &RectBlockInfo
             );
  RETURN_ERROR_STATUS (Status);
  //
  // Font info init
  //
  if (Button->Status == 0) {
    FontColor = Button->NotSlctColor.FontColor;
  } else {
    FontColor = Button->SlctColor.FontColor;
  }
  //
  // Display font
  //
  DisplayFont2 (
    GraphicsOutput,
    Button->Str,
    FontColor,
    Button->Area.StartX + ((Button->Area.Width - GetStrWidth(Button->Str)) / 2),
    Button->Area.StartY + ((Button->Area.Height - GetStrHeight(Button->Str)) / 2)
    );

  return EFI_SUCCESS;
}
