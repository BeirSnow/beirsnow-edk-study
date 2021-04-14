#include "DrawLine.h"

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
  )
{
  EFI_STATUS  Status;

  //
  // Check params
  //
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check params
  //
  if (Width == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (StartPoint.X >= GraphicsOutput->Mode->Info->HorizontalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  if (StartPoint.Y > GraphicsOutput->Mode->Info->VerticalResolution - Width) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Correct length
  //
  if (StartPoint.X + Length > GraphicsOutput->Mode->Info->HorizontalResolution) {
    Length = GraphicsOutput->Mode->Info->HorizontalResolution - StartPoint.X;
  }

  //
  // Draw line
  //
  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             Color,
                             EfiBltVideoFill,
                             0, 0,
                             StartPoint.X,
                             StartPoint.Y,
                             Length,
                             Width,
                             0
                             );
  return Status;
}

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
  )
{
  EFI_STATUS  Status;

  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check params
  //
  if (Width == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (StartPoint.X > GraphicsOutput->Mode->Info->HorizontalResolution - Width) {
    return EFI_INVALID_PARAMETER;
  }

  if (StartPoint.Y >= GraphicsOutput->Mode->Info->VerticalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Correct length
  //
  if (StartPoint.Y + Length > GraphicsOutput->Mode->Info->VerticalResolution) {
    Length = GraphicsOutput->Mode->Info->VerticalResolution - StartPoint.Y;
  }

  //
  // Draw line
  //
  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             Color,
                             EfiBltVideoFill,
                             0, 0,
                             StartPoint.X,
                             StartPoint.Y,
                             Width,
                             Length,
                             0
                             );
  return Status;
}

/**
  Draw a skew line
**/
EFI_STATUS
DrawLineSkew (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  POINT_T                          StartPoint,
  POINT_T                          EndPoint,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Color
  )
{
  EFI_STATUS  Status;
  UINTN       Width;
  UINTN       Height;
  UINTN       Row;
  UINTN       Col;
  BOOLEAN     Reverse = FALSE;

  //
  // Check params
  //
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (MAX(StartPoint.X, EndPoint.X) >= GraphicsOutput->Mode->Info->HorizontalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  if (MAX(StartPoint.Y, EndPoint.Y) >= GraphicsOutput->Mode->Info->VerticalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer = NULL;

  //
  // Keep the EndPoint.Y > StartPoint.Y
  //
  if (StartPoint.Y > EndPoint.Y) {
    Reverse = TRUE;
    StartPoint.Y ^= EndPoint.Y;
    EndPoint.Y   ^= StartPoint.Y;
    StartPoint.Y ^= EndPoint.Y;
  }
  Width  = EndPoint.X - StartPoint.X;
  Height = EndPoint.Y - StartPoint.Y;

  BltBuffer = AllocatePool (sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height);
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
                             StartPoint.X,
                             StartPoint.Y,
                             0, 0,
                             Width,
                             Height,
                             0
                             );
  RETURN_ERROR_STATUS (Status);
  //
  // Save the skew line data to buffer
  //
  for (Row = 0; Row < Height; Row++) {
    for (Col = 0; Col < Width; Col++) {
      if (Reverse == FALSE && (Col * Height) == (Row * Width)) {
        BltBuffer[Row * Width + Col].Blue = Color->Blue;
        BltBuffer[Row * Width + Col].Green = Color->Green;
        BltBuffer[Row * Width + Col].Red = Color->Red;
      }
      if (Reverse == TRUE && ((Width * Height) == (Row * Width) + (Col * Height))) {
        BltBuffer[Row * Width + Col].Blue = Color->Blue;
        BltBuffer[Row * Width + Col].Green = Color->Green;
        BltBuffer[Row * Width + Col].Red = Color->Red;
      }
    }
  }
  //
  // Draw the skew line
  //
  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             BltBuffer,
                             EfiBltBufferToVideo,
                             0, 0,
                             StartPoint.X,
                             StartPoint.Y,
                             Width,
                             Height,
                             0
                             );
  FreePool (BltBuffer);

  return Status;
}
