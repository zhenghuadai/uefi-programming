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

inline UEFI_PIXEL Pixel(UINT8 r, UINT8 g, UINT8 b){ return UPixel(r,g,b);}
inline void setPixel(UEFI_PIXEL* p, UEFI_PIXEL color){*p = color;}


static EFI_IMAGE_OUTPUT              LocalFrameBuffer;
Brick*                              Brick::ActiveBrick = 0;
Brick*                              Brick::TabList = 0;
Brick*                              Brick::BrickList = 0;
Brick*                              Brick::CurWindow = 0;
void*                                Brick::FrameBuffer = (void*) &LocalFrameBuffer;
bool                                 Brick::IsRedrawFullScreen = false;
CHAR8*                               Brick::SystemLanguage = 0;
UFrameBuffer*                        Brick::pDisplay = 0;
RECT                                 Brick::ClientRect (0,0,0,0);
bool                                 Brick::ToDisplay = true;

UIEvents                              Brick::gUiEvent;
bool                                 Brick::Running = true;

EFI_IMAGE_OUTPUT                     SystemFrameBuffer;

EFI_GRAPHICS_OUTPUT_PROTOCOL         *gGraphicsOutput;
UINT16                               gDefaultFontWidth = 9;
UINT16                               gDefaultFontHeight = 20;


const int MAX_HOT_KEYS = 16;
int HotKeys::HotKeyNum = 0;
HotKeys::HotKey HotKeys::gHotKey[MAX_HOT_KEYS];
/*
 *
 * */
int HotKeys::RegisterHotKey(EFI_KEY_DATA Key, void(*handler)())
{
    if(HotKeyNum >= MAX_HOT_KEYS) return -1;
    gHotKey[HotKeyNum].Key = Key;
    gHotKey[HotKeyNum].handler = handler;
    HotKeyNum ++;
    return HotKeyNum - 1;
}

int UnRegisterHotKey(int hotKeyHandle)
{
    return 0;
}

int ExeHotKey(EFI_INPUT_KEY aKey)
{
    return 0;
}

int HotKeys::ExeHotKey(EFI_KEY_DATA aKey)
{
    int ret = NO;
    int i=0;
    for(i=0;i<HotKeyNum; i++){
        if(*(UINT32*)&aKey == *(UINT32*)&gHotKey[i].Key 
            && aKey.KeyState.KeyShiftState == gHotKey[i].Key.KeyState.KeyShiftState
            && aKey.KeyState.KeyToggleState == gHotKey[i].Key.KeyState.KeyToggleState
            ){
            ret = HOTKEY_QUIT_PROPAGATE;
            gHotKey[i].handler();
        }
    }
    return ret;
}
static struct{
    EFI_FONT_DISPLAY_INFO noname;
    CHAR16 name[2];
} iRealDontWanttoNameit = {{
    {0xFF,0x00,0x00,0x00},// Fore
    {0xEE,0xEE,0xFF,0x00},// Back
    EFI_FONT_INFO_ANY_FONT,
    {EFI_HII_FONT_STYLE_SHADOW, 0, L'楷'}
}, {L'体', 0}};

EFI_FONT_DISPLAY_INFO& gDefaultFont = iRealDontWanttoNameit.noname;



