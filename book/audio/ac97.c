/*
 * =====================================================================================
 *
 *       Filename:  ac97.c
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

#include <Protocol/DevicePathToText.h>
#include <Protocol/PciIo.h>
#include <IndustryStandard/Pci22.h>
#include "wintype.h"
#include "ac97reg.h"
#include "ichreg.h"

EFI_PCI_IO_PROTOCOL       *gAudioPciIo = 0;
EFI_HANDLE                 gAudioHandle = 0;

const u8* m_pBusMasterBase = 0; 
//extern void delay(u32);
 void Delay(u32 x)
 {
     while (x--)
	     gBS->Stall(1);
         ;
 }
#if 0
#define myPrint(...) Print(__VA_ARGS__)
//#define Print8(Format, ...) Print(L ## Format, ##__VA_ARGS__)
#define Print8(Format, ...) Print(L ## Format ); Print(L"\n")
#define DOUT(a, x) Print8 x
#else
#define DOUT(a, x)
#define myPrint(...) 
#endif

#define PAGED_CODE() 
#define STATUS_SUCCESS EFI_SUCCESS
#define STATUS_IO_TIMEOUT EFI_TIMEOUT
#define NT_SUCCESS(x) (x>=0)
#define STATUS_DEVICE_NOT_READY EFI_NOT_READY

#define gPCI_REG gAudioPciIo->Io 

void KeStallExecutionProcessor (UINTN n)
{
	Delay((u32 )n);
}

EFI_STATUS WRITE_IO(UINT8 bar, u32 addr, u32 data, EFI_PCI_IO_PROTOCOL_WIDTH width)
{
	EFI_STATUS Status;
	Status = gPCI_REG.Write(
			gAudioPciIo,
			width,
			bar,
			addr,
			1,
			&data
			); 
	if (!EFI_ERROR (Status)) {
		myPrint(L" wIO(%d:%x) %0x\n", bar, addr, data);
	}else{
		myPrint(L" wIO(%d:%x) error %r\n", bar, addr,  Status       );
	}
	Delay(100);
	return Status;
}

EFI_STATUS WriteCodec(u32 addr, u16 data)
{
	return WRITE_IO(0, addr, data, EfiPciIoWidthUint16);
}

EFI_STATUS WriteMaster(u32 addr, u8 data)
{
	return WRITE_IO(1, addr, data, EfiPciIoWidthUint8);
}

EFI_STATUS MASTER_WRITE_PORT_USHORT(u32 addr, u16 data)
{
	return WRITE_IO(1, addr, data, EfiPciIoWidthUint16);
}

EFI_STATUS MASTER_WRITE_PORT_ULONG(u32 addr, u32 data)
{
	return WRITE_IO(1, addr, data, EfiPciIoWidthUint32);
}

EFI_STATUS READ_IO(UINT8 bar, u32 addr, void* data, EFI_PCI_IO_PROTOCOL_WIDTH width)
{
	EFI_STATUS Status;
	Status = gPCI_REG.Read(
			gAudioPciIo,
			width,
			bar,
			addr,
			1,
			data
			); 
	if (!EFI_ERROR (Status)) {
		myPrint(L" rIO(%d:%x) %0x\n", bar, addr, *(UINTN*)data);
	}else{
		myPrint(L" rIO(%d:%x) error %r\n", bar, addr,  Status       );
	}
	Delay(100);
	return Status;
}

EFI_STATUS ReadCodec(u32 addr, u16* data)
{
	return READ_IO(0, addr, (void*)data,EfiPciIoWidthUint16);
}

EFI_STATUS ReadMaster(u32 addr, u8* data)
{
	return READ_IO(1, addr, (void*)data, EfiPciIoWidthUint8);
}



u32 MASTER_READ_PORT_ULONG (u32 addr)
{
	u32 data;
	READ_IO(1, addr, (void*)&data, EfiPciIoWidthUint32);
	return data; 
}

u8 MASTER_READ_PORT_UCHAR (u32 addr)
{
	u8 data;
	READ_IO(1, addr, (void*)&data, EfiPciIoWidthUint8);
	return data; 
}

/*****************************************************************************
 * CAC97AdapterCommon::ReadBMControlRegister32
 *****************************************************************************
 * Read a dword (ULONG) from BusMaster Control register.
 */
ULONG ReadBMControlRegister32
(
    IN  ULONG ulOffset
)
{
    ULONG ulValue = (ULONG)-1;

    DOUT (DBG_PRINT, ("[CAC97AdapterCommon::ReadBMControlRegister32]"));

    //ulValue = READ_PORT_ULONG ((PULONG)(m_pBusMasterBase + ulOffset));
    ulValue = MASTER_READ_PORT_ULONG (( ulOffset));

    DOUT (DBG_REGS, ("ReadBMControlRegister read 0x%8x = 0x%4p", ulValue,
                   m_pBusMasterBase + ulOffset));

    return ulValue;
}



