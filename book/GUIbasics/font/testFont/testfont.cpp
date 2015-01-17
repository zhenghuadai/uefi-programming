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

extern "C"
{
#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HiiImage.h>
#include <Protocol/HiiFont.h>
}

EFI_STATUS
EFIAPI
UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
        )
{
    EFI_STATUS                           Status = 0;
    EFI_HII_FONT_PROTOCOL                *HiiFont = 0;
    EFI_GRAPHICS_OUTPUT_PROTOCOL         *gGraphicsOutput;
    EFI_IMAGE_OUTPUT                     gSystemFrameBuffer;
    EFI_IMAGE_OUTPUT*                    pSystemFrameBuffer  = &gSystemFrameBuffer;
    const CHAR16* Str = (const CHAR16*)L" 您好UEFI Font";
    struct PRIVATE_EFI_FONT_DISPLAY_INFO 
    {
        EFI_FONT_DISPLAY_INFO Font;
        CHAR16 Name[31];
    } SystemFont= {{
        {0x00,0x00,0xFF,0x00},// BGRA ForeColor
        {0xFF,0xFF,0xFF,0x00},// BackColor
        EFI_FONT_INFO_ANY_FONT,
        {EFI_HII_FONT_STYLE_NORMAL, 19, L'D'}
    }, {}};

    Status = gBS->LocateProtocol (&gEfiHiiFontProtocolGuid, NULL, (VOID **) &HiiFont);

    gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,NULL,(VOID **)&gGraphicsOutput);
    gSystemFrameBuffer.Width = (UINT16) gGraphicsOutput->Mode->Info->HorizontalResolution;
    gSystemFrameBuffer.Height = (UINT16) gGraphicsOutput->Mode->Info->VerticalResolution;
    gSystemFrameBuffer.Image.Screen = gGraphicsOutput;
    CopyMem(SystemFont.Font.FontInfo.FontName, (const CHAR16*)L"DMTestFont", StrLen((const CHAR16*)L"DMTestFont")*2 +2);
    Status = HiiFont->StringToImage (
            HiiFont,
            EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
            EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y |
            EFI_HII_IGNORE_LINE_BREAK | EFI_HII_OUT_FLAG_TRANSPARENT | EFI_HII_DIRECT_TO_SCREEN ,
            (CHAR16*)Str,
            (const EFI_FONT_DISPLAY_INFO*)(&SystemFont.Font),
            &pSystemFrameBuffer,
            (UINTN) 100,
            (UINTN) 100,                
            0,
            0,
            0 
            );
    return Status;
}
