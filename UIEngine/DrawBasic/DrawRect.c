#include "DrawRect.h"

/**
  Draw a rect
**/
EFI_STATUS
DrawRect (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN RECT_INFO                     *Rect
  )
{
  EFI_STATUS  Status;
  POINT_T     Point;

  //
  // Check params
  //
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Rect->Start.X >= GraphicsOutput->Mode->Info->HorizontalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  if (Rect->Start.Y >= GraphicsOutput->Mode->Info->VerticalResolution) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Correct width and height
  //
  if (Rect->Start.X + Rect->Width > GraphicsOutput->Mode->Info->HorizontalResolution) {
    Rect->Width = GraphicsOutput->Mode->Info->HorizontalResolution - Rect->Start.X;
  }

  if (Rect->Start.Y + Rect->Height > GraphicsOutput->Mode->Info->VerticalResolution) {
    Rect->Height = GraphicsOutput->Mode->Info->VerticalResolution - Rect->Start.Y;
  }
  //
  // Draw left ver line
  //
  Point = Rect->Start;
  Status = DrawLineVer (GraphicsOutput, Point, Rect->Height, Rect->FrameWidth, &Rect->FrameColor);

  //
  // Draw top hor line
  //
  Status = DrawLineHor (GraphicsOutput, Point, Rect->Width, Rect->FrameWidth, &Rect->FrameColor);

  //
  // Draw right ver line
  //
  Point.X += Rect->Width;
  Point.X -= Rect->FrameWidth;
  Status = DrawLineVer (GraphicsOutput, Point, Rect->Height, Rect->FrameWidth, &Rect->FrameColor);

  //
  // Draw bottom hor line
  //
  Point.X =  Rect->Start.X;
  Point.Y += Rect->Height;
  Point.Y -= Rect->FrameWidth;
  Status = DrawLineHor (GraphicsOutput, Point, Rect->Width, Rect->FrameWidth, &Rect->FrameColor);

  return Status;
}

