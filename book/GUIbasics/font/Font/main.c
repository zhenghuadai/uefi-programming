/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2013 08:22:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <Uefi.h>
#include <Protocol/HiiFont.h>
#include "Protocol/HiiDatabase.h"
#include <Library/UefiHiiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#ifndef __EFI_SHELL_PARAMETERS_PROTOCOL__
typedef struct _EFI_SHELL_PARAMETERS_PROTOCOL {
     CHAR16 **Argv;
     UINTN Argc;
} EFI_SHELL_PARAMETERS_PROTOCOL;
typedef struct {
    EFI_HANDLE                ImageHandle;
    VOID *Info;
    CHAR16                    **Argv;
    UINTN                     Argc;
}EFI_SHELL_INTERFACE;
#endif
extern EFI_WIDE_GLYPH  gDMFontWideGlyphData[];
extern UINT32 gDMFontBytes;
extern EFI_WIDE_GLYPH  gSimpleFontWideGlyphData[];
extern UINT32 gSimpleFontBytes;
extern EFI_NARROW_GLYPH  gUsStdNarrowGlyphData[]; 
extern UINT32 mNarrowFontSize;
static EFI_GUID gDMFontPackageListGuid = {0xf6643673, 0x6006, 0x3738, {0x5c, 0xcd, 0xda, 0x1a, 0xeb, 0x3b, 0x26, 0xa2}};
#define WfontWidth  16
#define WfontHight  19
CONST UINT16 WideGlyphBytes = WfontHight * WfontWidth / 8;
CONST UINT16 NrGlyphBytes = 19;

EFI_STATUS FillNarrowGLYPH(UINT8* p, 
        EFI_NARROW_GLYPH* NarrowGlyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 NrStart, CHAR16 NrCharNum)
{
    UINT8* BitmapData = NULL; 
    UINTN Length = 0;
    EFI_HII_GLYPH_INFO Cell = {8, 19, 0, 0, (INT16)8};

    // SKIP
    if(Next != NrStart){
        EFI_HII_GIBT_SKIP2_BLOCK* FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK*)p;
        FontSkip2Block ->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
        FontSkip2Block ->SkipCount = NrStart - Next ;

        p = (UINT8*)(FontSkip2Block+1);
        Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
    }

    EFI_HII_GIBT_GLYPHS_BLOCK * GlyphsBlock = (EFI_HII_GIBT_GLYPHS_BLOCK *)(p);
    GlyphsBlock -> Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS;
    GlyphsBlock -> Cell = Cell;
    GlyphsBlock -> Count = NrCharNum;
    BitmapData = GlyphsBlock->BitmapData;
    {
        UINT16 i = 0, j = 0; 
        for(i=0;i < NrCharNum; i++){
            for(j=0;j < 19; j++){
                BitmapData[j] = NarrowGlyph[i].GlyphCol1[j];
            }
            BitmapData += 19;
        }
    }
    Length += ( sizeof(EFI_HII_GIBT_GLYPHS_BLOCK ) + 19 * NrCharNum -1); 
    return (EFI_STATUS)Length;
}

EFI_STATUS FillWideGLYPH(UINT8* p, 
        EFI_WIDE_GLYPH* WideGlyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 Start, CHAR16 CharNum)
{
    UINT8* BitmapData = NULL; 
    UINTN Length = 0;

    // SKIP
    if(Next != Start){
        EFI_HII_GIBT_SKIP2_BLOCK* FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK*)p;
        FontSkip2Block ->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
        FontSkip2Block ->SkipCount = Start - Next ;

        p = (UINT8*)(FontSkip2Block+1);
        Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
    }

    EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK* GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK*)(p);
    GlyphsDefaultBlock -> Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS_DEFAULT;
    GlyphsDefaultBlock -> Count = CharNum;
    BitmapData = GlyphsDefaultBlock->BitmapData;
    {
        UINT16 i = 0, j = 0; 
        for(i=0;i < CharNum; i++){
            for(j=0;j < WideGlyphBytes; j++){
                BitmapData[j] = WideGlyph[i].GlyphCol1[(j%2)*19 + j/2];
            }
            BitmapData += WideGlyphBytes;
        }
    }
    Length += ( sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) + WideGlyphBytes * CharNum -1); 
    return (EFI_STATUS)Length;
}

