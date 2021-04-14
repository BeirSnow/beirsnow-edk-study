#include "InstallDMFont.h"

UINT16 WideGlyphBytes = WIDE_FONT_WIDTH * WIDE_FONT_HEIGHT / 8;
UINT16 NrGlyphBytes = 19;

static EFI_GUID gDMFontPackageListGuid = {0xf6645675, 0x6106, 0x3138, {0x5c, 0xcd, 0xda, 0x1a, 0xab, 0x3b, 0x26, 0xa2}};

//Font functions
EFI_STATUS
FillNarrowGLYPH (
  UINT8            *p,
  EFI_NARROW_GLYPH *NarrowGlyph,
  UINT32           SizeInBytes,
  CHAR16           Next,
  CHAR16           NrStart,
  CHAR16           NrCharNum
  )
{
  UINT8  *BitmapData = NULL;
  UINTN  Length = 0;
  UINT16 i;
  UINT16 j;
  EFI_HII_GIBT_GLYPHS_BLOCK *GlyphsBlock;
  EFI_HII_GLYPH_INFO Cell = {
    NARROW_FONT_WIDTH,
    NARROW_FONT_HEIGHT,
    NARROW_FONT_OFFSET_X,
    NARROW_FONT_OFFSET_Y,
    NARROW_FONT_ADVANCE_X
  };

  //
  // EFI_HII_GIBT_SKIP2_BLOCK
  //
  if(Next != NrStart){
    EFI_HII_GIBT_SKIP2_BLOCK* FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK*)p;
    FontSkip2Block ->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
    FontSkip2Block ->SkipCount        = NrStart - Next ;

    p = (UINT8 *)(FontSkip2Block + 1);
    Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
  }

  //
  // EFI_HII_GIBT_GLYPHS_BLOCK
  //
  GlyphsBlock = (EFI_HII_GIBT_GLYPHS_BLOCK *)(p);
  GlyphsBlock -> Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS;
  GlyphsBlock -> Cell = Cell;
  GlyphsBlock -> Count = NrCharNum;
  BitmapData = GlyphsBlock->BitmapData;

  for (i = 0; i < NrCharNum; i++) {
    for (j = 0; j < 19; j++) {
      BitmapData[j] = NarrowGlyph[i].GlyphCol1[j];
    }
    BitmapData += 19;
  }

  Length += (sizeof(EFI_HII_GIBT_GLYPHS_BLOCK ) + 19 * NrCharNum - 1);

  return (EFI_STATUS)Length;
}

EFI_STATUS
FillWideGLYPH (
  UINT8          *p,
  EFI_WIDE_GLYPH *WideGlyph,
  UINT32         SizeInBytes,
  CHAR16         Next,
  CHAR16         Start,
  CHAR16         CharNum
  )
{
  UINT8* BitmapData = NULL;
  UINTN  Length     = 0;
  UINT16 i;
  UINT16 j;
  EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *GlyphsDefaultBlock;

  // EFI_HII_GIBT_SKIP2_BLOCK
  if (Next != Start) {
    EFI_HII_GIBT_SKIP2_BLOCK *FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK*)p;
    FontSkip2Block->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
    FontSkip2Block->SkipCount = Start - Next ;

    p = (UINT8*)(FontSkip2Block + 1);
    Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
  }

  // EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK
  GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK*)(p);
  GlyphsDefaultBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS_DEFAULT;
  GlyphsDefaultBlock->Count            = CharNum;
  BitmapData = GlyphsDefaultBlock->BitmapData;
  for(i = 0; i < CharNum; i++){
    for(j = 0; j < WideGlyphBytes; j++){
      BitmapData[j] = WideGlyph[i].GlyphCol1[(j % 2) * 19 + j / 2];
    }
    BitmapData += WideGlyphBytes;
  }
  Length += ( sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) + WideGlyphBytes * CharNum -1);
  return (EFI_STATUS)Length;
}

