#include <UGui.h>
extern "C"{
#include <Protocol/HiiFont.h>
#include <Library/HiiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/BlockIo.h>
#include <Library/MemoryAllocationLib.h>
#if (( defined USE_SIMPLE_STDIO)  && (! defined EFI_SHELL_FILE_PROTOCOL))
void* LocateFileProtocol(CHAR16* FileName );
#endif
}
#include "fb.h"

namespace UGUI{


/********************************************************************
 *
 ********************************************************************/
bool OptionList::testHit(UINT16 X, UINT16 Y)
{
    UINT16 bH = H();
    if(IsFocused()){
        bH = H() + mOptionNum * gDefaultFontHeight;
    }
    return ( (X>mX) && (X<(mX+mW)) &&(Y<(mY+bH)) &&(Y>mY) );
}

void OptionList::OnClick()
{
    if(IsFocused()){
        UMouseEvent mousePos = Brick::gUiEvent.GetMousePos();
        if((mH>0) && (mousePos.Y > mY+mH)){
            mCurOption = (mousePos.Y-mY-mH)/gDefaultFontHeight;
            OnEnter();
        }
    }else{
        SetFocus();
    }
}

void OptionList::DrawTo(void* pFrameBuffer, UINT32 ToScreen )
{
    EFI_STATUS Status;
    EFI_IMAGE_OUTPUT                     *pLocalFrameBuffer = (EFI_IMAGE_OUTPUT*) pFrameBuffer;
    UEFI_PIXEL        *Bitmap = 0;

    if(ToScreen ==EFI_HII_DIRECT_TO_SCREEN){
    }else{
        Bitmap = pLocalFrameBuffer->Image.Bitmap;
    }

    if(gHiiFont ==NULL) {
        return;
    }

    if(GetFocusState() == GOT_FOCUS){
        CopyFromFrameBuffer(Bitmap,pLocalFrameBuffer->Width,RestorBuffer, W(), X(), Y()+ H(), W(), H()*mOptionNum );
        SetFocusState(ON_FOCUS);
    }else if (GetFocusState() == LOSE_FOCUS){
        CopyToFrameBuffer(Bitmap,pLocalFrameBuffer->Width,RestorBuffer, W(), X(), Y()+ H(), W(), H()*mOptionNum );
        SetFocusState(NO_FOCUS);
    }

    if (GetFocusState() == NO_FOCUS){

    }else if (GetFocusState() == ON_FOCUS){
        UINT16 StartX = X();
        UINT16 StartY = Y();
        for(UINT16 i =0; i< mOptionNum; i++){
            StartY = StartY + gDefaultFontHeight;
            Status = gHiiFont->StringToImage (
                    gHiiFont,
                    EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
                    EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y |
                    EFI_HII_IGNORE_LINE_BREAK | ToScreen,
                    (CHAR16*)L"                         ",
                    (const EFI_FONT_DISPLAY_INFO*)( (i ==mCurOption)?NULL:&gDefaultFont),
                    &pLocalFrameBuffer,
                    (UINTN) StartX,
                    (UINTN) StartY,                
                    NULL,
                    NULL,
                    NULL
                    );
            //!draw textbox password            
            Status = gHiiFont->StringToImage (
                    gHiiFont,
                    EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
                    EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y |
                    EFI_HII_IGNORE_LINE_BREAK |  ToScreen,
                    (CHAR16*)Lists[i],
                    (const EFI_FONT_DISPLAY_INFO*)((i ==mCurOption)?NULL:&gDefaultFont),
                    &pLocalFrameBuffer,
                    (UINTN) StartX,
                    (UINTN) StartY,                
                    NULL,
                    NULL,
                    NULL
                    ); 

        }

    }
    StringBrick::DrawTo(pFrameBuffer, ToScreen );
}

/********************************************************************
 *
 ********************************************************************/
extern "C"
EFI_STATUS
EFIAPI
mUefiHiiServicesLibConstructor (
        IN EFI_HANDLE        ImageHandle,
        IN EFI_SYSTEM_TABLE  *SystemTable
        )
{
    EFI_STATUS Status;

    EFI_HANDLE  *ControllerHandle = NULL;    
    UINTN        NumHandles;
    EFI_HANDLE  HiiController;

    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiHiiFontProtocolGuid,
            NULL,
            &NumHandles,
            &ControllerHandle);

    if(EFI_ERROR(Status) ){
        if(Status == EFI_NOT_FOUND){            
        }
        gST -> ConOut-> OutputString(gST -> ConOut, (CHAR16*) L"\nOpen Hii Error\n");
        return (size_t) Status;
    }else if(NumHandles > 1){        
    }else{
    }

    HiiController = ControllerHandle[0];

    //
    // Retrieve the pointer to the UEFI HII String Protocol 
    //
    Status = gBS->HandleProtocol (HiiController, &gEfiHiiStringProtocolGuid,  (VOID **) &gHiiString);
    ASSERT_EFI_ERROR (Status);

    //
    // Retrieve the pointer to the UEFI HII Database Protocol 
    //
    Status = gBS->HandleProtocol (HiiController,&gEfiHiiDatabaseProtocolGuid,  (VOID **) &gHiiDatabase);
    ASSERT_EFI_ERROR (Status);

    //
    // Retrieve the pointer to the UEFI HII Config Routing Protocol 
    //
    Status = gBS->HandleProtocol (HiiController,&gEfiHiiConfigRoutingProtocolGuid,  (VOID **) &gHiiConfigRouting);
    ASSERT_EFI_ERROR (Status);

    //
    // Retrieve the pointer to the optional UEFI HII Font Protocol 
    //
    gBS->HandleProtocol (HiiController,&gEfiHiiFontProtocolGuid,  (VOID **) &gHiiFont);

    //
    // Retrieve the pointer to the optional UEFI HII Image Protocol 
    //
    gBS->HandleProtocol (HiiController,&gEfiHiiImageProtocolGuid,  (VOID **) &gHiiImage);
    return EFI_SUCCESS;
}

}//namespace UGUI{
/********************************************************************
 *
 ********************************************************************/
