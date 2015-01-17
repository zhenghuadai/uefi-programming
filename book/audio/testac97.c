/*
 * =====================================================================================
 *
 *       Filename:  testac97.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2012 08:07:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include "audio.h"
#include <Library/BaseMemoryLib.h>

#if 0
#define __cdecl 
#define __inline  inline
#define EFI_SHELL_FILE_PROTOCOL
#include "StdUefi.h"
//#define printf(...)
#include "stdfile.h"
#else
#define memset(dst, c, size) ZeroMem(dst, size)
#endif

#include "wav.h"
#include "sample.h"

char* filename = "test.wav";
EFI_AUDIO_PROTOCOL* Audio;

void playafile(char* fn)
{
    EFI_STATUS Status;
    UINTN BLOCK_SIZE = 65534 / 2 * 32;
    EFI_EVENT myEvent[2];
    WAV_HEADER* wavHeader = 0;
    wavHeader = readWaveFile2(filename);

    if( wavHeader !=0 && wavHeader->dmDataHeader.dwDataSize/4 > 0){
        UINTN TotalSamples = wavHeader->dmDataHeader.dwDataSize/4;
        BLOCK_SIZE = wavHeader->dmFmtHeader.wavFmt.dwSamplesPerSec * 16;
        Print(L"samples block:%d\n", wavHeader->dmDataHeader.dwDataSize/4/BLOCK_SIZE  );
        Print(L"samples:%d\n", wavHeader->dmDataHeader.dwDataSize/4  );

        Status = gBS->CreateEvent(EVT_TIMER  , TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID*)NULL, &myEvent[0]);
        Status = gBS->SetTimer(myEvent[0],TimerPeriodic , 16 * 10 * 1000 * 1000);
        myEvent[1] =  gST->ConIn->WaitForKey;


        //Audio->Play(Audio, ((UINT8*)wavHeader->wavData) , 1 , 2);
        {
              UINTN i = 0;
              UINTN index = 0;
              while(i < TotalSamples){ 
                  UINTN samples = TotalSamples - i;
                  if(samples > BLOCK_SIZE) samples = BLOCK_SIZE;
                  Print(L"%d: samples:%d\n", i, samples);
                  Audio->Play(Audio, ((UINT8*)wavHeader->wavData) + i * 4, 1 , samples * 4);
                  i += BLOCK_SIZE;
                  Print(L"press any key to exit......\n");
                  Status = gBS->WaitForEvent(2, myEvent, &index);
                  if(index == 1){
                      break;
                  }
              }
        }
        gBS->CloseEvent(myEvent[0]);
        freeWavHeader(wavHeader);
    }
}

void playSample()
{
    Audio->Play(Audio, (UINT8*)wavData, 1 ,sizeof(wavData)/4  );
    gBS->Stall(12000);
}

EFI_STATUS
EFIAPI
UefiMain (
		IN EFI_HANDLE        ImageHandle,
		IN EFI_SYSTEM_TABLE  *SystemTable
	 )
{    
	EFI_STATUS  Status = EFI_SUCCESS;
	//UINTN       Index;
	//UINT32      CurRow=0, CurCol=0;    

	//UINT32 CmdLen = 0;
	// init CPP and global Constructor
    //((unsigned short*)(wavHeader->wavData));

    {
        UINTN Index;
        EFI_GUID gEfiAudioProtocolGUID =        EFI_AUDIO_PROTOCOL_GUID; 
        
        Status = gBS->LocateProtocol(
                &gEfiAudioProtocolGUID ,
                NULL,
                (VOID **)&Audio);
        if (EFI_ERROR(Status)) {
            Print(L"LocateProtocol %r\n", Status);
            return Status;
        }
        {
            INT32 v = 200;
            Audio->Volume(Audio, 0,  &v);
        }

        if(filename == NULL){
            playSample();
        }else{
            playafile(filename);
        }
        (void) Index;
        //gBS->WaitForEvent(1, &(Audio->WaitForEndEvent), &Index);
        Print(L"press any key to exit......\n");
        gBS->WaitForEvent(1, &( gST->ConIn->WaitForKey), &Index);
        return Status;
    }
}

int main(int argc, char** argv)
{
    if(argc == 2){
        filename = argv[1];
        printf("The filename %s\n", filename);
    }else{ 
        filename = NULL;
    }
    UefiMain(gImageHandle, gST);
    return 0;
}

#ifndef SEEK_SET
INTN
EFIAPI
ShellAppMain (
          IN UINTN Argc,
          IN CHAR16 **Argv
              )
{

    char FilenameA[32];
    if(Argc == 2){
        UnicodeStrToAsciiStr(FilenameA, Argv[1]);
        filename = FilenameA;
    }
    else 
        filename = "test.wav";
    UefiMain(gImageHandle, gST);
}
#endif