EFI_STATUS
InstallDMFontPkg (
  VOID
  )
{
  EFI_HII_FONT_PACKAGE_HDR *FontPkgHeader;
  EFI_GLYPH_GIBT_END_BLOCK *FontEndBlock;
  EFI_HII_GLYPH_INFO       Cell;
  UINT32                   PackageLength;
  UINT8                    *Package;
  UINT16                   FontNameLen;
  CHAR16                   *FontName;
  UINT16                   CharNum;
  UINT16                   NrCharNum;
  UINT8                    *pCurrent = 0;
  UINTN                    BlockLength = 0;
  CHAR16                   NextChar;
  CHAR16                   NrStart = 32;

  FontName    = L"NormalFont";
  FontNameLen = (UINT16)StrLen (FontName);

  CharNum   = (CHAR16)(gSimpleFontBytes / sizeof(EFI_WIDE_GLYPH));
  NrCharNum = (CHAR16)(mNarrowFontSize  / sizeof(EFI_NARROW_GLYPH)),
  //
  // Init Cell
  //
  Cell.Width    = WIDE_FONT_WIDTH;
  Cell.Height   = WIDE_FONT_HEIGHT;
  Cell.OffsetX  = WIDE_FONT_OFFSET_X;
  Cell.OffsetY  = WIDE_FONT_OFFSET_Y;
  Cell.AdvanceX = WIDE_FONT_ADVANCE_X;

  //
  // PackageLength
  //
  PackageLength = 4 + sizeof (EFI_HII_FONT_PACKAGE_HDR) + (FontNameLen * 2 /*Max Length of Font Name*/) +
    sizeof(EFI_HII_GIBT_SKIP2_BLOCK) +
    sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) -1 + CharNum * WideGlyphBytes +
    sizeof(EFI_HII_GIBT_SKIP2_BLOCK) +
    sizeof(EFI_HII_GIBT_GLYPHS_BLOCK) -1 + NrCharNum * NrGlyphBytes  +
    sizeof(EFI_GLYPH_GIBT_END_BLOCK) ;

  Package = (UINT8*)AllocateZeroPool (PackageLength);
  if (Package == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Pkg header
  //
  WriteUnaligned32((UINT32 *)Package, PackageLength);

  FontPkgHeader = (EFI_HII_FONT_PACKAGE_HDR *)(Package + 4);
  FontPkgHeader->Header.Length    = (UINT32) (PackageLength - 4);
  FontPkgHeader->Header.Type      = EFI_HII_PACKAGE_FONTS;
  FontPkgHeader->HdrSize          = sizeof(EFI_HII_FONT_PACKAGE_HDR) + (FontNameLen) * 2;
  FontPkgHeader->GlyphBlockOffset = sizeof(EFI_HII_FONT_PACKAGE_HDR) + (FontNameLen) * 2;
  FontPkgHeader->Cell             = Cell;
  FontPkgHeader->FontStyle        = EFI_HII_FONT_STYLE_NORMAL;
  CopyMem((FontPkgHeader->FontFamily), FontName, StrLen(FontName) * 2 + 2);

  pCurrent = (UINT8 *)(Package + 4 + FontPkgHeader->GlyphBlockOffset);

  //
  // EFI_HII_GIBT_SKIP2_BLOCK (0 ~ 31) and EFI_HII_GIBT_GLYPHS_BLOCK (32 ~ 255)
  //
  NextChar = 1;
  BlockLength = FillNarrowGLYPH(pCurrent, gUsStdNarrowGlyphData, mNarrowFontSize, NextChar, NrStart, NrCharNum);

  pCurrent += BlockLength;
  NextChar = NrStart + NrCharNum;

  //
  // EFI_HII_GIBT_SKIP2_BLOCK (0x100 ~ 0x4dff) and EFI_HII_GIBT_GLYPHS_DEFAULT (0x4e00 ~ 0x9fa4)
  //
  BlockLength = FillWideGLYPH(pCurrent, gSimpleFontWideGlyphData, gSimpleFontBytes, NextChar, 0x4e00, CharNum);
  pCurrent += BlockLength;

  //
  // EFI_GLYPH_GIBT_END_BLOCK
  //
  FontEndBlock = (EFI_GLYPH_GIBT_END_BLOCK*)(pCurrent);
  FontEndBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_END;

  //
  // Add package
  //
  EFI_HANDLE gFontHiiHandle = HiiAddPackages (
                                &gDMFontPackageListGuid,
                                NULL,
                                Package,
                                NULL
                                );
  (VOID)gFontHiiHandle;

  FreePool (Package);

  return EFI_SUCCESS;
}

EFI_STATUS
UnLoadFont (
  VOID
  )
{
  EFI_STATUS        Status = 0;
  EFI_HII_HANDLE    *handles = 0;
  UINT32            i = 0;
  handles = HiiGetHiiHandles (&gDMFontPackageListGuid);
  if (handles == 0) {
    return -1;
  }
  while (handles[i] != 0) {
    HiiRemovePackages(handles[i]);
    i++;
  }
  FreePool(handles);

  return Status;
}
