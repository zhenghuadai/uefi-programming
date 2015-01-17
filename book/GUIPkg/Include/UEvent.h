#ifndef __UEVENT_HEADER__
#define __UEVENT_HEADER__
 

extern "C"{
#include <Protocol/HiiFont.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/SimpleTextInEx.h>
#include <Library/UefiHiiServicesLib.h>
#include <Protocol/SimplePointer.h>
#include <Library/PrintLib.h>
}

namespace UGUI{

#define UM_MOUSEFIRST                   0x0200
#define UM_MOUSEMOVE                    0x0200
#define UM_LBUTTONDOWN                  0x0201
#define UM_LBUTTONUP                    0x0202
#define UM_LBUTTONDBLCLK                0x0203
#define UM_RBUTTONDOWN                  0x0204
#define UM_RBUTTONUP                    0x0205
#define UM_RBUTTONDBLCLK                0x0206
#define UM_MBUTTONDOWN                  0x0207
#define UM_MBUTTONUP                    0x0208
#define UM_MBUTTONDBLCLK                0x0209

class UMouseEvent{
    public:
    UINT16 X, Y, Z, A;
};

typedef EFI_GRAPHICS_OUTPUT_BLT_PIXEL UEFI_PIXEL;

class UPixel{
    public:
        UPixel(UINT8 r, UINT8 g, UINT8 b){
            mPixel.Red = r; mPixel.Green = g; mPixel.Blue = b;
        }
    operator EFI_GRAPHICS_OUTPUT_BLT_PIXEL() const{ return mPixel;}
    private:
        EFI_GRAPHICS_OUTPUT_BLT_PIXEL mPixel;
};

class UIEvents
{
public:
    enum{UI_KEY = 0, UI_MOUSE = 1, UI_TIMER };
    static const UINTN MOUSE_RESOLUTION_PIXS = 10;
public:
    UIEvents(){}
    ~UIEvents(){ gBS->CloseEvent(mTimerEvent);mTimerEvent = 0;}
    UINTN Wait(){
        EFI_STATUS Status;
        UINTN            Index;
        Status = gBS->WaitForEvent(3, WaitArray, &Index);
        return Index;
    }
    UMouseEvent* GetMouse();
    UMouseEvent GetMousePos(){return mMouseCurPos;}
    UMouseEvent GetMouseLastPos(){return mMouseLastPos;}
    void SetMousePos(UINT16 x, UINT16 y){ mMouseCurPos.X =x; mMouseCurPos.Y =y;}
public:
    void SetTimer(UINT64 t= 1 * 1000 * 1000/24) {  gBS->SetTimer(mTimerEvent,TimerPeriodic , t);}
    void Init(){
        EFI_STATUS Status;
        memset(this, 0, sizeof(UIEvents));
        Status = gBS->CreateEvent(EVT_TIMER, TPL_APPLICATION, (EFI_EVENT_NOTIFY)NULL , (VOID*)NULL, &mTimerEvent);
        Status = gBS->SetTimer(mTimerEvent,TimerPeriodic , 1 * 1000 * 1000/24);    
        WaitArray[UI_TIMER] = mTimerEvent;
        Status = gBS->LocateProtocol(
            &gEfiSimplePointerProtocolGuid,
            NULL,
            (VOID**)&mMouse
            ); 
        Status = gBS->LocateProtocol(
            &gEfiSimpleTextInputExProtocolGuid,
            NULL,
            (VOID**)&mInputEx
            );
        if(mInputEx){
            WaitArray[UI_KEY] = mInputEx->WaitForKeyEx;
        }else{
            WaitArray[UI_KEY] = gST->ConIn->WaitForKey;
        }

        if(mMouse){
            WaitArray[UI_MOUSE] = mMouse->WaitForInput;
        }else{
            WaitArray[UI_MOUSE] = 0;
        }
        mMouseCurPos.X = 0;
        mMouseCurPos.Y = 0;
        mMouseLastPos.X = 0;
        mMouseLastPos.Y = 0;
    }

    EFI_STATUS ReadKey(EFI_KEY_DATA* key)
    {
        EFI_STATUS Status = ReadKey();
        if(key)
            *key = mKeyData;
        return Status;
    }

    EFI_STATUS ReadKey(EFI_INPUT_KEY* key)
    {
        EFI_STATUS Status = ReadKey();
        if(key)
            *key = mKeyData.Key;
        return Status;
    }

    void HandleTimer() { if(mTimerHandler)mTimerHandler(mTimerEvent, NULL);}
private:
    EFI_STATUS ReadKey()
    {
        EFI_STATUS Status;
        if(mInputEx)
        {
            Status = mInputEx->ReadKeyStrokeEx(mInputEx, &mKeyData);
        }else{
            Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &mKeyData.Key);
        }
        return Status;
    }
private:
    EFI_EVENT        WaitArray[16];

    EFI_EVENT        mTimerEvent;
    EFI_EVENT_NOTIFY mTimerHandler;

    EFI_SIMPLE_POINTER_PROTOCOL* mMouse;
    UMouseEvent            mMouseLastPos;
    UMouseEvent            mMouseCurPos;

    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL* mInputEx;
    EFI_KEY_DATA     mKeyData;
};

}
#endif
