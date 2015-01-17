/*
 * =====================================================================================
 *
 *       Filename:  audio.c
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
#define UNICODE_ESC 0x001B
#define UNICODE_CR  0x000D
#define UNICODE_BS  0x0008
#define SCANCODE_LEFT  0x03
#define SCANCODE_UP    0x01 
#define SCANCODE_RIGHT 0x04 
#define SCANCODE_DOWN  0x02
#define SCANCODE_ESC   0x17

#include <Protocol/DevicePathToText.h>
#include <Protocol/PciIo.h>
#include <IndustryStandard/Pci22.h>

#include "wav.h"
#include "wintype.h"

extern EFI_PCI_IO_PROTOCOL       *gAudioPciIo ;
extern EFI_HANDLE                 gAudioHandle ;
EFI_DEVICE_PATH_TO_TEXT_PROTOCOL*          gDevice2TextProtocol;
#if 0
#define PrintStr(s) gST -> ConOut-> OutputString(gST -> ConOut, (CHAR16*)s);
#else
#define PrintStr(s)  Print(s);
#endif


//typedef unsigned int    u32;
//typedef unsigned short    u16;
typedef unsigned char    u08;

#if 1
#define myPrint(...) Print(__VA_ARGS__)
#else
#define myPrint(...) 
#endif

UINT8 Bar = 1;
 
 //ac97
 #define GPDCON        0x7F008060
 UINT32 AC_GLBCTRL  =   0x00000000;
 #define AC_GLBSTAT    (AC_GLBCTRL + 0x00000004)
 #define AC_CODEC_CMD  (AC_GLBCTRL + 0x00000008)
 #define AC_CODEC_STAT (AC_GLBCTRL + 0x0000000C)
 #define AC_PCMDATA    (AC_GLBCTRL + 0x00000018)
 #define VIC1INTENABLE (AC_GLBCTRL + 0x00000010)
 #define VIC1INTSELECT (AC_GLBCTRL + 0x0000000C)
 #define VIC1IRQSTATUS (AC_GLBCTRL + 0x00000000)
 
 #define BUF_ADDR 0x53000000
 
 
 void init_ac97();
 void wrreg_ac97(u32 addr, u16 data);
 u16 rdreg_ac97(u32 addr);
 void delay(u32 x);
#define gPCI_REG gAudioPciIo->Io 
 void wrreg16(UINTN addr, u16 data)
 {
    EFI_STATUS Status;
    Status = gPCI_REG.Write(
		    gAudioPciIo,
		    EfiPciIoWidthUint16,
		    Bar,
		    addr,
		    1,
		    &data
		    ); 
    if (!EFI_ERROR (Status)) {
	    myPrint(L" wIO(%x) %0x\n",  addr, data);
	    return ;
    }else{
	    myPrint(L" IO error %r\n",  Status       );
    }
     delay(10000);
 }
 
 u16 rdreg16(UINTN addr)
 {
    EFI_STATUS Status;
    UINT16 Data =0; 

    Status = gPCI_REG.Read(
		    gAudioPciIo,
		    EfiPciIoWidthUint16,
		    Bar,
		    addr,
		    1,
		    &Data
		    ); 
    if (!EFI_ERROR (Status)) {
	    myPrint(L" rIO(%x) %0x\n",  addr, Data);
	    return 0;
    }else{
	    myPrint(L" rIO(%0x) error %r\n",  addr, Status       );
    }
     delay(10000);
     return Data & 0xffff;
 }

 void wrreg32(u32 addr, u32 data)
 {
    EFI_STATUS Status;
    Status = gPCI_REG.Write(
		    gAudioPciIo,
		    EfiPciIoWidthUint32,
		    Bar,
		    addr,
		    1,
		    &data
		    ); 
    if (!EFI_ERROR (Status)) {
	    myPrint(L" wIO(%x) %0x\n",  addr, data);
	    return ;
    }else{
	    myPrint(L" wIO(%x) error %r\n",  addr, Status       );
    }
     delay(10000);
 }
 
 u32 rdreg32(UINTN addr)
 {
    EFI_STATUS Status;
    UINT32 Data =0; 

    Status = gPCI_REG.Read(
		    gAudioPciIo,
		    EfiPciIoWidthUint32,
		    Bar,
		    addr,
		    1,
		    &Data
		    ); 
    if (!EFI_ERROR (Status)) {
	    myPrint(L" rIO(%x) %0x\n",  addr, Data);
	    return 0;
    }else{
	    myPrint(L" rIO(%0x) error %r\n",  addr, Status       );
    }
     delay(10000);
     return Data  ;
 }

 void Play(u32* p, u32 maxi )
 {
     u32 i = 0;
 
     // 
     //wrreg32(GPDCON, 0x44444);
 
     // Cold reset
     wrreg32(AC_GLBCTRL , 1);
     delay(10000);
     {
	     u32 Data = rdreg32(AC_GLBCTRL);
	     wrreg32(AC_GLBCTRL , Data & (~(1)));
     }
     delay(10000);
 
     // Wake up codec from power down
     wrreg16(AC_GLBCTRL , 1 << 1);
     delay(10000);
     {
	     u32 Data = rdreg32(AC_GLBCTRL);
	     wrreg32(AC_GLBCTRL , Data & (~( 1 << 1)));
     }
     delay(10000);
 
     // AC-Link on
     wrreg32(AC_GLBCTRL , 1 << 2);
     delay(10000);
 
     // Transfer data enable using AC-link
     wrreg32(AC_CODEC_CMD , (1 << 23)|(0x00 << 16)|0); 
     {
	     u32 Data = rdreg32(AC_GLBCTRL);
	     wrreg32(AC_GLBCTRL , Data | (1 << 3));
     }
     delay(10000);
 
     // Check if active
     if ((rdreg32(AC_GLBSTAT) & 7) != 3)
     {
         printf("AC97 Controller not Active %d!\n", rdreg32(AC_GLBSTAT));
         //return;
     }
 
     // ??ac97
     init_ac97();
 
     // PCM out channel transfer mode
 // PCM out channel underrun interrupt enable 
     {
	     u32 Data = rdreg32(AC_GLBCTRL);
	     wrreg32(AC_GLBCTRL, Data &( ~(3 << 12)));
	     Data = rdreg32(AC_GLBCTRL);
	     wrreg32(AC_GLBCTRL, Data  | ((1 << 12)|(1 << 21)));
     }
 
     // ??PIO(??)????
     while( i< maxi)
     {
		 wrreg32(AC_GLBCTRL , 0);
         // ????????????
         if (!(rdreg32(AC_GLBSTAT) & (1 << 21))){
		 delay(1000);
            // continue;
	 }
         // ??PCM out channel underrun????
	 {
		 u32 Data = rdreg32(AC_GLBCTRL);
		 wrreg32(AC_GLBCTRL , Data | ( 1 << 29));
	 }
 
         wrreg32(AC_PCMDATA , p[i++]);
     }

 }
 
 
 // ??ac97
 void init_ac97()
 {
     // Register 04h controls the headphone output pins, HPL and HPR.
 //
     wrreg_ac97(0x04, (0 << 15)|(0 << 8)|(0 << 7)|(0));
 
     // Register 0Ch controls the audio DACs (but not AUXDAC).
     wrreg_ac97(0x0C, (0 << 15)|(0 << 8)|(0));
 
     // Register 1Ch controls the inputs to the output PGAs
     wrreg_ac97(0x1C, (2 << 6)|(2 << 4));
 
     // Register 1Eh controls 3D stereo enhancement for the audio DACs
     wrreg_ac97(0x1E, (0 << 5)|(0 << 4)|(8));
 
     // Register 20h controls the bass and treble response of the left and right andio DAC (but not AUXDAC).
     wrreg_ac97(0x20, (1 << 15)|(0 << 12)|(0 << 8)|(0 << 6)|(0 << 4)|(0));
 
     // Register 24h ??????
 //
     
 // Register 26h is for power management according to the AC97 specification.
     wrreg_ac97(0x26, 0);
 
     // Register 2Ah controls variable rate audio
     wrreg_ac97(0x2A, 1);
 
     // Register 2Ch control the sample rates for the stereo DAC.
     wrreg_ac97(0x2C, 0xAC44);
 
     // Register 3Ch is for power management additional to the AC97 speification.
     wrreg_ac97(0x3C, 0);
 
     // Register 3Eh is for power management additional to the AC97 specification.
     wrreg_ac97(0x3E, 0);
 
     // Register 40h is a "general purpose" register as defined by the AC97 specification.
     wrreg_ac97(0x40, 1 << 13);
 
     // Register 42h controls power-up conditions for output PGAs.
     wrreg_ac97(0x42, 0);
 
     // Register 44h controls clock division and muxing.
     wrreg_ac97(0x44, (1 << 7)|(0 << 2)|(0 << 1)|(0));
 }
 
 void wrreg_ac97(u32 addr, u16 data)
 {
    EFI_STATUS Status;
    UINT32 Data = (0 << 23)|(addr << 16)|data;
    Status = gPCI_REG.Write(
		    gAudioPciIo,
		    EfiPciIoWidthUint32,
		    Bar,
		    AC_CODEC_CMD,
		    1,
		    &Data
		    ); 
    if (!EFI_ERROR (Status)) {
	    myPrint(L" wIO(%0x:%x) %0x\n", AC_CODEC_CMD, addr, Data);
	    return ;
    }else{
	    myPrint(L" wIO(%d) error %r\n", addr,  Status       );
    }
     delay(10000);
 }
 
 u16 rdreg_ac97(u32 addr)
 {
    EFI_STATUS Status;
    UINT32 Data = (1 << 23)|(addr << 16);
    Status = gPCI_REG.Write(
		    gAudioPciIo,
		    EfiPciIoWidthUint32,
		    Bar,
		    AC_CODEC_CMD,
		    1,
		    &Data
		    ); 
    if (!EFI_ERROR (Status)) {
	    myPrint(L" wIO(%0x:%x) %0x\n", AC_CODEC_CMD ,addr, Data);
	    return 0;
    }else{
	    myPrint(L" wIO(%x) error %r\n",  addr, Status       );
    }
    delay(10000);

    Status = gPCI_REG.Read(
		    gAudioPciIo,
		    EfiPciIoWidthUint32,
		    0,
		    AC_CODEC_STAT,
		    1,
		    &Data
		    ); 
    if (!EFI_ERROR (Status)) {
	    myPrint(L" rIO(%x:%x) %0x\n", AC_CODEC_STAT, addr, Data);
	    return 0;
    }else{
	    myPrint(L" rIO(%x) error %r\n",  addr, Status       );
    }
     delay(10000);
     return  (u16)(Data & 0xffff);
 }


 void delay(u32 x)
 {
     while (x--)
	     gBS->Stall(1);
         ;
 }



EFI_STATUS
WalkonController(EFI_GUID* guid, EFI_STATUS (*runController)(EFI_HANDLE))
{
    
    EFI_STATUS Status;
    EFI_HANDLE  *ControllerHandle = NULL;	
    UINTN	    /*HandleIndex, ProtocolIndex,*/ NumHandles;
    UINTN i; 
     
 
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        guid,
        NULL,
        &NumHandles,
        &ControllerHandle);
    Print(L"Walk handles %ld\n", NumHandles);
    for(i =0; i<NumHandles; i++){
       
	    Status = runController(ControllerHandle[i]);
	    //Print(L" Open Protocol %r: %x\n", Status, Protocol); 
	    if(!EFI_ERROR(Status)){

	    }

    }
    if(ControllerHandle) gBS->FreePool(ControllerHandle);
    return Status;
}

