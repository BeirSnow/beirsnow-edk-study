#include  "PopWindow.h"

EFI_STATUS
PopWindowConstruct (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  IN  OUT POP_WINDOW_INFO      *PopWindow,
  OUT BOOLEAN                  *SlctFlag
  )
{
  EFI_STATUS                         Status;
  RECT_BLOCK_STYLE                   RectStyle;
  UINTN                              MaxStrWidth;
  UINTN                              MaxStrHeight;
  UINTN                              ButtonFrameWidth;
  UINTN                              ButtonWidth;
  UINTN                              ButtonHeight;
  UINT16                             ScanCode;
  UINT16                             UniChar;
  UINT32                             ShiftState;
  EFI_KEY_TOGGLE_STATE               ToggleState;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInputEx;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *BltBuffer = NULL;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *AcceptButtonRawBuffer = NULL;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *CancelButtonRawBuffer = NULL;
  //
  // Check params
  //
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  switch (PopWindow->Style) {
    case POP_RECT_BLOCK_NORMAL:
      RectStyle = NORMAL_RECT_BLK;
      break;
    default:
      break;
  }
  Status = LocateSimpleTextInputEx (&SimpleTextInputEx);
  RETURN_ERROR_STATUS (Status);
  //
  // Correct width and height
  //
  ButtonFrameWidth = (PopWindow->Area.FrameWidth > 1) ? (PopWindow->Area.FrameWidth - 1) : 1;
  ButtonWidth = GetStrWidth(PopWindow->PopWindowStr.AcceptStr) + GetStrWidth(PopWindow->PopWindowStr.CancelStr);
  ButtonWidth += 10;
  ButtonWidth += (2 * ButtonFrameWidth);
  MaxStrWidth = 3 * ButtonWidth;
  MaxStrWidth = MAX(MaxStrWidth, GetStrWidth(PopWindow->PopWindowStr.NoteStr));
  ButtonHeight = MAX(GetStrHeight(PopWindow->PopWindowStr.AcceptStr), GetStrHeight(PopWindow->PopWindowStr.CancelStr));
  ButtonHeight += (2 * ButtonFrameWidth);
  MaxStrHeight = MAX(ButtonHeight, GetStrHeight(PopWindow->PopWindowStr.NoteStr));

  PopWindow->Area.Width  = MAX(PopWindow->Area.Width, MaxStrWidth + 10 + PopWindow->Area.FrameWidth * 2);
  PopWindow->Area.Height = MAX(PopWindow->Area.Height, MaxStrHeight * 2 + 10 + PopWindow->Area.FrameWidth * 2);

  BltBuffer = AllocatePool (sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * PopWindow->Area.Width * PopWindow->Area.Height);
  if (BltBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Get raw data from video
  //
  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             BltBuffer,
                             EfiBltVideoToBltBuffer,
                             PopWindow->Area.StartX,
                             PopWindow->Area.StartY,
                             0, 0,
                             PopWindow->Area.Width,
                             PopWindow->Area.Height,
                             0
                             );
  RETURN_ERROR_STATUS (Status);

  // Initialize rectBlock information
  //
  RECT_BLOCK_INFO RectBlockInfo = {
    {PopWindow->Area.StartX, PopWindow->Area.StartY},
    PopWindow->Area.Width,
    PopWindow->Area.Height,
    PopWindow->Area.FrameWidth,
    RectStyle,
    0,
    PopWindow->Color.FrameColor,
    PopWindow->Color.FilledColor
  };
  //
  // Display the rect block (The frame of PopWindow)
  //
  Status = DrawRectBlock (
             GraphicsOutput,
             &RectBlockInfo
             );
  RETURN_ERROR_STATUS (Status);

  //
  // Display note string
  //
  DisplayFont2 (
    GraphicsOutput,
    PopWindow->PopWindowStr.NoteStr,
    PopWindow->Color.FontColor,
    PopWindow->Area.StartX + ((PopWindow->Area.Width - GetStrWidth(PopWindow->PopWindowStr.NoteStr)) / 2),
    PopWindow->Area.StartY + ((PopWindow->Area.Height - 2 * MaxStrHeight) / 3)
    );

  //
  // Initialize accept button information
  //
  BUTTON_INFO *AcceptButton = (BUTTON_INFO *)AllocatePool(sizeof(BUTTON_INFO));
  AcceptButton->Status = 0;
  AcceptButton->Str = PopWindow->PopWindowStr.AcceptStr;
  AcceptButton->Area.StartX = PopWindow->Area.StartX + (PopWindow->Area.Width - 2 * ButtonWidth) / 3;
  AcceptButton->Area.StartY = PopWindow->Area.StartY + PopWindow->Area.Height - ((PopWindow->Area.Height - 2 * MaxStrHeight) / 3) - ButtonHeight;
  AcceptButton->Area.Width  = ButtonWidth;
  AcceptButton->Area.Height = ButtonHeight;
  AcceptButton->Area.FrameWidth = ButtonFrameWidth;
  AcceptButton->Style = BUTTON_RECT_BLOCK_NORMAL;
  CopyMem (&AcceptButton->NotSlctColor.FrameColor, &PopWindow->Color.FrameColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  CopyMem (&AcceptButton->NotSlctColor.FilledColor, &PopWindow->Color.FilledColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  CopyMem (&AcceptButton->NotSlctColor.FontColor, &PopWindow->Color.FontColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  CopyMem (&AcceptButton->SlctColor.FrameColor, &PopWindow->Color.FontColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  CopyMem (&AcceptButton->SlctColor.FilledColor, &PopWindow->Color.FrameColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  CopyMem (&AcceptButton->SlctColor.FontColor, &PopWindow->Color.FilledColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  AcceptButton->Callback = NULL;

  //
  // Initialize cancel button information
  //
  BUTTON_INFO *CancelButton = (BUTTON_INFO *)AllocatePool(sizeof(BUTTON_INFO));
  CopyMem (CancelButton, AcceptButton, sizeof(BUTTON_INFO));
  CancelButton->Status = 1;
  CancelButton->Str = PopWindow->PopWindowStr.CancelStr;
  CancelButton->Area.StartX = PopWindow->Area.StartX + PopWindow->Area.Width - (PopWindow->Area.Width - 2 * ButtonWidth) / 3 - ButtonWidth;
  AcceptButton->Callback = NULL;

  AcceptButtonRawBuffer = AllocatePool (sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * PopWindow->Area.Width * PopWindow->Area.Height);
  if (AcceptButtonRawBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Get accept button raw data from video
  //
  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             AcceptButtonRawBuffer,
                             EfiBltVideoToBltBuffer,
                             AcceptButton->Area.StartX,
                             AcceptButton->Area.StartY,
                             0, 0,
                             AcceptButton->Area.Width,
                             AcceptButton->Area.Height,
                             0
                             );
  RETURN_ERROR_STATUS (Status);

  CancelButtonRawBuffer = AllocatePool (sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * PopWindow->Area.Width * PopWindow->Area.Height);
  if (CancelButtonRawBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Get cancel button raw data from video
  //
  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             CancelButtonRawBuffer,
                             EfiBltVideoToBltBuffer,
                             CancelButton->Area.StartX,
                             CancelButton->Area.StartY,
                             0, 0,
                             CancelButton->Area.Width,
                             CancelButton->Area.Height,
                             0
                             );
  RETURN_ERROR_STATUS (Status);
  //
  // Draw button
  //
  ButtonConstruct (GraphicsOutput, AcceptButton);
  ButtonConstruct (GraphicsOutput, CancelButton);

  while (TRUE) {
    //
    // Wait for KeyEvent
    //
    Status = GetKeyEx(
               SimpleTextInputEx,
               &ScanCode,
               &UniChar,
               &ShiftState,
               &ToggleState
               );
    RETURN_ERROR_STATUS (Status);

    if (ScanCode == SCAN_RIGHT) {
      //
      // Update button status
      //
      AcceptButton->Status = 0;
      CancelButton->Status = 1;
      ScanCode = 0;
    } else if(ScanCode == SCAN_LEFT) {
      //
      // Update button status
      //
      AcceptButton->Status = 1;
      CancelButton->Status = 0;
      ScanCode = 0;
    } else if (UniChar == CHAR_CARRIAGE_RETURN) {
      //
      // Wirte the value of SlctFlag
      //
      if (AcceptButton->Status == 1) {
        *SlctFlag = TRUE;
      } else {
        *SlctFlag = FALSE;
      }
      //
      // Recover pop window raw data
      //
      Status = GraphicsOutput->Blt (
                                 GraphicsOutput,
                                 BltBuffer,
                                 EfiBltBufferToVideo,
                                 0, 0,
                                 PopWindow->Area.StartX,
                                 PopWindow->Area.StartY,
                                 PopWindow->Area.Width,
                                 PopWindow->Area.Height,
                                 0
                                 );
      return Status;
    }
    //
    // Recover accept button rawdata
    //
    Status = GraphicsOutput->Blt (
                               GraphicsOutput,
                               AcceptButtonRawBuffer,
                               EfiBltBufferToVideo,
                               0, 0,
                               AcceptButton->Area.StartX,
                               AcceptButton->Area.StartY,
                               AcceptButton->Area.Width,
                               AcceptButton->Area.Height,
                               0
                               );
    RETURN_ERROR_STATUS (Status);

    Status = GraphicsOutput->Blt (
                               GraphicsOutput,
                               CancelButtonRawBuffer,
                               EfiBltBufferToVideo,
                               0, 0,
                               CancelButton->Area.StartX,
                               CancelButton->Area.StartY,
                               CancelButton->Area.Width,
                               CancelButton->Area.Height,
                               0
                               );
    RETURN_ERROR_STATUS (Status);

    //
    // Update buttons
    //
    Status = ButtonConstruct (GraphicsOutput, AcceptButton);
    RETURN_ERROR_STATUS (Status);
    Status = ButtonConstruct (GraphicsOutput, CancelButton);
    RETURN_ERROR_STATUS (Status);
  }
  return Status;
}