EFI_STATUS FillGLYPH(UINT8* p, 
        UINT8* Glyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 Start, CHAR16 CharNum)
{
    UINT8* BitmapData = NULL; 
    UINTN Length = 0;

    // SKIP
    if(Next != Start){
        EFI_HII_GIBT_SKIP2_BLOCK* FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK*)p;
        FontSkip2Block ->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
        FontSkip2Block ->SkipCount = Start - Next ;

        p = (UINT8*)(FontSkip2Block+1);
        Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
    }

    // 
    EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK* GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK*)(p);
    GlyphsDefaultBlock -> Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS_DEFAULT;
    GlyphsDefaultBlock -> Count = CharNum;
    BitmapData = GlyphsDefaultBlock->BitmapData;
    CopyMem (BitmapData, Glyph, SizeInBytes);
    Length += ( sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) + SizeInBytes -1); 
    return (EFI_STATUS)Length;
}

EFI_STATUS CreateDMFontPkg(CHAR16* FontName, UINT16 FontWidth, UINT16 FontHeight,  
        EFI_NARROW_GLYPH* NarrowGlyph, UINT32 NrSizeInBytes, CHAR16 NrStart, CHAR16 NrCharNum,
        EFI_WIDE_GLYPH* WideGlyph, UINT32 SizeInBytes, CHAR16 Start, CHAR16 CharNum)
{
    EFI_HII_FONT_PACKAGE_HDR *FontPkgHeader; 
    UINT32 PackageLength; 
    UINT8 *Package; 
    UINTN BlockLength = 0;
    UINT8 *pCurrent = 0; 
    CHAR16 NextChar = 0;
    EFI_HII_GLYPH_INFO Cell = {FontWidth, FontHeight, 10, 10, (INT16)FontWidth};

    UINT16 FontNameLen = StrLen(FontName);
    PackageLength = 4 + sizeof (EFI_HII_FONT_PACKAGE_HDR) + (FontNameLen*2 /*Max Length of Font Name*/) + 
        sizeof(EFI_HII_GIBT_SKIP2_BLOCK) + 
        sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) -1 + CharNum * WideGlyphBytes + 
        sizeof(EFI_HII_GIBT_SKIP2_BLOCK) + 
        sizeof(EFI_HII_GIBT_GLYPHS_BLOCK) -1 + NrCharNum * NrGlyphBytes  +
        sizeof(EFI_GLYPH_GIBT_END_BLOCK) ;
    Package = (UINT8*)AllocateZeroPool (PackageLength); 
    //ASSERT (Package != NULL); 

    // Header
    WriteUnaligned32((UINT32 *) Package,PackageLength); 
    FontPkgHeader = (EFI_HII_FONT_PACKAGE_HDR *) (Package + 4); 
    FontPkgHeader->Header.Length = (UINT32) (PackageLength - 4); 
    FontPkgHeader->Header.Type = EFI_HII_PACKAGE_FONTS; 
    FontPkgHeader->HdrSize = sizeof(EFI_HII_FONT_PACKAGE_HDR) + (FontNameLen)*2;
    FontPkgHeader->GlyphBlockOffset = sizeof(EFI_HII_FONT_PACKAGE_HDR) + (FontNameLen*2); 
    FontPkgHeader->Cell = Cell;
    FontPkgHeader->FontStyle = EFI_HII_FONT_STYLE_NORMAL;
    CopyMem((FontPkgHeader->FontFamily), FontName, StrLen(FontName)*2 +2);


    pCurrent = (UINT8 *) (Package + 4 + FontPkgHeader->GlyphBlockOffset); 

    //CHAR 0...255
    BlockLength = FillNarrowGLYPH(pCurrent, NarrowGlyph, NrSizeInBytes, 1, NrStart, NrCharNum);
    pCurrent += BlockLength;
    NextChar = NrStart + NrCharNum;
    // EFI_HII_GIBT_GLYPHS_DEFAULT
    BlockLength = FillWideGLYPH(pCurrent, WideGlyph, SizeInBytes, NextChar, Start, CharNum);
    pCurrent += BlockLength;
    // END BLOCK
    EFI_GLYPH_GIBT_END_BLOCK* FontEndBlock = (EFI_GLYPH_GIBT_END_BLOCK*)(pCurrent);
    FontEndBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_END;

    // 
    // Add this simplified font package to a package list then install it. 
    // 
    {
        EFI_HANDLE gFontHiiHandle = HiiAddPackages ( 
                &gDMFontPackageListGuid ,
                NULL, 
                Package, 
                NULL 
                ); 
        (void)gFontHiiHandle;
        //ASSERT (gFontHiiHandle != NULL); 
    }
    FreePool (Package); 
    return EFI_SUCCESS;
}



    EFI_STATUS
