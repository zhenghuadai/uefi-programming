#ifndef __UGUIEXT_HEADER__
#define __UGUIEXT_HEADER__
 
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

namespace UGUI{

/**
 Support for display the string
**/
class TextBrick:public StringBrick
{
protected:
    CHAR16 mShadow[STR_MAX_LEN+1];
    UINTN  mRefeshes;
public:
    void resize(int length){}
public:
    TextBrick(){mShadow[0]=0; mRefeshes = 0; Status = 0;}
    virtual void LoseFocus(){RemoveCursor(); Brick::LoseFocus();};
    virtual void SetFocus(){Brick::SetFocus(); };
    virtual void OnClick(){SetFocus();}
    virtual CHAR16* SetStr(CHAR16 * Str) { StrCpy(mShadow, Str); return StringBrick::SetStr(Str); };
    virtual CONST CHAR16* GetDisplayStr(){ 
        if(!IsFocused())
            return mShadow;
        static const UINTN cursorFreq = 32;
        UINTN modnum = mRefeshes % cursorFreq;
        if(modnum  == 0){        
            //! No Cursor
            RemoveCursor();         
        }else if(modnum == cursorFreq/2){
            //! Add Cursor
            mShadow[mLen] = (CHAR16)0x7C;
            mShadow[mLen+1]=0;            
        }
        mRefeshes ++;
        return mShadow;
    }
    virtual void OnKey(CHAR16 CurChar){
        if(CurChar == CHAR_CR){             
        }else if(CurChar == CHAR_BS){  
            if(mLen >0)
                mStr [-- mLen] = 0;
        }else if(mLen <= mCapacity){
            mStr[mLen++] = CurChar;
            mStr[mLen]  = 0;
        }else{
            resize(mCapacity * 2);
        }
        if(mLen >=0){
            if(mLen >0)
                mShadow[mLen-1] = mStr[mLen-1];
            mShadow[mLen]=0;
        }
    }
private:
    void RemoveCursor(){mShadow[mLen]=0;}    
};

/**
  PassWord
**/
class PasswordBrick:public TextBrick{
private:
public:
    virtual void OnKey(CHAR16 CurChar){
        StringBrick::OnKey(CurChar);
        if(mLen >=0){
            if(mLen >0)
            mShadow[mLen-1] = L'*';
            mShadow[mLen]=0;
        }
    }
    //virtual CONST CHAR16* GetDisplayStr(){return mShadow;}
};

class OptionList:public StringBrick{
protected:
    enum{MAX_OPTION=16, PIXEL_MAX_WIDTH=200};
    CHAR16* Lists[16];
    UINT16  mOptionNum;
    UINT16  mCurOption;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL* RestorBuffer;
public:
    OptionList():mOptionNum(0),mCurOption(0){ 
        memset(Lists, 0, sizeof(CHAR16*)*MAX_OPTION);
        RestorBuffer = new EFI_GRAPHICS_OUTPUT_BLT_PIXEL[ PIXEL_MAX_WIDTH*16*20];
        mW = 200;
        mH = 20;
    }
    void AddOption(CHAR16* Str){ 
        Lists[mOptionNum] = new CHAR16[StrLen(Str) +1];
        StrCpy(Lists[mOptionNum], Str);
        mOptionNum++;
    }

    void AddOption(CHAR8* Str){ 
        Lists[mOptionNum] = new CHAR16[AsciiStrLen(Str) +1];
        for(UINTN i=0;i<AsciiStrLen(Str);i++) Lists[mOptionNum][i]= (CHAR16) Str[i];
        Lists[mOptionNum][AsciiStrLen(Str)] = 0;
        mOptionNum++;
    }

public:
    virtual CHAR16* GetStr(){return Lists[mCurOption];}
    virtual CONST CHAR16* GetDisplayStr(){return Lists[mCurOption];}
    virtual void DrawTo(void* pFrameBuffer, UINT32 ToScreen = 0) ;
    virtual void OnKey(CHAR16 CurChar){}
    virtual void OnExtKey(UINT16 ExtKey){
        if(ExtKey == SCAN_DOWN)  {
            mCurOption++;
            if(mCurOption >=mOptionNum) mCurOption = 0;
        }else if(ExtKey == SCAN_UP){            
            if(mCurOption == 0) 
                mCurOption =mOptionNum? mOptionNum - 1:0;
            else
                mCurOption--;
        }
    }
    virtual void OnClick();//{SetFocus();}
    virtual bool testHit(UINT16 X, UINT16 Y);
    virtual ~OptionList(){ 
        for(UINT16 i=0;i<mOptionNum;i++) delete Lists[i]; 
        delete RestorBuffer;
    }
};

}//namespace UGUI
#endif