UMouseEvent* UIEvents::GetMouse(){ 
    EFI_STATUS Status;
    EFI_SIMPLE_POINTER_STATE     tMouseState;
    Status = mMouse->GetState(mMouse, &tMouseState);
    if(!EFI_ERROR(Status)){
        mMouseLastPos = mMouseCurPos;
        UINT64 mMouseResolutionX = mMouse->Mode->ResolutionX;
        UINT64 mMouseResolutionY = mMouse->Mode->ResolutionY;
        if(mMouseResolutionX != 0){
            mMouseCurPos.X = mMouseCurPos.X + (UINT16)(tMouseState.RelativeMovementX*MOUSE_RESOLUTION_PIXS/mMouseResolutionX);
            mMouseCurPos.Y = mMouseCurPos.Y + (UINT16)(tMouseState.RelativeMovementY*MOUSE_RESOLUTION_PIXS/mMouseResolutionY);
        }

        if(Brick::ClientRect.W >0){
            if(mMouseCurPos.X< Brick::ClientRect.X) mMouseCurPos.X = Brick::ClientRect.X;
            if(mMouseCurPos.X> Brick::ClientRect.X + Brick::ClientRect.W) mMouseCurPos.X = Brick::ClientRect.X + Brick::ClientRect.W -1;
            if(mMouseCurPos.Y< Brick::ClientRect.Y) mMouseCurPos.Y = Brick::ClientRect.Y;
            if(mMouseCurPos.Y> Brick::ClientRect.Y + Brick::ClientRect.H) mMouseCurPos.Y = Brick::ClientRect.Y + Brick::ClientRect.H -1;
        }else{
            EFI_IMAGE_OUTPUT                     *pFB = (EFI_IMAGE_OUTPUT*) Brick::FrameBuffer;
            if(mMouseCurPos.X > (UINT16) pFB->Width) mMouseCurPos.X = (UINT16) pFB->Width;
            if(mMouseCurPos.Y > (UINT16) pFB->Height) mMouseCurPos.Y = (UINT16) pFB->Height;
        }

        if(tMouseState.LeftButton == 1) 
            mMouseCurPos.A = UM_LBUTTONDOWN;
        else if (tMouseState.RightButton == 1) 
            mMouseCurPos.A = UM_RBUTTONDOWN;
        else 
            mMouseCurPos.A = 0;
        return &mMouseCurPos;
    }else 
        return NULL;
}

EFI_STATUS Brick::Initialize()
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

    Status = gBS->HandleProtocol (HiiController,&gEfiHiiDatabaseProtocolGuid, (VOID **) &gHiiDatabase);
    Status = gBS->HandleProtocol (HiiController,&gEfiHiiFontProtocolGuid, (VOID **) &gHiiFont);

    gBS->LocateProtocol(
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        (VOID **)&gGraphicsOutput);
    
    Brick::pDisplay = UFrameBuffer::Create(0);
    
    SystemFrameBuffer.Width = (UINT16) gGraphicsOutput->Mode->Info->HorizontalResolution;
    SystemFrameBuffer.Height = (UINT16) gGraphicsOutput->Mode->Info->VerticalResolution;
    SystemFrameBuffer.Image.Screen = gGraphicsOutput;

    LocalFrameBuffer.Width = (UINT16) gGraphicsOutput->Mode->Info->HorizontalResolution;
    LocalFrameBuffer.Height = (UINT16) gGraphicsOutput->Mode->Info->VerticalResolution;
    gBS->AllocatePool(EfiRuntimeServicesData, LocalFrameBuffer.Width *LocalFrameBuffer.Height * sizeof(UEFI_PIXEL), (VOID**)&(LocalFrameBuffer.Image.Bitmap));
    
    pUiEvent->Init();
    pUiEvent->SetTimer();

#if (( defined USE_SIMPLE_STDIO)  && (! defined EFI_SHELL_FILE_PROTOCOL))
    LocateFileProtocol(0);
#endif
    return (size_t) Status;
}

void Brick::Finish()
{
    SAFE_FREE(SystemLanguage);
    SAFE_DELETE(pDisplay);
}

void Brick::OnDraw()
{
    EFI_IMAGE_OUTPUT* pFB = &SystemFrameBuffer;
    DrawTo(pFB, (UINT32)EFI_HII_DIRECT_TO_SCREEN);
}

void Brick::AddtoBrickList()
{    
    if(BrickList == 0){
        BrickList = this;
        BrickList->NextinBrickList = this;
        if(CurWindow){            
            ((Window*)CurWindow)->SetChildHeader(this);
        }else{
            //ASSERT(0);
        }
    }else{
        this->NextinBrickList = BrickList->NextinBrickList;
        BrickList->NextinBrickList = this;
        BrickList = this;
    }
}
void Brick::AddtoTabList()
{
    if(TabList == 0){
        TabList = this;
        TabList->NextinTabList = this;
        if(CurWindow){
            ((Window*)CurWindow)->SetTabHeader(this);
        }else{
            //ASEERT(0);
        }
    }else{
        this->NextinTabList = TabList->NextinTabList;
        TabList->NextinTabList = this;
        TabList = this;
    }
}

void Brick:: OnTab()
{
    if(Brick::ActiveBrick && Brick::ActiveBrick->GetNextInTabList())
        Brick::ActiveBrick->GetNextInTabList()->SetFocus();
    else if(Brick::TabList)
        Brick::TabList->SetFocus();
}

