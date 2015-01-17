#ifndef __UGUI_HEADER__
#define __UGUI_HEADER__
 
extern "C"{
#include <Uefi.h>
#include <Base.h> 
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiFont.h>
#include <Protocol/HiiString.h>
#include <Library/UefiHiiServicesLib.h>
#include <Protocol/SimplePointer.h>
#include <Library/PrintLib.h>
}

#include "bmp.h" 
#include "StdUefi.h"
#include "UEvent.h"
#define SAFE_FREE(s) { if(s){ free(s); s = 0;}}
#define SAFE_DELETE(s) { if(s){ delete (s); s = 0;}}
#define UCTOR
#define UDTOR
#define UVIRT

namespace UGUI{
#define CHAR_BS CHAR_BACKSPACE 
#define CHAR_CR CHAR_CARRIAGE_RETURN 
class UFrameBuffer;

typedef struct RECT{
    UINT16 X, Y, W, H;
    RECT(UINT16 x, UINT16 y, UINT16 w, UINT16 h):X(x),Y(y),W(w),H(h){}
}RECT;
class Brick
{
public:
    typedef int (*EventHandler)(Brick* O, void* context);
public:
    Brick():mX(0),mY(0),mW(0),mH(0),Status(0),NextinTabList(0),NextinBrickList(0){AddtoBrickList(); }
    Brick(bool b):mX(0),mY(0),mW(0),mH(0),Status(0),NextinTabList(0),NextinBrickList(0){BrickList = 0; TabList = 0;} //! For Create Window
    template<typename T>Brick(T x, T y, T w, T h):Status(0),NextinTabList(0),NextinBrickList(0){ Pos(x, y); Size(w, h); AddtoBrickList();}
    virtual ~Brick(){};
public:
    template<typename T>void Pos(T x, T y){mX = (UINT16) x; mY = (UINT16) y;}
    template<typename T>void Size(T w, T h){mW = (UINT16) w; mH = (UINT16) h;}
    UINT16 X(){return mX;}
    UINT16 Y(){return mY;}
    UINT16 W(){return mW;}
    UINT16 H(){return mH;}
public:
    virtual void OnEnter()   {};
    virtual void OnTab();
    virtual void OnKey(CHAR16 CurChar)     {};
    virtual void OnExtKey(UINT16 CurChar)     {};
    virtual void LoseFocus(){Focus_Status = (UINT32)LOSE_FOCUS;};
    virtual void SetFocus(){
        Focus_Status = (UINT32)GOT_FOCUS; 
        if(ActiveBrick && ActiveBrick!=this) ActiveBrick->LoseFocus();
        ActiveBrick = this;
    };
    virtual bool IsFocused(){ return ((Focus_Status == (UINT32)ON_FOCUS)||(Focus_Status == (UINT32)GOT_FOCUS));}
    //!Draw to local buffer
    virtual void DrawTo(void* pFrameBuffer = Brick::FrameBuffer, UINT32 ToScreen = 0)    {}; 
    //! Draw to Screen
    virtual void OnDraw();    
    virtual void OnClick(){}    
    virtual bool testHit(UINT16 X, UINT16 Y){return ( (X>mX) && (X<(mX+mW)) &&(Y<(mY+mH)) &&(Y>mY) );}
    
public:
    void DrawFrame(void* pFrameBuffer, UINT32 ToScreen, 
                       EFI_GRAPHICS_OUTPUT_BLT_PIXEL LeftColor,
                       EFI_GRAPHICS_OUTPUT_BLT_PIXEL RightColor,
                       EFI_GRAPHICS_OUTPUT_BLT_PIXEL TopColor,
                       EFI_GRAPHICS_OUTPUT_BLT_PIXEL BottomColor);
    void reDraw(){DrawTo(FrameBuffer,0);}
    void AddtoTabList();
    Brick*& GetNextInBrickList(){return NextinBrickList;}
    static Brick*& GetNextInBrickList(Brick* o){return o->GetNextInBrickList();}
    Brick*& GetNextInTabList(){return NextinTabList;}
    static Brick*& GetNextInTabList(Brick* o){return o->GetNextInTabList();}
    UINTN GetFocusState(){return Focus_Status & FOCUS_MASK;}    
    void  SetFocusState(UINTN FocusState){Focus_Status = (UINT32)FocusState;}
    bool IsEnabled(){return  (UINTN)Activity != WG_DISABLE;}
    void Enable(){Activity = WG_ENABLE;}
    void Disable(){Activity = WG_DISABLE;}
    bool IsChanged(){return mIsChanged == 1;}
private:
    void AddtoBrickList();
    void MarkAsChanged(){ mIsChanged = 1;}
    void MarkAsUnchanged(){mIsChanged = 0;}
    void Clear(UEFI_PIXEL color, void* pFrameBuffer = Brick::FrameBuffer);
public:
    static void VLine( UEFI_PIXEL* pS, int len, UEFI_PIXEL color, int stride);
    static void HDotLine( UEFI_PIXEL* pS, int len, UEFI_PIXEL color);
    static void HLine( UEFI_PIXEL* pS, int len, UEFI_PIXEL color);
    static void Rectangle( UEFI_PIXEL* pS, int w, int h, UEFI_PIXEL color, int stride);
    static  void DispatchMouseMessage(UMouseEvent MouseState);
    static  void UpdateScreen(void* pFrameBuffer = Brick::FrameBuffer);
    static  void UpdateFullScreen(void* pFrameBuffer = Brick::FrameBuffer);
    static  void SetFrameBuffer(void* p){FrameBuffer = p;}
    static  void Run();
protected:
    UINT16 mX;
    UINT16 mY;
    UINT16 mW;
    UINT16 mH;
    union{
        UINT32  Status;
        struct{
            unsigned int Focus_Status :3;
            unsigned int Visibility   :1;
            unsigned int Activity     :1;  
            unsigned int mIsChanged    :1;  
        };
    };
public:
    Brick* NextinTabList;

