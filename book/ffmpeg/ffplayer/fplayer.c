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
//#define UEFI_DEBUG
#include "../ffdecoder/ffdecoder.h"
#ifdef __GNUC__
#define rdtsc1(t) \
           __asm__ __volatile__("rdtsc" : "=a" (*(int*)&t), "=d" (*(((int*)&t)+1)));
#elif  defined(_MSC_VER)
#define rdtsc1(t) 
int _fltused = 0x9875;
long _ftol2_sse(double f);
long _ftol2(double f);


long ftol2_magic(double x){
	static const double magic = 6755399441055744.0; // (1<<51) | (1<<52)
	double tmp = x;
	tmp += (x > 0) ? -0.499999999999 : +0.499999999999;  // If you need 
	tmp += magic;
	return *(long*)&tmp;
}
#if 1
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

UINT32 FullWidth= 0, FullHeight = 0;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL* pFrameBuffer0 = 0; 
EFI_GRAPHICS_OUTPUT_BLT_PIXEL* pFrameBuffer = 0; 
EFI_PHYSICAL_ADDRESS pVideoCardFrameBuffer=0;
EFI_PCI_IO_PROTOCOL                  *g_VideoPciIo;
#define PCI_DEVICE_ENABLED  (EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE) 
static 
BOOLEAN
BiosVideoIsVga (
  IN  EFI_PCI_IO_PROTOCOL       *PciIo
  )
{
  EFI_STATUS    Status;
  BOOLEAN       VgaCompatible;
  PCI_TYPE00    Pci;

  VgaCompatible = FALSE;

  //
  // Read the PCI Configuration Header
  //
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        0,
                        sizeof (Pci) / sizeof (UINT32),
                        &Pci
                        );
  if (EFI_ERROR (Status)) {
    return VgaCompatible;
  }

  //
  // See if this is a VGA compatible controller or not
  //
  if ((Pci.Hdr.Command & PCI_DEVICE_ENABLED) == PCI_DEVICE_ENABLED) {
    if (Pci.Hdr.ClassCode[2] == PCI_CLASS_OLD && Pci.Hdr.ClassCode[1] == PCI_CLASS_OLD_VGA) {
      //
      // Base Class 0x00 Sub-Class 0x01 - Backward compatible VGA device
      //
      VgaCompatible = TRUE;
    }

    if (Pci.Hdr.ClassCode[2] == PCI_CLASS_DISPLAY && Pci.Hdr.ClassCode[1] == PCI_CLASS_DISPLAY_VGA && Pci.Hdr.ClassCode[0] == 0x00) {
      //
      // Base Class 3 Sub-Class 0 Programming interface 0 - VGA compatible Display controller
      //
      VgaCompatible = TRUE;
    }
  }

  return VgaCompatible;
}


static EFI_STATUS GetVideoController(EFI_HANDLE* VideoHandle, EFI_PCI_IO_PROTOCOL** VideoPciIo)
{
    EFI_STATUS Status;
    EFI_HANDLE  *ControllerHandle = NULL;
    UINTN    i,  NumHandles;

    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiPciIoProtocolGuid,
            NULL,
            &NumHandles,
            &ControllerHandle);
    for( i =0; i<NumHandles; i++){
        EFI_PCI_IO_PROTOCOL* PciIo ;
        Status = gBS->HandleProtocol(
                ControllerHandle[i],
                &gEfiPciIoProtocolGuid,
                (VOID**)&PciIo
                );
        if(!EFI_ERROR(Status)){
            if(BiosVideoIsVga(PciIo)){
                if(VideoHandle)
                    *VideoHandle = ControllerHandle[i];
                *VideoPciIo = PciIo;
            } 
        }
    }
    if(ControllerHandle) gBS->FreePool(ControllerHandle);
    return Status;
}


EFI_STATUS LocateGraphicsOutput()
{
    EFI_STATUS Status = gBS->LocateProtocol(
            &gEfiGraphicsOutputProtocolGuid,
            NULL,
            (VOID **)&GraphicsOutput);
    if (EFI_ERROR(Status)) {
        Print(L"LocateProtocol %r\n", Status);
    }
    FullWidth = GraphicsOutput->Mode->Info->HorizontalResolution;
    FullHeight= GraphicsOutput->Mode->Info->VerticalResolution;
    pVideoCardFrameBuffer = GraphicsOutput->Mode->FrameBufferBase;
    Status =  gBS->AllocatePool(EfiLoaderData, FullWidth*FullHeight*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL), (VOID**)&pFrameBuffer0 );
    pFrameBuffer = pFrameBuffer0;
    {
        UINT32 i=0;
        CHAR8* p = (CHAR8*)pFrameBuffer;
        for(i=0;i<FullWidth*FullHeight*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);i++)
            p[i]= 0;
    }
    GetVideoController(NULL, &g_VideoPciIo);
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

void ShowFrame0(AVFrame *pFrame, UINT32 width, UINT32 height, int iFrame)
{
    if(GraphicsOutput)
        GraphicsOutput->Blt(
                GraphicsOutput,
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)pFrame->data[0],
                EfiBltBufferToVideo,
                0,0,
                0,(FullHeight - height) /2,
                width, height,
                0
                );
}