void WriteBMControlRegisterMask(
    IN  ULONG reg,
    IN  u8 wData,
    IN  u8 wMask
		)
{
	u8 TempData = MASTER_READ_PORT_UCHAR(reg); 
        TempData &= ~wMask;
        TempData |= (wMask & wData);
	WriteMaster(reg, TempData);
}

/*****************************************************************************
 * CAC97AdapterCommon::WriteBMControlRegister
 *****************************************************************************
 * Writes a word (USHORT) to BusMaster Control register.
 */
void WriteBMControlRegister16
(
    IN  ULONG ulOffset,
    IN  USHORT usValue
)
{
    DOUT (DBG_PRINT, ("[CAC97AdapterCommon::WriteBMControlRegister (USHORT)]"));

    //WRITE_PORT_USHORT ((PUSHORT)(m_pBusMasterBase + ulOffset), usValue);
    MASTER_WRITE_PORT_USHORT ( ulOffset, usValue);

    DOUT (DBG_REGS, ("WriteBMControlRegister wrote 0x%4x to 0x%4p",
                   usValue, m_pBusMasterBase + ulOffset));
}

/*****************************************************************************
 * CAC97AdapterCommon::WriteBMControlRegister
 *****************************************************************************
 * Writes a DWORD (ULONG) to BusMaster Control register.
 */
void WriteBMControlRegister32
(
    IN  ULONG ulOffset,
    IN  ULONG ulValue
)
{
    DOUT (DBG_PRINT, ("[CAC97AdapterCommon::WriteBMControlRegister (ULONG)]"));

    //WRITE_PORT_ULONG ((PULONG)(m_pBusMasterBase + ulOffset), ulValue);
    MASTER_WRITE_PORT_ULONG ( ulOffset, ulValue);

    DOUT (DBG_REGS, ("WriteBMControlRegister wrote 0x%8x to 0x%4p.",
                   ulValue, m_pBusMasterBase + ulOffset));
}


NTSTATUS AcquireCodecSemiphore ()
{

    ULONG ulCount = 0;
    while (MASTER_READ_PORT_UCHAR ( CAS) & CAS_CAS)
    {
        //
        // Do we want to give up??
        //
        if (ulCount++ > 100)
        {
            return STATUS_IO_TIMEOUT;
        }

        //
        // Let's wait a little, 40us and then try again.
        //
        KeStallExecutionProcessor (40L);
    }

    return STATUS_SUCCESS;
}

#define CODEC_REG2ADDR(reg) (reg*2)

EFI_STATUS ReadCodecRegister (
		IN  AC97Register reg,
		OUT PWORD wData
		)
{
       EFI_STATUS ntStatus;
       u32 Status;
       ntStatus = AcquireCodecSemiphore ();
       ntStatus = ReadCodec(CODEC_REG2ADDR(reg), wData);
       Status = MASTER_READ_PORT_ULONG (( GLOB_STA));
       if (Status & GLOB_STA_RCS)
       {
	       //
	       // clear the timeout bit
	       //
	       MASTER_WRITE_PORT_ULONG(( GLOB_STA), Status);
	       *wData = 0;
	       DOUT(DBG_ERR, ("ReadCodec Error"));
	       return STATUS_IO_TIMEOUT;
       }
       return ntStatus;
}

NTSTATUS WriteCodecRegister
(
    IN  AC97Register reg,
    IN  WORD wData,
    IN  WORD wMask
)
{

    WORD TempData = 0;
    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // No mask?  Could happen when you try to prg. left channel of a
    // mono volume.
    //
    if (!wMask)
        return STATUS_SUCCESS;

    //
    // Check to see if we are only writing specific bits.  If so, we want
    // to leave some bits in the register alone.
    //
    if (wMask != 0xffff)
    {
        //
        // Read the current register contents.
        //
        ntStatus = ReadCodecRegister (reg, &TempData);
        if (!NT_SUCCESS (ntStatus))
        {
            return ntStatus;
        }

        //
        // Do the masking.
        //
        TempData &= ~wMask;
        TempData |= (wMask & wData);
    }
    else
    {
        TempData = wData;
    }


    //
    // Grab the codec access semiphore.
    //
    ntStatus = AcquireCodecSemiphore ();
    if (!NT_SUCCESS (ntStatus))
    {
        return ntStatus;
    }

    //
    // Write the data.
    //
    //WRITE_PORT_USHORT (m_pCodecBase + reg, TempData);
    WriteCodec(CODEC_REG2ADDR(reg), TempData);

    //
    // Update cache.
    //

    return STATUS_SUCCESS;

}

/*****************************************************************************
 * CAC97AdapterCommon::PrimaryCodecReady
 *****************************************************************************
 * Checks whether the primary codec is present and ready.  This may take
 * awhile if we are bringing it up from a cold reset so give it a second
 * before giving up.
 */
