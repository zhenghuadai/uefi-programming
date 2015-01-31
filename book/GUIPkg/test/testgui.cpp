/*
 * =====================================================================================
 *
 *       Filename:  fplayer.c
 *
 *    Description:  Example of playing video with  EFI_FFDECODER_PROTOCOL 
 *
 *        Version:  1.0
 *        Created:  03/12/2012 09:13:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifdef __cplusplus
extern "C"{
#endif
#include <Uefi.h> 
#include <Base.h> 
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/PciIo.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EfiShellInterface.h>
#include <Protocol/EfiShellParameters.h>
EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
#ifdef __cplusplus
}
#endif
#include "UGui.h"
using namespace UGUI;
#ifdef __GNUC__
#define rdtsc1(t) \
           __asm__ __volatile__("rdtsc" : "=a" (*(int*)&t), "=d" (*(((int*)&t)+1)));
#elif  defined(_MSC_VER)
#define rdtsc1(t) 
#if !defined _STDIO_H_
int _fltused = 0x9875;
#endif

long _ftol2_sse(double f);
long _ftol2(double f);


long ftol2_magic(double x){
	static const double magic = 6755399441055744.0; // (1<<51) | (1<<52)
	double tmp = x;
	tmp += (x > 0) ? -0.499999999999 : +0.499999999999;  // If you need 
	tmp += magic;
	return *(long*)&tmp;
}
long _ftol2(double f){
	return ftol2_magic(f);
}

long _ftol2_sse(double f){
	return ftol2_magic(f);
}


#endif
ProgressBar* progress = 0;
UINT32 FullWidth= 0, FullHeight = 0;
EFI_PHYSICAL_ADDRESS pVideoCardFrameBuffer=0;
EFI_PCI_IO_PROTOCOL                  *g_VideoPciIo;
#define PCI_DEVICE_ENABLED  (EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE) 




EFI_STATUS LocateGraphicsOutput()
{
    EFI_STATUS Status = gBS->LocateProtocol(
            &gEfiGraphicsOutputProtocolGuid,
            NULL,
            (VOID **)&GraphicsOutput);
    if (EFI_ERROR(Status)) {
    }
    FullWidth = GraphicsOutput->Mode->Info->HorizontalResolution;
    FullHeight= GraphicsOutput->Mode->Info->VerticalResolution;
    return Status;
}

const INT32 FF_PLAYING = 1;
const INT32 FF_PAUSE = 0;

UINT32 Width=0, Height=0;

int WndProc(UINT32 msgid, WPARAM wParam, LPARAM lParam)
{
	if(msgid == UIEvents::UI_TIMER){		
	}else if(msgid == UIEvents::UI_MOUSE){ 
	}else if(msgid == UIEvents::UI_KEY){ 
		EFI_INPUT_KEY	                 Key;
		Key = *(EFI_INPUT_KEY*)&wParam;
		if (Key.UnicodeChar == 'q'){
			Brick::Running = false;
		}
		else if (Key.UnicodeChar == 'f'){ 
		}else if (Key.UnicodeChar == 'p'){
		}
	}else{
	}
	return 0;
}

class PlayerSwitcher : public Switcher
{
public:
	PlayerSwitcher(CHAR8* on, CHAR8* off):Switcher(on, off){}
	virtual void OnEnter(){
		Switcher::OnEnter();
		EFI_INPUT_KEY	                 Key;
		Key.UnicodeChar = 'p';
		WndProc	(UIEvents::UI_KEY, *(WPARAM*)&Key, 0);
	};
};

class MyButton : public Button
{
    public:
        MyButton():Button((CHAR16*)L"Button", 0){}
        virtual void OnEnter(){
            Button::OnEnter();
			Brick::Running = false;
        }
};

EFI_STATUS 
ffMainGui (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{	
	//_asm int 3;
	EFI_STATUS Status = 0;
	

#define PLAY_WIDTH 70
    Brick::Initialize();
	Window AdminWindow;
	AdminWindow.SetEventHandler(WndProc);
	const int START_X = 100;
	const int START_Y = FULL_HEIGHT - 200;
    Label* l = new Label((CHAR16*)L"Test");
    l->Pos(50, 50);

    MyButton* b = new MyButton();
    b->Pos(50, 100);
    b->Size(100,30);
	b->AddtoTabList();
	b->SetFocus();

	PlayerSwitcher* btn = new PlayerSwitcher("on.bmp", "off.bmp");
	btn->Pos(START_X, START_Y);
	btn->Size(PLAY_WIDTH,PLAY_WIDTH);
	btn->SetFocus();
	btn->AddtoTabList();

	progress = new ProgressBar();
	progress->Pos(START_X + PLAY_WIDTH , START_Y+ PLAY_WIDTH/2 - 8);
	progress->Size<UINT16>((UINT16)(FULL_WIDTH - (START_X + PLAY_WIDTH)*2),10);	
	progress->AddtoTabList();
	progress->SetMax(1000);

	Brick::Run();
	Brick::Finish();
	return Status ;
}

int main(int argc, char *argv[])
{
  INTN                           ReturnFromMain;
  EFI_SHELL_PARAMETERS_PROTOCOL *EfiShellParametersProtocol;
  EFI_SHELL_INTERFACE           *EfiShellInterface;
  EFI_STATUS                    Status;
  EFI_SYSTEM_TABLE  *SystemTable = gST;

  ReturnFromMain = -1;
  EfiShellParametersProtocol = NULL;
  EfiShellInterface = NULL;

  Status = SystemTable->BootServices->OpenProtocol(gImageHandle,
                             &gEfiShellParametersProtocolGuid,
                             (VOID **)&EfiShellParametersProtocol,
                             gImageHandle,
                             NULL,
                             EFI_OPEN_PROTOCOL_GET_PROTOCOL
                            );
  if (!EFI_ERROR(Status)) {
    //
    // use shell 2.0 interface
    //
    ReturnFromMain = ffMainGui(
                       EfiShellParametersProtocol->Argc,
                       EfiShellParametersProtocol->Argv
                      );
  } else {
    //
    // try to get shell 1.0 interface instead.
    //
    Status = SystemTable->BootServices->OpenProtocol(gImageHandle,
                               &gEfiShellInterfaceGuid,
                               (VOID **)&EfiShellInterface,
                               gImageHandle,
                               NULL,
                               EFI_OPEN_PROTOCOL_GET_PROTOCOL
                              );
    if (!EFI_ERROR(Status)) {
      //
      // use shell 1.0 interface
      //
      ReturnFromMain = ffMainGui(
                         EfiShellInterface->Argc,
                         EfiShellInterface->Argv
                        );
    } else {
      ASSERT(FALSE);
    }
  }
  if (ReturnFromMain == 0) {
    Status =  (EFI_SUCCESS);
  } else {
    Status =  (EFI_UNSUPPORTED);
  }
  return (int)Status;
}