void ShowFrame(AVFrame *pFrame, UINT32 width, UINT32 height, int iFrame)
{
    if(g_VideoPciIo){
        EFI_STATUS Status;
        CopyToFrameBuffer(pFrameBuffer, FullWidth, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)pFrame->data[0], width, 
                0, 0,
                width, height);
        Status = g_VideoPciIo->Mem.Write(g_VideoPciIo,
                EfiPciIoWidthUint8,
                EFI_PCI_IO_PASS_THROUGH_BAR,
                (UINT64) (pVideoCardFrameBuffer ),
                FullWidth*height*(4),
                (UINT8*)pFrameBuffer);
    }else{
        ShowFrame0(pFrame, width, height, iFrame);
    }

}


const INT32 FF_PLAYING = 1;
const INT32 FF_PAUSE = 0;

int 
ffMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
    AVFrame         *pFrame; 
    EFI_GUID gEfiFFDecoderProtocolGUID = EFI_FFDECODER_PROTOCOL_GUID ;
    EFI_FFDECODER_PROTOCOL *FFDecoder; 
    UINT32 Width, Height;
    UINT32 FrameWidth, FrameHeight;
    CHAR16* FileName = 0;
    EFI_EVENT                            TimerEvent;
    EFI_EVENT                            WaitArray[2];
    EFI_INPUT_KEY	                 Key;
    INT32 PlayPause = FF_PLAYING;
    EFI_STATUS Status;
    UINT64 t1 = 0, t2 =0, t3=0, ct = 0, st = 0;
    UINTN FrameNo = 0;
    (void)FrameNo;

    // Locate the Protocol
#ifdef FF_ALLINONE
    EFI_FFDECODER_PROTOCOL* EFIAPI InitFFdecoderPrivate ();

    FFDecoder = InitFFdecoderPrivate(); 
#else
    Status = gBS->LocateProtocol(
            &gEfiFFDecoderProtocolGUID ,
            NULL,
            (VOID **)&FFDecoder );
    if (EFI_ERROR(Status)) {
        Print(L"LocateProtocol %r\n", Status);
        return Status;
    }
#endif
    LocateGraphicsOutput();

    // Open Video
    //Status = FFDecoder -> OpenVideo( FFDecoder, FileName );
    FileName =  Argv[1];
    Status = FFDecoder -> OpenVideo2( FFDecoder, FileName, &FFDecoder);
    if (EFI_ERROR(Status)) {
        Print(L"Open %r\n", Status);
        return Status;
    }
    // Query Frame Size(Width height)
    Status = FFDecoder-> QueryFrameSize(FFDecoder, &Width, &Height);
    FrameWidth = Width;
    FrameHeight = Height;
    Print(L"Width:%d Height:%d\n", Width, Height);
    // Set Event
    Status = gBS->CreateEvent(EVT_TIMER, TPL_CALLBACK, NULL, (VOID*)NULL, &TimerEvent);
    Status = gBS->SetTimer(TimerEvent,TimerPeriodic , 10 * 1000 * 1000/24);
    WaitArray[0] = gST->ConIn->WaitForKey;
    WaitArray[1] = TimerEvent;
    // Query Frame 
    while( 1)
    {
	    UINTN index=10;
        if(PlayPause == FF_PLAYING)
        {
//            Print(L"Frmae :%d \n", FrameNo++);
            rdtsc1(t1);
            if(EFI_ERROR( FFDecoder-> QueryFrame(FFDecoder, &pFrame))) break;
            rdtsc1(t2);
            ShowFrame0(pFrame, Width, Height, 0);
            rdtsc1(t3);
            ct += (t2-t1);
            st += (t3-t2);
        }
	    Status = gBS->WaitForEvent(2, WaitArray, &index);
	    if(index == 0){
		    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
		    if (Key.UnicodeChar == 'q'){
                Print(L"Computation Time: %ld\n", ct);
                Print(L"Showing     Time: %ld\n", st);
			    break;
            }
            else if (Key.UnicodeChar == 'f'){
#define TMPFACTOR_TYPE UINT32
#define FAXTOR_NUM 1000
                TMPFACTOR_TYPE factor = 0, factorh = 0;
                factor = ((TMPFACTOR_TYPE )FullWidth) * FAXTOR_NUM  / (TMPFACTOR_TYPE )Width; 
                factorh =  ((TMPFACTOR_TYPE )FullHeight)* FAXTOR_NUM  / (TMPFACTOR_TYPE )Height; 
                if(factor > factorh) factor = factorh;
                if( factor != 1.0 ){
                    Width = (UINT32)((TMPFACTOR_TYPE )Width * factor) / FAXTOR_NUM ;
                    Height = (UINT32)((TMPFACTOR_TYPE )Height * factor) / FAXTOR_NUM ;
                    Status = FFDecoder-> SetFrameSize(FFDecoder, Width, Height);
                    if(EFI_ERROR(Status))
                        break;
                }
            }else if (Key.UnicodeChar == 'p'){
                if(PlayPause == FF_PLAYING){
                    PlayPause = FF_PAUSE;
                }else if(PlayPause == FF_PAUSE){
                    PlayPause = FF_PLAYING;
                }
            }
	    }

    }
    // Close Video
    Status = FFDecoder -> CloseVideo(FFDecoder );
    (void) gBS->FreePool ( pFrameBuffer0 );
    return 0;
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
    ReturnFromMain = ffMain(
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
      ReturnFromMain = ffMain(
                         EfiShellInterface->Argc,
                         EfiShellInterface->Argv
                        );
    } else {
      ASSERT(FALSE);
    }
  }
  if (ReturnFromMain == 0) {
    return (EFI_SUCCESS);
  } else {
    return (EFI_UNSUPPORTED);
  }
}

INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
  return ffMain(Argc, Argv);
}
