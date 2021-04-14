#include "DisplayFont.h"

UINTN
GetStrWidth (
  IN CHAR16 *Str
  )
{
  UINTN Width = 0;

  for(; *Str != L'\0'; Str++) {
    if(*Str >= 0x4e00 && *Str <= 0x9fa4) {
      Width += WIDE_FONT_ADVANCE_X;
    } else{
      Width += NARROW_FONT_ADVANCE_X;
    }
  }
  return Width;
}

UINTN
GetStrHeight (
  IN CHAR16 *Str
  )
{
  UINTN NarrowHeight = 0;
  UINTN WideHeight   = 0;

  for(; *Str != L'\0'; Str++) {
    if(*Str >= 0x4e00 && *Str <= 0x9fa4) {
      WideHeight   = WIDE_FONT_HEIGHT + WIDE_FONT_OFFSET_Y;
    } else {
      NarrowHeight = NARROW_FONT_HEIGHT + NARROW_FONT_OFFSET_Y;
    }
  }
  return MAX(NarrowHeight, WideHeight);
}

/**
  Display font
**/
EFI_STATUS
DisplayFont (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  CHAR16                        *Str,
  IN  EFI_FONT_DISPLAY_INFO         *StrInfo,
  UINTN                             PosX,
  UINTN                             PosY
  )
{
  EFI_STATUS Status;
  EFI_HII_FONT_PROTOCOL *HiiFont;
  //
  // Check params
  //
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (PosX >= GraphicsOutput->Mode->Info->HorizontalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  if (PosY >= GraphicsOutput->Mode->Info->VerticalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Locate protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiFontProtocolGuid,
                  NULL,
                  (VOID **)&HiiFont
                  );
  RETURN_ERROR_STATUS (Status);

  CopyMem(StrInfo->FontInfo.FontName, L"NormalFont", StrLen(L"NormalFont") * 2 + 2);

  EFI_IMAGE_OUTPUT Screen = {
    GraphicsOutput->Mode->Info->HorizontalResolution,
    GraphicsOutput->Mode->Info->VerticalResolution,
    .Image.Screen = GraphicsOutput
  };

  EFI_IMAGE_OUTPUT *Screen_p = &Screen;

  UINT32 HiiOutFlag = EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
                      EFI_HII_OUT_FLAG_CLIP_CLEAN_Y | EFI_HII_OUT_FLAG_CLIP_CLEAN_X |
                      EFI_HII_IGNORE_LINE_BREAK | EFI_HII_OUT_FLAG_TRANSPARENT |
                      EFI_HII_DIRECT_TO_SCREEN;

  Status = HiiFont->StringToImage (
                      HiiFont,
                      HiiOutFlag,
                      Str,
                      StrInfo,
                      &Screen_p,
                      PosX,
                      PosY,
                      NULL,
                      NULL,
                      NULL
                      );
  RETURN_ERROR_STATUS (Status);

  return EFI_SUCCESS;
}

/**
  Display font
**/
EFI_STATUS
DisplayFont2 (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  CHAR16                        *Str,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Color,
  UINTN                             PosX,
  UINTN                             PosY
  )
{
  EFI_STATUS Status;
  EFI_HII_FONT_PROTOCOL *HiiFont;
  UINTN                 Width;
  UINTN                 Height;
  EFI_FONT_DISPLAY_INFO StrInfo = {
    Color,
    Color,
    EFI_FONT_INFO_ANY_FONT,
    {
      EFI_HII_FONT_STYLE_NORMAL,
      19,
      {L'D'}
    }
  };

  //
  // Check params
  //
  if (GraphicsOutput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (PosX >= GraphicsOutput->Mode->Info->HorizontalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  if (PosY >= GraphicsOutput->Mode->Info->VerticalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  Height = MAX ((WIDE_FONT_HEIGHT + WIDE_FONT_OFFSET_Y), (NARROW_FONT_HEIGHT + NARROW_FONT_OFFSET_Y));
  Width  = GetStrWidth(Str);

  //
  // Correct width and height
  //
  if (PosX + Width > GraphicsOutput->Mode->Info->HorizontalResolution) {
    Width = GraphicsOutput->Mode->Info->HorizontalResolution - PosX;
  }

  if (PosY + Height > GraphicsOutput->Mode->Info->VerticalResolution) {
    Height = GraphicsOutput->Mode->Info->VerticalResolution - PosY;
  }
  //
  // Locate protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiFontProtocolGuid,
                  NULL,
                  (VOID **)&HiiFont
                  );
  RETURN_ERROR_STATUS (Status);

  EFI_IMAGE_OUTPUT  ImageOutput = {Width, Height, {NULL}};
  ImageOutput.Image.Bitmap = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)AllocatePool(Width * Height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  if (ImageOutput.Image.Bitmap == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Get raw data from video
  //
  Status = GraphicsOutput->Blt (
             GraphicsOutput,
             ImageOutput.Image.Bitmap,
             EfiBltVideoToBltBuffer,
             PosX,
             PosY,
             0, 0,
             Width,
             Height,
             0
             );
  RETURN_ERROR_STATUS (Status);

  EFI_IMAGE_OUTPUT  *ImageOutput_p = &ImageOutput;

  CopyMem(StrInfo.FontInfo.FontName, L"DMTestFont", StrLen(L"DMTestFont") * 2 + 2);

  UINT32 HiiOutFlag = EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
                      EFI_HII_OUT_FLAG_CLIP_CLEAN_Y | EFI_HII_OUT_FLAG_CLIP_CLEAN_X |
                      EFI_HII_IGNORE_LINE_BREAK | EFI_HII_OUT_FLAG_TRANSPARENT;

  Status = HiiFont->StringToImage (
                      HiiFont,
                      HiiOutFlag,
                      Str,
                      &StrInfo,
                      &ImageOutput_p,
                      0,
                      0,
                      NULL,
                      NULL,
                      NULL
                      );
  RETURN_ERROR_STATUS (Status);

  //
  // Display the font
  //
  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             ImageOutput.Image.Bitmap,
                             EfiBltBufferToVideo,
                             0, 0,
                             PosX, PosY,
                             Width,
                             Height,
                             0
                             );

  FreePool (ImageOutput.Image.Bitmap);

  return EFI_SUCCESS;
}
