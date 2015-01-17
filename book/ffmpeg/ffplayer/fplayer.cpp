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
//#define UEFI_DEBUG
#include "../ffdecoder/ffdecoder.h"
EFI_GUID gEfiFFDecoderProtocolGUID = EFI_FFDECODER_PROTOCOL_GUID ;
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
#if 0
long _ftol2(double f){
	__asm cvttss2si eax , dword ptr f
}

long _ftol2_sse(double f){
	__asm cvttss2si eax , dword ptr f
}
#else
long _ftol2(double f){
	return ftol2_magic(f);
}

long _ftol2_sse(double f){
	return ftol2_magic(f);
}

#endif

#endif
ProgressBar* progress = 0;
UINT32 FullWidth= 0, FullHeight = 0;

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
void CopyToFrameBuffer(EFI_GRAPHICS_OUTPUT_BLT_PIXEL* FrameBuffer, UINT32 FrameBufferWidth, EFI_GRAPHICS_OUTPUT_BLT_PIXEL* Buffer, UINT32 BufferWidth, UINT32 StartX, UINT32 StartY, UINT32 Width, UINT32 Height)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *pSrc, *pDst;
    UINTN row , col;
    pSrc = Buffer;
    pDst = FrameBuffer + FrameBufferWidth * StartY + StartX;
    for(row =0; row <Height; row ++){        
        for(col=0; col <Width; col++){
            pDst[col] = pSrc[col];           
        }
        pDst = pDst + FrameBufferWidth;
        pSrc = pSrc + BufferWidth;
    }
}


const INT32 FF_PLAYING = 1;
const INT32 FF_PAUSE = 0;

EFI_FFDECODER_PROTOCOL *pFFDecoder; 
UINT32 Width=0, Height=0;
INT32 PlayPause = FF_PLAYING;
int FrameNo = 0;
void DecShowFrame()
{
    AVFrame         *pFrame; 
	EFI_IMAGE_OUTPUT                     *pLocalFrameBuffer = (EFI_IMAGE_OUTPUT*) Brick::FrameBuffer;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL        *Bitmap = 0;
    Bitmap = pLocalFrameBuffer->Image.Bitmap;
	if(EFI_ERROR( pFFDecoder-> QueryFrame(pFFDecoder, &pFrame))) return;
	CopyToFrameBuffer(Bitmap, FULL_WIDTH, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)pFrame->data[0], Width, 		
		(FULL_WIDTH - Width) /2,(FULL_HEIGHT - Height) /2,
		Width, Height);
	progress->Set(FrameNo++);
}

int WndProc(UINT32 msgid, WPARAM wParam, LPARAM lParam)
{
	if(msgid == UIEvents::UI_TIMER){		
		if(PlayPause == FF_PLAYING)		{
			DecShowFrame();
			Brick::ReDrawScreen();
		}

	}else if(msgid == UIEvents::UI_MOUSE){ 
	}else if(msgid == UIEvents::UI_KEY){ 
		EFI_INPUT_KEY	                 Key;
		Key = *(EFI_INPUT_KEY*)&wParam;
		if (Key.UnicodeChar == 'q'){
			//Print(L"Computation Time: %ld\n", ct);
			//Print(L"Showing     Time: %ld\n", st);
			Brick::Running = false;
		}
		else if (Key.UnicodeChar == 'f'){ 
#define TMPFACTOR_TYPE UINT32
#define FAXTOR_NUM 1000
			TMPFACTOR_TYPE factor = 0, factorh = 0;
			factor = (((TMPFACTOR_TYPE )FULL_WIDTH) * FAXTOR_NUM)  / (TMPFACTOR_TYPE )Width; 
			factorh =  (((TMPFACTOR_TYPE )FULL_HEIGHT)* FAXTOR_NUM)  / (TMPFACTOR_TYPE )Height; 
			if(factor > factorh) factor = factorh;
			if( factor != 1.0 ){
				Width = (UINT32)((TMPFACTOR_TYPE )Width * factor) / FAXTOR_NUM ;
				Height = (UINT32)((TMPFACTOR_TYPE )Height * factor) / FAXTOR_NUM ;
				EFI_STATUS Status = pFFDecoder-> SetFrameSize(pFFDecoder, Width, Height);
				if(EFI_ERROR(Status))
					Brick::Running = false;;
			}
		}else if (Key.UnicodeChar == 'p'){
			if(PlayPause == FF_PLAYING){
				PlayPause = FF_PAUSE;
			}else if(PlayPause == FF_PAUSE){
				PlayPause = FF_PLAYING;
			}
		}
	}else{
		//switch(msgid)
		//{		
		//default:
		//	break;
		//}
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

EFI_STATUS 
ffMainGui (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{	
	//_asm int 3;
	EFI_STATUS Status;
	
#ifdef FF_ALLINONE
	EFI_FFDECODER_PROTOCOL* EFIAPI InitFFdecoderPrivate ();

	FFDecoder = InitFFdecoderPrivate(); 
#else
	Status = gBS->LocateProtocol(
		&gEfiFFDecoderProtocolGUID ,
		NULL,
		(VOID **)&pFFDecoder );
	if (EFI_ERROR(Status)) {
		return Status;
	}
#endif
    Status = pFFDecoder -> OpenVideo2( pFFDecoder, Argv[1], &pFFDecoder);
    Status = pFFDecoder-> QueryFrameSize(pFFDecoder, &Width, &Height);

#define PLAY_WIDTH 70
    Brick::Initialize();
	Window AdminWindow;
	AdminWindow.SetEventHandler(WndProc);
	const int START_X = 100;
	const int START_Y = FULL_HEIGHT - 200;
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
	return 0;
}

int cppMain(int argc, char **argv)
{
  INTN                           ReturnFromMain = -1;
  EFI_SHELL_PARAMETERS_PROTOCOL *EfiShellParametersProtocol = NULL;
  EFI_STATUS                    Status;

  Status = gST ->BootServices->OpenProtocol(gImageHandle,
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
      EFI_SHELL_INTERFACE           *EfiShellInterface;
      EfiShellInterface = NULL;
    //
    // try to get shell 1.0 interface instead.
    //
    Status = gST ->BootServices->OpenProtocol(gImageHandle,
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