void Brick::  DispatchMouseMessage(UMouseEvent MouseState)
{
    if(Brick::TabList == NULL) return;
    Brick* ListHeader = Brick::TabList;
    Brick* O = ListHeader;
    do{
        if(O->testHit(MouseState.X, MouseState.Y)){
            if(MouseState.A == UM_LBUTTONDOWN)
                O->OnClick();
            break;
        }else{
            O = O->NextinTabList;
        }
    }while(O != ListHeader);

}

void Brick::UpdateScreen(void* pFrameBuffer )
{
    EFI_IMAGE_OUTPUT                     *pFB = (EFI_IMAGE_OUTPUT*) pFrameBuffer;
    UEFI_PIXEL        *Bitmap = 0; 
    Bitmap = pFB->Image.Bitmap;
    if((void*)Bitmap == (void*)gGraphicsOutput){
        return ;
    }
    if(pDisplay){
        pDisplay->UpdateScreen(Bitmap);
    }
    //gGraphicsOutput ->Blt(gGraphicsOutput, Bitmap,  
    //    EfiBltBufferToVideo,
    //    0, 0, 0 , 0, pFB->Width, pFB->Height, 0);
}

void Brick::UpdateFullScreen(void* pFrameBuffer )
{
    EFI_IMAGE_OUTPUT                     *pFB = (EFI_IMAGE_OUTPUT*) pFrameBuffer;
    UEFI_PIXEL        *Bitmap = 0; 
    Bitmap = pFB->Image.Bitmap;
    if((void*)Bitmap == (void*)gGraphicsOutput){
        return ;
    }
    if(pDisplay){
        pDisplay->UpdateFullScreen(Bitmap);
    }
}

class TabNextHelper
{
    public:
        static Brick* Next(Brick* o){return Brick::GetNextInTabList(o);}
};

class BrickNextHelper
{
    public:
        static Brick* Next(Brick* o){return Brick::GetNextInBrickList(o);}
};

const bool CHECK_CHANGE_BEFORE_REDRAW = true;
const bool DONOT_CHECK_CHANGE_BEFORE_REDRAW =false;
template<class T, bool CheckChanged>
void ReDrawList(Brick* Header)
{
    Brick* ListHeader = Header;
    Brick* O = ListHeader;
    do{
        if(CheckChanged ) {
            if(O->IsChanged())O->reDraw();
        }else{
            O->reDraw();
        }
        O = T::Next(O);
    }while(O != ListHeader);
}

void Brick::Run()
{
    EFI_KEY_DATA CurKeyData = {0, 0};
    EFI_INPUT_KEY& CurChar = CurKeyData.Key;
    UINTN Index = 0;
    Brick::UpdateFullScreen();        
    Brick::DrawMouse();
    Brick::ReDrawScreen();
    WPARAM wParam = 0;
    LPARAM lParam = 0;
    while(Brick::Running == true){
        //!
        Brick::EraseMouse();

        ((Window*)(Brick::CurWindow))->DrawTo(Brick::FrameBuffer, 0);

        Brick::DrawMouse();
        Brick::UpdateScreen();

        Index = pUiEvent->Wait();
        if(Index == UIEvents::UI_TIMER){            
            pUiEvent->HandleTimer();
        }else if(Index == UIEvents::UI_MOUSE){
            Brick::DispatchMouseMessage( *(pUiEvent->GetMouse()));
            // UMouseEvent tMouseEvent = pUiEvent -> GetMousePos();
            // *(UINT32*)&wParam = *(WPARAM*)&tMouseEvent;
            // *(UINT32*)&lParam = *(UINT32*)&tMouseEvent.Z;
        }else    {
            EFI_STATUS Status;
            Status = pUiEvent->ReadKey(&CurKeyData);
            *( EFI_INPUT_KEY*)&wParam = CurChar ;
            if(HotKeys::ExeHotKey(CurKeyData) == HotKeys::HOTKEY_QUIT_PROPAGATE){

            }else if(CurChar.UnicodeChar ==0){ 
                if(Brick::ActiveBrick)
                    Brick::ActiveBrick->OnExtKey(CurChar.ScanCode);
            }else if(CurChar.UnicodeChar == CHAR_CR){      
                if(Brick::ActiveBrick)
                    Brick::ActiveBrick->OnEnter();               
            }else if(CurChar.UnicodeChar == CHAR_TAB){ 
                if(Brick::ActiveBrick)
                    Brick::ActiveBrick->OnTab();
            }else{
                if(Brick::ActiveBrick)
                    Brick::ActiveBrick->OnKey(CurChar.UnicodeChar);
            }

        }
        if(((Window*)(Brick::CurWindow))->mEventHandler)
            ((Window*)(Brick::CurWindow))->mEventHandler((UINT32)Index,wParam, lParam );
    }// while(Running)
}
/************************************************************
 * Mouse 
 *************************************************************/