UnLoadFont()
{
    EFI_STATUS                             Status = 0;
    EFI_HII_HANDLE    * handles = 0;
    UINT32 i = 0;
    handles = HiiGetHiiHandles(&gDMFontPackageListGuid); 
    if(handles == 0){
        return -1;
    }
    while(handles[i] != 0){
        HiiRemovePackages( handles[i]); 
        i++;
    }
    FreePool(handles);
    return Status;
}

#if 1
EFI_STATUS
EFIAPI
LoadFont(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
        )
{
    CHAR16 **Argv = 0;
    UINTN Argc = 0;
    EFI_SHELL_PARAMETERS_PROTOCOL *EfiShellParametersProtocol = NULL;
    EFI_SHELL_INTERFACE           *EfiShellInterface = NULL;
    EFI_STATUS                    Status;
    EFI_GUID mEfiShellParametersProtocolGuid = {0x752f3136, 0x4e16, 0x4fdc, {0xa2, 0x2a, 0xe5, 0xf4, 0x68, 0x12, 0xf4, 0xca}};
    EFI_GUID mEfiShellInterfaceGuid          = {0x47c7b223, 0xc42a, 0x11d2, {0x8e, 0x57, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    Status = SystemTable->BootServices->OpenProtocol(ImageHandle,
            &mEfiShellParametersProtocolGuid,
            (VOID **)&EfiShellParametersProtocol,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
    if (!EFI_ERROR(Status)) {
        Argc = EfiShellParametersProtocol->Argc; 
        Argv = EfiShellParametersProtocol->Argv; 
    }else{
        Status = SystemTable->BootServices->OpenProtocol(ImageHandle,
                &mEfiShellInterfaceGuid,
                (VOID **)&EfiShellInterface,
                ImageHandle,
                NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
        if (!EFI_ERROR(Status)) {
            Argc = EfiShellInterface->Argc; 
            Argv = EfiShellInterface->Argv; 
        }

    }

    if( Argc > 1){
        CHAR16* p = Argv[1];
        if(p[0] == '-' && ( p[1] == 'u' || p[1] == 'U')){
            UnLoadFont();
        }
    }else{
        EFI_HII_HANDLE    * handles = 0;
        handles = HiiGetHiiHandles(&gDMFontPackageListGuid); 
        if(handles == 0){
            extern CHAR16* FontName ;
            Status = CreateDMFontPkg( FontName, 16, 19,  
                    gUsStdNarrowGlyphData  , mNarrowFontSize, 32, mNarrowFontSize/sizeof(EFI_NARROW_GLYPH),
                    gSimpleFontWideGlyphData, gSimpleFontBytes, 0x4e00, gSimpleFontBytes/sizeof(EFI_WIDE_GLYPH)
                    );
        }else
            FreePool(handles);
    }
    return Status;
}
#else

EFI_STATUS
EFIAPI
LoadFont(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
        )
{
    UnLoadFont();
    return 0;
}
#endif
