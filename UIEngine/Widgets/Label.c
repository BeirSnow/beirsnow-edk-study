#include "Label.h"

EFI_STATUS
LabelConstruct (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  IN OUT LABEL_INFO            *Label
  )
{
  EFI_STATUS       Status;
  RECT_BLOCK_STYLE RectStyle;
  //
  // Check params
  //
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  switch (Label->Style) {
    case LABEL_RECT_BLOCK_NORMAL:
      RectStyle = NORMAL_RECT_BLK;
      break;
    default:
      break;
  }
  //
  // Correct Width and Height
  //
  Label->Area.Width  = MAX(Label->Area.Width, GetStrWidth(Label->Str) + 2 * Label->Area.FrameWidth + 10);
  Label->Area.Height = MAX(Label->Area.Height, GetStrHeight(Label->Str) + 2 * Label->Area.FrameWidth);
  //
  // RectBlock info init
  //
  RECT_BLOCK_INFO RectBlockInfo = {
    {Label->Area.StartX, Label->Area.StartY},
    Label->Area.Width,
    Label->Area.Height,
    Label->Area.FrameWidth,
    RectStyle,
    0,
    Label->Color.FrameColor,
    Label->Color.FilledColor
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
  //
  // Display font
  //
  DisplayFont2 (
    GraphicsOutput,
    Label->Str,
    Label->Color.FontColor,
    Label->Area.StartX + ((Label->Area.Width - GetStrWidth(Label->Str)) / 2),
    Label->Area.StartY + ((Label->Area.Height - GetStrHeight(Label->Str)) / 2)
    );

  return EFI_SUCCESS;
}