NTSTATUS PrimaryCodecReady (void)
{

    ULONG           WaitCycles = 200;
    LARGE_INTEGER   WaitTime;
    DWORD dwRegValue = ReadBMControlRegister32 (GLOB_CNT);

    PAGED_CODE ();

    DOUT (DBG_PRINT, ("[CAC97AdapterCommon::PrimaryCodecReady]"));
    //
    // Enable the AC link and raise the reset line.
    //

    // If someone enabled GPI Interrupt Enable, then he hopefully handles that
    // too.
    dwRegValue = (dwRegValue | GLOB_CNT_COLD) & ~(GLOB_CNT_ACLOFF | GLOB_CNT_PRIE);
    WriteBMControlRegister32 (GLOB_CNT, dwRegValue);

    //
    // Wait for the Codec to be ready.
    //

    WaitTime.QuadPart = (UINT64)(-50000);   // wait 5000us (5ms) relative

    do
    {
        //if (READ_PORT_ULONG ((PULONG)(m_pBusMasterBase + GLOB_STA)) &
        if (MASTER_READ_PORT_ULONG ( GLOB_STA) &
            GLOB_STA_PCR)
        {
            return STATUS_SUCCESS;
        }

	KeStallExecutionProcessor(200); 
        //KeDelayExecutionThread (KernelMode, FALSE, &WaitTime);
    } while (WaitCycles--);

    DOUT (DBG_ERROR, ("PrimaryCodecReady timed out!"));
    return STATUS_IO_TIMEOUT;
}


/*****************************************************************************
 * CAC97AdapterCommon::PowerUpCodec
 *****************************************************************************
 * Sets the Codec to the highest power state and waits until the Codec reports
 * that the power state is reached.
 */
NTSTATUS PowerUpCodec (void)
{

    WORD        wCodecReg;
    NTSTATUS    ntStatus;
    ULONG           WaitCycles = 200;
    LARGE_INTEGER   WaitTime;

    PAGED_CODE ();
    DOUT (DBG_PRINT, ("[CAC97AdapterCommon::PowerUpCodec]"));

    //
    // Power up the Codec.
    //
    WriteCodecRegister (AC97REG_POWERDOWN, 0x00, 0xFFFF);

    //
    // Wait for the Codec to be powered up.
    //

    WaitTime.QuadPart = (UINT64)(-50000);   // wait 5000us (5ms) relative

    do
    {
        //
        // Read the power management register.
        //
        ntStatus = ReadCodecRegister (AC97REG_POWERDOWN, &wCodecReg);
        if (!NT_SUCCESS (ntStatus))
        {
            wCodecReg = 0;      // Will cause an error.
            break;
        }

        //
        // Check the power state. Should be ready.
        //
        if ((wCodecReg & 0x0f) == 0x0f)
            break;

        //
        // Let's wait a little, 5ms and then try again.
        //
        //KeDelayExecutionThread (KernelMode, FALSE, &WaitTime);
    } while (WaitCycles--);

    // Check if we timed out.
    if ((wCodecReg & 0x0f) != 0x0f)
    {
        DOUT (DBG_ERROR, ("PowerUpCodec timed out. CoDec not powered up."));
        ntStatus = STATUS_DEVICE_NOT_READY;
    }

    return ntStatus;
}



NTSTATUS InitAC97 (void)
{


    //
    // First check if there is an AC link to the primary CoDec.
    //
    NTSTATUS ntStatus = PrimaryCodecReady ();
    if (NT_SUCCESS (ntStatus))
    {
        //
        // Second, reset this primary CoDec; If this is a AMC97 CoDec, only
        // the audio registers are reset. If this is a MC97 CoDec, the CoDec
        // should ignore the reset (according to the spec).
        //
        WriteCodecRegister (AC97REG_RESET, 0x00, 0xFFFF);

        ntStatus = PowerUpCodec ();
    }
    else
    {
        DOUT (DBG_ERROR, ("Initialization of AC97 CoDec failed."));
    }

    return ntStatus;
}

void run()
{
	WriteBMControlRegisterMask(PO_CR           , 1,1);
	gBS->Stall(1000);
}

void Play2(u32* p, u32 maxi )
{
	BufferDescriptor bds[3];
	EFI_STATUS Status;
	Status = InitAC97();
	if(EFI_ERROR(Status))
	{
		Print(L"Init AC97 ERROR:%r\n", Status);
	}
	{
		WORD data= 0;
		ReadCodecRegister(AC97REG_PCM_OUT_VOLUME, &data); 
		Print(L" Pcm Out Volume :%d\n", data);
		WriteCodecRegister (AC97REG_PCM_OUT_VOLUME, 0xF0, 0xFFFF);
		ReadCodecRegister(AC97REG_PCM_OUT_VOLUME, &data); 
		Print(L" Pcm Out Volume :%d\n", data);
	}

	bds[0].addr = (u32)p;
	bds[0].len = (u16)maxi;
	bds[0].BUP = 1;
	bds[0].IOC = 0;
	WriteBMControlRegister32(PO_BDBAR       ,  (u32)bds);
	WriteBMControlRegister(PO_LVI         ,  0);

	run();
	{
		//EFI_EVENT myEvt;

	}
	(void) bds;
}