    Brick* NextinBrickList;
    enum{NO_FOCUS=0x0, ON_FOCUS=0x1, GOT_FOCUS=0x3, LOSE_FOCUS=0x2, FOCUS_MASK=(0x3)};
    enum{WG_ENABLE=0x0, WG_DISABLE=0x1};
public:
    static Brick* ActiveBrick;    
    static Brick* TabList;
    static Brick* BrickList;
    static Brick* CurWindow;

    static bool    IsRedrawFullScreen;
    static void*   FrameBuffer;    //!EFI_IMAGE_OUTPUT*
    static UFrameBuffer*  pDisplay;
    static CHAR8*  SystemLanguage;
    static RECT    ClientRect;     //!The active GUI region. The object in this region can be updated to window automatically.
    static bool    ToDisplay;
    static bool    Running;  
    static UIEvents gUiEvent;
#define pUiEvent (&Brick::gUiEvent)
public:
    static void    QuitGui(){ Running = false;};
    static void    DrawMouse();
    static void    EraseMouse();
    static void    ReDrawScreen(){IsRedrawFullScreen = true;}
    static void    SetSystemLanguage(CHAR8* lang){ if(SystemLanguage)free(SystemLanguage); SystemLanguage = lang;}
    static void    SetClientRect(UINT16 x, UINT16 y,UINT16 w, UINT16 h){ClientRect.X=x; ClientRect.Y=y; ClientRect.W=w; ClientRect.H=h;}
    static void    Finish();
    static EFI_STATUS    Initialize();
    static EFI_GRAPHICS_OUTPUT_BLT_PIXEL* LocalBitMap(){ return ((EFI_IMAGE_OUTPUT*) FrameBuffer)->Image.Bitmap;}
    static UINT16 FBWidth(){ return ((EFI_IMAGE_OUTPUT*) FrameBuffer)->Width;}
    static UINT16 FBHeight(){ return ((EFI_IMAGE_OUTPUT*) FrameBuffer)->Height;}
};

typedef UINTN WPARAM;
typedef UINTN LPARAM;
class Window: public Brick
{
public:
    typedef int (*WndEventHandler)(UINT32 msgid, WPARAM wParam, LPARAM lParam);
public:
    Window():mBrickList(0), mTabList(0),Brick(false), mEventHandler(0),mbgBmp(0){Brick::CurWindow = this;}
    void SetChildHeader(Brick* o){mBrickList = o;}
    void SetTabHeader(Brick* o){mTabList = o;}
    void SetEventHandler(WndEventHandler ehd){mEventHandler = ehd;}    
    void Activate(){Brick::CurWindow = this;} 
    void Show(){
        if(!IsEnabled()) return;
        if(Brick::ActiveBrick)
            Brick::ActiveBrick->LoseFocus();
        Brick::CurWindow = this; 
        Brick::BrickList = mBrickList;
        Brick::TabList = mTabList;
        Brick::ActiveBrick = mTabList;
        if(Brick::ActiveBrick)
            Brick::ActiveBrick->SetFocus();
        Brick::ReDrawScreen(); 
    }
    ~Window(){
        if(mBrickList == NULL) return;
        Brick* ListHeader = mBrickList;
        Brick* O = ListHeader;
        do{
            Brick * d = O;            
            O = O->NextinBrickList;            
            delete d;
        }while(O != ListHeader);
        mTabList =0;
    }
public:
    virtual void DrawTo(void* pFrameBuffer = Brick::FrameBuffer, UINT32 ToScreen = 0) ; 
private:
    Brick* mBrickList;
    Brick* mTabList;
public:
    WndEventHandler mEventHandler;
    bmp_t*  mbgBmp;
};

class StringBrick:public Brick
{
public UCTOR:
    StringBrick(){
        int length = 32;
        mStr = new CHAR16[length +1]; 
        mStr[0] = 0; mLen = 0; 
        mCapacity = length + 1;
    }
    StringBrick(int length ){
        mStr = new CHAR16[length +1]; 
        mStr[0] = 0; mLen = 0; 
        mCapacity = length + 1;
    }
    ~StringBrick(){delete mStr;}
public:
    virtual void OnEnter(){OnTab();}
    virtual CHAR16* GetStr(){return mStr;}
    virtual CHAR16* SetStr(CHAR16 * Str) {          
        UINTN tLen = StrLen(Str); 
        if(tLen >mCapacity){
            delete mStr;             
            mStr = new CHAR16[tLen+1];
            mCapacity = tLen + 1;            
        } 
        mLen = tLen;
        return StrCpy(mStr, Str); 
    };
    virtual CONST CHAR16* GetDisplayStr(){return mStr;}
    virtual void DrawTo(void* pFrameBuffer, UINT32 ToScreen = 0);
    UINTN GetLen(){return mLen;}
protected:
    CHAR16* mStr;
    UINTN  mLen;
private:
    UINTN  mCapacity;
};

class Label:public StringBrick
{
public:
    enum{INVALID_STRID = 0xFFFFFFFF};
    Label(CHAR16* str = (CHAR16*)L""):mStringId((UINT16)INVALID_STRID),mHiiHandle(0){StrCpy(mStr, str);}
    Label(void* handle, UINT16 strid):mHiiHandle(handle),mStringId(strid){}
public:
    virtual void DrawTo(void* pFrameBuffer, UINT32 ToScreen = 0);
private:
    UINT16 mStringId;
    void*  mHiiHandle;
};

#define GET_BMP_FROM_FILE(bmp, bmpfile) {\
    if(bmpfile != 0 ) {\
    bmp = new bmp_t;\
        unsigned char* ret = read_bmp(bmpfile, bmp);        \
        if(ret==NULL){\
            delete bmp;\
            bmp = 0;\
        }\
    }\
}