VOID PrintPciIOConfig(
		IN  EFI_HANDLE  Controller,
		IN  EFI_PCI_IO_PROTOCOL       *PciIo
		)
{
	EFI_STATUS    Status;
	PCI_TYPE00    Pci;

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
		return ;
	}

	myPrint(L" UINT16  VendorId :%0x\n",     Pci.Hdr. VendorId );
	myPrint(L" UINT16  DeviceId :%0x\n",     Pci.Hdr. DeviceId );
	myPrint(L" UINT16  Command :%0x\n",      Pci.Hdr. Command );
	myPrint(L" UINT16  Status :%0x\n",       Pci.Hdr. Status );
	myPrint(L" UINT8   RevisionID :%0x\n",   Pci.Hdr. RevisionID );
	myPrint(L" UINT8   ClassCode[2] :%0x\n", Pci.Hdr. ClassCode[2] );
	myPrint(L" UINT8   ClassCode[1] :%0x\n", Pci.Hdr. ClassCode[1] );
	myPrint(L" UINT8   ClassCode[0] :%0x\n", Pci.Hdr. ClassCode[0] );
	myPrint(L" UINT8   CacheLineSize :%0x\n",Pci.Hdr. CacheLineSize );
	myPrint(L" UINT8   LatencyTimer :%0x\n", Pci.Hdr. LatencyTimer );
	myPrint(L" UINT8   HeaderType :%0x\n",   Pci.Hdr. HeaderType );
	myPrint(L" UINT8   BIST :%0x\n",         Pci.Hdr. BIST          );
	myPrint(L" Bar[0] :%0x\n",         Pci.Device.Bar[0]          );
	myPrint(L" Bar[1] :%0x\n",         Pci.Device.Bar[1]          );
	myPrint(L" Bar[2] :%0x\n",         Pci.Device.Bar[2]          );
	myPrint(L" Bar[3] :%0x\n",         Pci.Device.Bar[3]          );
	myPrint(L" Bar[4] :%0x\n",         Pci.Device.Bar[4]          );
	myPrint(L" Bar[5] :%0x\n",         Pci.Device.Bar[5]          );

	//if ((Pci.Hdr.Command & PCI_DEVICE_ENABLED) == PCI_DEVICE_ENABLED) 
	{
		UINT16 Data = 0;
		if (Pci.Hdr.ClassCode[2] == PCI_CLASS_MEDIA && Pci.Hdr.ClassCode[1] == PCI_CLASS_MEDIA_AUDIO && Pci.Hdr.ClassCode[0] == 0x00) {
			UINT64 reg;
			gAudioPciIo = PciIo;
			gAudioHandle = Controller;
			//Bar = 1;
			//AC_GLBCTRL  =   Pci.Device.Bar[Bar]  & (~1);
			AC_GLBCTRL  =  0; 
			for( reg = 0; reg < 256; reg +=2)
			{
			Status = PciIo->Io.Read(
					PciIo,
					EfiPciIoWidthUint16,
					Bar,
					reg,
					1,
					&Data
					); 
			if (!EFI_ERROR (Status)) {
				myPrint(L" <D(%d):%0x>", reg, Data);
			}else{
				myPrint(L" IO(%d) error %r\n", reg, Status       );
			}
			}

		}
	}
}


	EFI_STATUS 