/**
  Draw a rect block
**/
EFI_STATUS
DrawRectBlock (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN RECT_BLOCK_INFO               *Rect
  )
{
  EFI_STATUS  Status;
  UINTN       Row;
  UINTN       Col;
  UINT32      Blue0;
  UINT32      Blue1;
  UINT32      Green0;
  UINT32      Green1;
  UINT32      Red0;
  UINT32      Red1;
  UINT32      Alpha;
  UINT32      GradientTmp;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *RawBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;

  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Rect->FrameColor.Reserved > 100 || Rect->FilledColor.Reserved > 100) {
    return EFI_INVALID_PARAMETER;
  }

  if (Rect->Start.X >= GraphicsOutput->Mode->Info->HorizontalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  if (Rect->Start.Y >= GraphicsOutput->Mode->Info->VerticalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Correct width and height
  //
  if ((Rect->Start.X + Rect->Width) > GraphicsOutput->Mode->Info->HorizontalResolution) {
    Rect->Width = GraphicsOutput->Mode->Info->HorizontalResolution - Rect->Start.X;
  }

  if ((Rect->Start.Y + Rect->Height) > GraphicsOutput->Mode->Info->VerticalResolution) {
    Rect->Height = GraphicsOutput->Mode->Info->VerticalResolution - Rect->Start.Y;
  }
  //
  // Allocate BltBuffer
  //
  BltBuffer = AllocatePool (sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Rect->Width * Rect->Height);
  if (BltBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // For Alpha
  //
  if (Rect->FrameColor.Reserved != 0 || Rect->FilledColor.Reserved != 0) {
    RawBuffer = AllocatePool (sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Rect->Width * Rect->Height);
    if (RawBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Get raw data from video
    //
    Status = GraphicsOutput->Blt (
                               GraphicsOutput,
                               RawBuffer,
                               EfiBltVideoToBltBuffer,
                               Rect->Start.X,
                               Rect->Start.Y,
                               0, 0,
                               Rect->Width,
                               Rect->Height,
                               0
                               );
    RETURN_ERROR_STATUS (Status);

    //
    // Get mixed data
    //
    for (Row = 0; Row < Rect->Height; Row++) {
      for (Col = 0; Col < Rect->Width; Col++) {
        Blue0  = RawBuffer[Row * Rect->Width + Col].Blue;
        Green0 = RawBuffer[Row * Rect->Width + Col].Green;
        Red0   = RawBuffer[Row * Rect->Width + Col].Red;

        if (Row < Rect->FrameWidth ||
            Row >= (Rect->Height - Rect->FrameWidth) ||
            Col < Rect->FrameWidth ||
            Col >= (Rect->Width - Rect->FrameWidth)) {
          Blue1  = Rect->FrameColor.Blue;
          Green1 = Rect->FrameColor.Green;
          Red1   = Rect->FrameColor.Red;
          Alpha  = 100 - Rect->FrameColor.Reserved;
        } else {
          switch (Rect->Style) {
            case NORMAL_RECT_BLK:
              GradientTmp = 0;
              break;
            case VER_GRADIENT:
              GradientTmp = Rect->Gradient * Row / (GraphicsOutput->Mode->Info->VerticalResolution / 255);
              break;
            case VER_REVERSE_GRADIENT:
              GradientTmp = Rect->Gradient * (Rect->Height - Row - 1) / (GraphicsOutput->Mode->Info->VerticalResolution / 255);
              break;
            case HOR_GRADIENT:
              GradientTmp = Rect->Gradient * Col / (GraphicsOutput->Mode->Info->HorizontalResolution / 255);
              break;
            case HOR_REVERSE_GRADIENT:
              GradientTmp = Rect->Gradient * (Rect->Width - Col - 1) / (GraphicsOutput->Mode->Info->HorizontalResolution / 255);
              break;
            default:
              return EFI_INVALID_PARAMETER;
          }
          Blue1  = MIN((UINT32)Rect->FilledColor.Blue + GradientTmp, 255);
          Green1 = MIN((UINT32)Rect->FilledColor.Green + GradientTmp, 255);
          Red1   = MIN((UINT32)Rect->FilledColor.Red + GradientTmp, 255);
          Alpha  = 100 - Rect->FilledColor.Reserved;
        }
        BltBuffer[Row * Rect->Width + Col].Blue  = (UINT8)((Blue0 * (100 - Alpha) + Blue1 * Alpha) / 100);
        BltBuffer[Row * Rect->Width + Col].Green = (UINT8)((Green0 * (100 - Alpha) + Green1 * Alpha) / 100);
        BltBuffer[Row * Rect->Width + Col].Red   = (UINT8)((Red0 * (100 - Alpha) + Red1 * Alpha) / 100);
      }
    }
    FreePool (RawBuffer);
  } else {
    //
    // Normal
    //
    for (Row = 0; Row < Rect->Height; Row++) {
      for (Col = 0; Col < Rect->Width; Col++) {
        BltBuffer[Row * Rect->Width + Col].Reserved = 0;
        if (Row < Rect->FrameWidth ||
            Row >= (Rect->Height - Rect->FrameWidth) ||
            Col < Rect->FrameWidth ||
            Col >= (Rect->Width - Rect->FrameWidth)) {
          BltBuffer[Row * Rect->Width + Col].Blue  = Rect->FrameColor.Blue;
          BltBuffer[Row * Rect->Width + Col].Green = Rect->FrameColor.Green;
          BltBuffer[Row * Rect->Width + Col].Red   = Rect->FrameColor.Red;
        } else {
          switch (Rect->Style) {
            case NORMAL_RECT_BLK:
              GradientTmp = 0;
              break;
            case VER_GRADIENT:
              GradientTmp = Rect->Gradient * Row / (GraphicsOutput->Mode->Info->VerticalResolution / 255);
              break;
            case VER_REVERSE_GRADIENT:
              GradientTmp = Rect->Gradient * (Rect->Height - Row - 1) / (GraphicsOutput->Mode->Info->VerticalResolution / 255);
              break;
            case HOR_GRADIENT:
              GradientTmp = Rect->Gradient * Col / (GraphicsOutput->Mode->Info->HorizontalResolution / 255);
              break;
            case HOR_REVERSE_GRADIENT:
              GradientTmp = Rect->Gradient * (Rect->Width - Col - 1) / (GraphicsOutput->Mode->Info->HorizontalResolution / 255);
              break;
            default:
              return EFI_INVALID_PARAMETER;
          }
          BltBuffer[Row * Rect->Width + Col].Blue  = MIN((UINT32)Rect->FilledColor.Blue + GradientTmp, 255);
          BltBuffer[Row * Rect->Width + Col].Green = MIN((UINT32)Rect->FilledColor.Green + GradientTmp, 255);
          BltBuffer[Row * Rect->Width + Col].Red   = MIN((UINT32)Rect->FilledColor.Red + GradientTmp, 255);
        }
      }
    }
  }
  //
  // Draw the rect
  //
  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             BltBuffer,
                             EfiBltBufferToVideo,
                             0, 0,
                             Rect->Start.X,
                             Rect->Start.Y,
                             Rect->Width,
                             Rect->Height,
                             0
                             );

  FreePool (BltBuffer);

  return Status;
}