#define MOUSE_BITMAP_BASEWIDTH  (sizeof(CommonMouseBitmap[0])/sizeof(CommonMouseBitmap[0][0])) //0x0c 
#define MOUSE_BITMAP_BASEHEIGHT (sizeof(CommonMouseBitmap)/sizeof(CommonMouseBitmap[0])) //0x12 
#define HH {0x10,0x10,0x10,0x00}
#define __ {0xFF,0xFF,0xFF,0x00}
#define _  {0x00,0x00,0x00,0x00}
static UEFI_PIXEL CommonMouseBitmap[][0x0c]={
    /*****{00,01,02,03,04,05,06,07,08,09,0a,0b},*****/
    /*00*/{HH,_ ,_ ,_ ,_ ,_ ,_ ,_ ,_ ,_ ,_ ,_ },/*00*/
    /*01*/{HH,HH,_ ,_ ,_ ,_ ,_ ,_ ,_ ,_ ,_ ,_ },/*01*/
    /*02*/{HH,__,HH,_ ,_ ,_ ,_ ,_ ,_ ,_ ,_ ,_ },/*02*/
    /*03*/{HH,__,__,HH,_ ,_ ,_ ,_ ,_ ,_ ,_ ,_ },/*03*/
    /*04*/{HH,__,__,__,HH,_ ,_ ,_ ,_ ,_ ,_ ,_ },/*04*/
    /*05*/{HH,__,__,__,__,HH,_ ,_ ,_ ,_ ,_ ,_ },/*05*/
    /*06*/{HH,__,__,__,__,__,HH,_ ,_ ,_ ,_ ,_ },/*06*/
    /*07*/{HH,__,__,__,__,__,__,HH,_ ,_ ,_ ,_ },/*07*/
    /*08*/{HH,__,__,__,__,__,__,__,HH,_ ,_ ,_ },/*08*/
    /*09*/{HH,__,__,__,__,__,__,__,__,HH,_ ,_ },/*09*/
    /*0a*/{HH,__,__,__,__,__,__,__,__,__,HH,_ },/*0a*/
    /*0b*/{HH,__,__,__,__,__,__,_ ,_ ,_ ,_ ,HH},/*0b*/
    /*0c*/{HH,__,__,__,__,__,__,HH,HH,HH,HH,HH},/*0c*/
    /*0d*/{HH,__,__,_ ,HH,__,__,HH,_ ,_ ,_ ,_ },/*0d*/
    /*0e*/{HH,__,_ ,HH,_ ,HH,__,__,HH,_ ,_ ,_ },/*0e*/
    /*0f*/{HH,_ ,HH,_ ,_ ,HH,__,__,HH,_ ,_ ,_ },/*0f*/
    /*10*/{HH,HH,_ ,_ ,_ ,_ ,HH,__,_ ,HH,_ ,_ },/*10*/
    /*11*/{_ ,_ ,_ ,_ ,_ ,_ ,_ ,HH,HH,_ ,_ ,_ } /*11*/
    /*****{00,01,02,03,04,05,06,07,08,09,0a,0b},*****/
};
static UEFI_PIXEL EraseMouseBitmap [MOUSE_BITMAP_BASEWIDTH*MOUSE_BITMAP_BASEHEIGHT];

#undef HH 
#undef __
#undef _ 
UEFI_PIXEL* MouseBitmap = (UEFI_PIXEL*)CommonMouseBitmap;