class Button:public Label
{
public:
    Button(CHAR16* str, CHAR8* bg = 0):mbackGround(0),Label(str),bShowFrame(TRUE){SetBackGroundFile(bg); Size(100,30);}
    Button(void* handle, UINT16 strid, CHAR8* bg = 0):mbackGround(0),Label(handle,strid),bShowFrame(TRUE){SetBackGroundFile(bg);}
    ~Button(){if(mbackGround){free_bmp(mbackGround); delete mbackGround;} }
public:
    virtual void DrawTo(void* pFrameBuffer, UINT32 ToScreen = 0);    
    virtual void OnEnter();
    virtual void OnClick();
public:
    void SetNoFrame(){bShowFrame=FALSE;}
private:
    void SetBackGroundFile(CHAR8* bg){
        if(bg == 0) return;
        mbackGround = new bmp_t;
        unsigned char* ret = read_bmp(bg, mbackGround);        
        if(ret==NULL){
            delete mbackGround;
            mbackGround = 0;
        }
    }
private:
    bmp_t* mbackGround;
    BOOLEAN bShowFrame;
};

class Switcher:public Label
{
public:
    typedef enum{SWITCH_ON=1, SWITCH_OFF=0}SwitchState;
    void TurnOn();
    void TurnOff();
    Switcher(CHAR8* on, CHAR8* off):mstate(SWITCH_ON),mbmpon(0), mbmpoff(0){
        GET_BMP_FROM_FILE(mbmpon, on);
        GET_BMP_FROM_FILE(mbmpoff, off);
    }    
    virtual void DrawTo(void* pFrameBuffer, UINT32 ToScreen = 0);    
    virtual void OnEnter();
    virtual void OnClick(){OnEnter();};
protected:
    SwitchState mstate;
    bmp_t* mbmpon;
    bmp_t* mbmpoff;
};

class ProgressBar:public Brick
{
public: 
    ProgressBar():mmax(100),mcur(0){}    
    void SetMax(int p){mmax = p;}
    void Set(int p){mcur = p;}
    virtual void DrawTo(void* pFrameBuffer, UINT32 ToScreen = 0);    
    virtual void OnEnter(){};
    virtual void OnClick(){OnEnter();};
protected:
    int mmax;
    int mcur;
};

class HotKeys{
    private:
    typedef void (*HotKeyNotifer)();
    class HotKey{
        public:
            EFI_KEY_DATA Key;
            HotKeyNotifer handler;
    };
    public:
    enum{NO = 0,  YES = 1, HOTKEY_CONITNUE_PROPAGATE = 1, HOTKEY_QUIT_PROPAGATE = 3};
    static int RegisterHotKey(EFI_KEY_DATA Key, void(*handler)());
    static int ExeHotKey(EFI_KEY_DATA aKey);
    static const int MAX_HOT_KEYS = 16;
    static int HotKeyNum ;
    static HotKey gHotKey[MAX_HOT_KEYS];
};


extern EFI_GRAPHICS_OUTPUT_PROTOCOL         *gGraphicsOutput;
#define FULL_WIDTH (gGraphicsOutput->Mode->Info->HorizontalResolution)
#define FULL_HEIGHT (gGraphicsOutput->Mode->Info->VerticalResolution)
}//namespace UGUI
#endif