RunPciIO(IN EFI_HANDLE ControlHandle)
{    
	EFI_STATUS  Status;
	CHAR16*                TextDevicePath;
	EFI_DEVICE_PATH_PROTOCOL  *DiskDevicePath;  
	EFI_PCI_IO_PROTOCOL       *PciIo;
	Status = gBS->OpenProtocol(
			ControlHandle,
			&gEfiDevicePathProtocolGuid, 
			(VOID**)&DiskDevicePath,
			NULL,
			NULL,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL
			);
	TextDevicePath = gDevice2TextProtocol->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE); 
	Print(L"%s",TextDevicePath);
	gBS->FreePool(TextDevicePath);
	Status = gBS->HandleProtocol(ControlHandle, &gEfiPciIoProtocolGuid,(VOID**)&PciIo);
	PrintPciIOConfig(ControlHandle, PciIo);
	return Status;
}

	EFI_STATUS 
ListPciIo(IN EFI_HANDLE ImageHandle)
{
	EFI_STATUS  Status;
	Status = gBS->LocateProtocol(
			&gEfiDevicePathToTextProtocolGuid,
			NULL,
			(VOID**)&gDevice2TextProtocol
			);
	Status = WalkonController(&gEfiPciIoProtocolGuid,RunPciIO);
	return Status;
}


NTSTATUS InitAC97 (void);
void Play2(u32* p, u32 maxi );

EFI_STATUS
EFIAPI
UefiMain (
		IN EFI_HANDLE        ImageHandle,
		IN EFI_SYSTEM_TABLE  *SystemTable
	 )
{    
	EFI_STATUS  Status = EFI_SUCCESS;
    WAV_HEADER* wavHeader = 0;
	//UINTN       Index;
	//UINT32      CurRow=0, CurCol=0;    

	//UINT32 CmdLen = 0;

	// init CPP and global Constructor
	if (gST == NULL)
		gST = SystemTable;
	if (gBS == NULL)
		gBS = gST->BootServices;

	ListPciIo(ImageHandle);
	if(gAudioPciIo == NULL )
		return EFI_NOT_FOUND;

    wavHeader = readWaveFile2("test.wav");
    //((unsigned short*)(wavHeader->wavData));

    //Play((u32*)wavHeader->wavData, wavHeader->dmDataHeader.dwDataSize/4  );
    Play2((u32*)wavHeader->wavData, wavHeader->dmDataHeader.dwDataSize/4  );
    return Status;
}

int main(int argc, char** argv)
{
	UefiMain(gImageHandle, gST);
	return 0;
}