static UMouseEvent LastDrawMousePos;
void Brick::DrawMouse()
{
    UMouseEvent CurMousePos = pUiEvent->GetMousePos();
    LastDrawMousePos = CurMousePos;
    UFrameBuffer::CopyFromFrameBuffer(Brick::LocalBitMap(),Brick::FBWidth(),
            EraseMouseBitmap, 
            MOUSE_BITMAP_BASEWIDTH, 
            CurMousePos.X, CurMousePos.Y, 
            MOUSE_BITMAP_BASEWIDTH,MOUSE_BITMAP_BASEHEIGHT);
    UFrameBuffer::MaskedCopyToFrameBuffer(Brick::LocalBitMap(),Brick::FBWidth(),
            MouseBitmap, MOUSE_BITMAP_BASEWIDTH, 
            0, 0,
            CurMousePos.X, CurMousePos.Y, 
            MOUSE_BITMAP_BASEWIDTH,MOUSE_BITMAP_BASEHEIGHT);
}

void Brick::EraseMouse()
{
    UMouseEvent LastMousePos = LastDrawMousePos;
    UFrameBuffer::CopyToFrameBuffer(Brick::LocalBitMap(),Brick::FBWidth(),
            EraseMouseBitmap, MOUSE_BITMAP_BASEWIDTH,
            0,0,
            LastMousePos.X, LastMousePos.Y, 
            MOUSE_BITMAP_BASEWIDTH,MOUSE_BITMAP_BASEHEIGHT);
}

/************************************************************
 *
 *************************************************************/
void Label::DrawTo(void* pFrameBuffer, UINT32 ToScreen )
{
    EFI_STATUS Status;
    EFI_IMAGE_OUTPUT                     *pFB = (EFI_IMAGE_OUTPUT*) pFrameBuffer;
    if(gHiiFont ==NULL) {
        return;
    }

    if(mHiiHandle && mStringId !=INVALID_STRID ){
        Status = gHiiFont->StringIdToImage ( gHiiFont,
                EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
                EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y |
                EFI_HII_IGNORE_LINE_BREAK | EFI_HII_OUT_FLAG_TRANSPARENT | ToScreen,
                mHiiHandle,
                mStringId,
                (CHAR8*)Brick::SystemLanguage,
                (const EFI_FONT_DISPLAY_INFO*)&gDefaultFont,
                &pFB,
                (UINTN) X(), (UINTN) Y(),
                NULL, NULL, NULL
                );
    }else{
        Status = gHiiFont->StringToImage ( gHiiFont,
                EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
                EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y |
                EFI_HII_IGNORE_LINE_BREAK | EFI_HII_OUT_FLAG_TRANSPARENT | ToScreen,
                (CHAR16*)GetDisplayStr(),
                (const EFI_FONT_DISPLAY_INFO*)(&gDefaultFont),
                &pFB,
                (UINTN) X(),  (UINTN) Y(),                
                NULL, NULL, NULL
                );
    }
}

void StringBrick::DrawTo(void* pFrameBuffer, UINT32 ToScreen)
{
    EFI_STATUS Status;
    EFI_IMAGE_OUTPUT                     *pFB = (EFI_IMAGE_OUTPUT*) pFrameBuffer;
    if(gHiiFont ==NULL) {
        return;
    }
    // 覆盖原来的文字
    // UEFI_PIXEL WhitePix = {0xEE,0xEE,0xEE,0};
    // Clear(WhitePix);
    //! 新文字
    Status = gHiiFont->StringToImage (
            gHiiFont,
            EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
            EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y |
            EFI_HII_IGNORE_LINE_BREAK |  ToScreen,
            (CHAR16*)GetDisplayStr(),
            (const EFI_FONT_DISPLAY_INFO*)(IsFocused()?NULL:&gDefaultFont),
            &pFB,
            (UINTN) X(), (UINTN) Y(),                
            NULL, NULL, NULL
            ); 
}

/********************************************************************
 *
 ********************************************************************/
void Button::OnClick()
{
    OnEnter();
}

void Brick::VLine( UEFI_PIXEL* pS, int len, UEFI_PIXEL color, int stride)
{
    for(int i = 0; i < len; i ++){
        setPixel(&pS[i*stride], color);
    }
}

void Brick::Rectangle( UEFI_PIXEL* pS, int w, int h, UEFI_PIXEL color, int stride)
{
    for(int row = 0; row < h; row ++){
        pS += (h * stride);
        for(int i = 0; i < w; i ++){
            setPixel(&pS[i], color);
        }
    }
}

void Brick::HLine( UEFI_PIXEL* pS, int len, UEFI_PIXEL color)
{
    for(int i = 0; i < len; i ++){
        setPixel(&pS[i], color);
    }
}
void Brick::HDotLine( UEFI_PIXEL* pS, int len, UEFI_PIXEL color)
{
    UEFI_PIXEL empty = Pixel(0,0,0);
    for(int i = 0; i < len; i ++){
        setPixel(pS + i, ((i&1) == 0)?color:empty);
    }
}

void Brick::Clear(UEFI_PIXEL color, void* pFrameBuffer) 
{
    EFI_IMAGE_OUTPUT                     *pFB = (EFI_IMAGE_OUTPUT*) pFrameBuffer;
    UEFI_PIXEL        *Bitmap = 0;
    Bitmap = pFB->Image.Bitmap;
    if(Bitmap == 0) return;
    UINT16 FrameWidth = pFB->Width;
    Rectangle( Bitmap + Y()* FrameWidth + X(), W(), H(), color, FrameWidth);
}

void Brick::DrawFrame(void* pFrameBuffer, UINT32 ToScreen, 
        UEFI_PIXEL LeftC,
        UEFI_PIXEL RightC,
        UEFI_PIXEL TopC,
        UEFI_PIXEL BottomC)
{

    //EFI_STATUS Status;
    EFI_IMAGE_OUTPUT                     *pFB = (EFI_IMAGE_OUTPUT*) pFrameBuffer;
    UEFI_PIXEL        *Bitmap = 0;
    if(gHiiFont ==NULL) {
        return;
    }

    if(ToScreen ==EFI_HII_DIRECT_TO_SCREEN){
    }else{
        Bitmap = pFB->Image.Bitmap;
    }

    if(Bitmap == 0) return;
    //! Draw Frame
    UINT16 FrameWidth = pFB->Width;

    HLine( Bitmap + Y()* FrameWidth + X(), W(), TopC);
    HLine( Bitmap + Y()* FrameWidth + X() + (H() * FrameWidth) , W(), BottomC);

    if(IsFocused())    {
        HDotLine( Bitmap + Y()* FrameWidth + X()  + FrameWidth, W(), TopC);
        HDotLine( Bitmap + Y()* FrameWidth + X() + (H() * FrameWidth) -FrameWidth , W(), BottomC);
    } else{
        HLine( Bitmap + Y()* FrameWidth + X()  + FrameWidth, W(), TopC);
        HLine( Bitmap + Y()* FrameWidth + X() + (H() * FrameWidth) -FrameWidth , W(), BottomC);
    }

    VLine( Bitmap + Y()* FrameWidth + X(), H(), TopC, FrameWidth);
    VLine( Bitmap + Y()* FrameWidth + X() + 1, H(), TopC, FrameWidth);
    VLine( Bitmap + Y()* FrameWidth + X() + W(), H(), BottomC, FrameWidth);
    VLine( Bitmap + Y()* FrameWidth + X() + W()-1, H(), BottomC, FrameWidth);
    
}

void Button::DrawTo(void* pFrameBuffer, UINT32 ToScreen)
{
    UEFI_PIXEL WhitePix = {0xEE,0xEE,0xEE,0};
    UEFI_PIXEL BlackPix = {0x00,0x00,0x00,0};
    //! BackGround
    if(mbackGround){

        if(IsFocused())            
            UFrameBuffer::MaskedDrawtoFrameBuffer(((EFI_IMAGE_OUTPUT*) pFrameBuffer)->Image.Bitmap, ((EFI_IMAGE_OUTPUT*) pFrameBuffer)->Width, 
                    mbackGround, W() ,
                    0, 0,
                    X()+2, Y()+ 2,  
                    W(), H(), 
                    30);
        else
            UFrameBuffer::MaskedDrawtoFrameBuffer(((EFI_IMAGE_OUTPUT*) pFrameBuffer)->Image.Bitmap, ((EFI_IMAGE_OUTPUT*) pFrameBuffer)->Width, 
                    mbackGround, W() ,
                    0, 0,
                    X()+2, Y()+ 2,  
                    W(), H());
    }

    if((bShowFrame == TRUE))
        DrawFrame(pFrameBuffer, ToScreen, WhitePix,BlackPix,WhitePix,BlackPix);

    //! Text
    mX += (W() - gDefaultFontWidth*5)/2;
    Label::DrawTo(pFrameBuffer,ToScreen);
    mX -= (W() - gDefaultFontWidth*5)/2;
}

void Button::OnEnter()
{
    UEFI_PIXEL WhitePix = {0xEE,0xEE,0xEE,0};
    UEFI_PIXEL BlackPix = {0x00,0x00,0x00,0};
    DrawFrame(Brick::FrameBuffer, 0, BlackPix,WhitePix, BlackPix, WhitePix);
    UpdateScreen();
    gBS->Stall(50);
    DrawFrame(Brick::FrameBuffer, 0, WhitePix,BlackPix,WhitePix,BlackPix);
    UpdateScreen();
    gBS->Stall(50);
}

void Switcher::DrawTo(void* pFrameBuffer, UINT32 ToScreen)
{
    bmp_t* tbmp;
    if(mstate == SWITCH_ON)
        tbmp = mbmpon;
    else 
        tbmp = mbmpoff;
    if(tbmp){

        if(IsFocused())            
            UFrameBuffer::MaskedDrawtoFrameBuffer(((EFI_IMAGE_OUTPUT*) pFrameBuffer)->Image.Bitmap, ((EFI_IMAGE_OUTPUT*) pFrameBuffer)->Width, 
                    tbmp, W() ,
                    0, 0,
                    X()+2, Y()+ 2,  
                    W(), H());
        else
            UFrameBuffer::MaskedDrawtoFrameBuffer(((EFI_IMAGE_OUTPUT*) pFrameBuffer)->Image.Bitmap, ((EFI_IMAGE_OUTPUT*) pFrameBuffer)->Width, 
                    tbmp, W() ,
                    0, 0,
                    X()+2, Y()+ 2,  
                    W(), H());
    }
    //if((bShowFrame == TRUE)){
    //    UEFI_PIXEL WhitePix = {0xEE,0xEE,0xEE,0};
    //    UEFI_PIXEL BlackPix = {0x00,0x00,0x00,0};
    //    DrawFrame(pFrameBuffer, ToScreen, WhitePix,BlackPix,WhitePix,BlackPix);
    //}
}

void Switcher::OnEnter()
{
    if(mstate == SWITCH_ON)
        mstate = SWITCH_OFF;
    else 
        mstate = SWITCH_ON;
}

void ProgressBar::DrawTo(void* pFrameBuffer, UINT32 ToScreen)
{
    EFI_IMAGE_OUTPUT                     *pFB = (EFI_IMAGE_OUTPUT*) pFrameBuffer;
    UEFI_PIXEL        *Bitmap = 0;
    UEFI_PIXEL        forground =  {255,0,0,0};
    UEFI_PIXEL        background = {128,128,0,0};

    if(ToScreen ==EFI_HII_DIRECT_TO_SCREEN){
    }else{
        int cur = (mcur>mmax?W():(W() * mcur )/ mmax);
        Bitmap = pFB->Image.Bitmap;
        UFrameBuffer::SetToFrameBuffer(Bitmap,(UINT16)pFB->Width, X(), Y(), W(), H(),forground);
        UFrameBuffer::SetToFrameBuffer(Bitmap,(UINT16)pFB->Width, X(), Y(), (UINT16)cur, H(),background);

    }
}

void Window::DrawTo(void* pFrameBuffer, UINT32 ToScreen )
{
    if(Brick::IsRedrawFullScreen){
        if(((Window*)(Brick::CurWindow))->mbgBmp){
            EFI_IMAGE_OUTPUT* pfb= (EFI_IMAGE_OUTPUT*) Brick::FrameBuffer;
            UFrameBuffer::DrawtoFrameBuffer(pfb->Image.Bitmap, pfb->Width, 
                    ((Window*)(Brick::CurWindow))->mbgBmp, 0 ,
                    0, 0,  
                    0, 0,
                    pfb->Width, pfb->Height);
        }
        if(Brick::BrickList){
            ReDrawList<BrickNextHelper, DONOT_CHECK_CHANGE_BEFORE_REDRAW >(Brick::BrickList);
        }
    }else if(Brick::TabList){
        ReDrawList<TabNextHelper, CHECK_CHANGE_BEFORE_REDRAW>(Brick::TabList);
    }
    Brick::IsRedrawFullScreen = false;
}

}//namespace UGUI{
/********************************************************************
 *
 ********************************************************************/
