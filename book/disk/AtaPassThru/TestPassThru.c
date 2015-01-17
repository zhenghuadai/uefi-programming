#include <Uefi.h> 
#include <Base.h> 
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Protocol/DiskIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <IndustryStandard/Atapi.h>
#include <Protocol/AtaPassThru.h>
#include <Uefi/UefiGpt.h>
#include <Library/DevicePathLib.h>

EFI_ATA_PASS_THRU_PROTOCOL *AtaPassThroughProtocol;

EFI_STATUS ReadSectors(UINT16 Port, UINT16 PortMultiplierPort, UINT64 StartLba, UINT32 SectorCount, CHAR8* Buffer, UINT32 DeviceId)
{
	EFI_STATUS Status;
    UINT32 IsExt = (StartLba > 0xFFFFFFFFFFFFFFFF);  
    EFI_ATA_PASS_THRU_COMMAND_PACKET Packet;

    Packet.Protocol          = EFI_ATA_PASS_THRU_PROTOCOL_UDMA_DATA_IN;
    Packet.Acb->AtaCommand   = IsExt?  ATA_CMD_READ_DMA_EXT : ATA_CMD_READ_DMA ;
    Packet.Timeout = 0;
    Packet.Length            = EFI_ATA_PASS_THRU_LENGTH_SECTOR_COUNT;
    Packet.OutTransferLength = 0;
    Packet.OutDataBuffer     = NULL;
    Packet.InTransferLength  = SectorCount;
    Packet.InDataBuffer      = Buffer;
    Packet.Acb->AtaFeatures          = 0;    
    Packet.Acb->AtaFeaturesExp       = 0;   
    Packet.Acb->AtaSectorNumber      = (UINT8) StartLba; 
    Packet.Acb->AtaCylinderLow       = (UINT8) RShiftU64 (StartLba, 8); 
    Packet.Acb->AtaCylinderHigh      = (UINT8) RShiftU64 (StartLba, 16);
    Packet.Acb->AtaSectorCount       = (UINT8) SectorCount;  
    Packet.Acb->AtaDeviceHead        = (UINT8) (0xE0 | (PortMultiplierPort<< 4)); 
    if(IsExt != 0){
        Packet.Acb->AtaSectorNumberExp   = (UINT8) RShiftU64 (StartLba, 24);
        Packet.Acb->AtaCylinderLowExp    = (UINT8) RShiftU64 (StartLba, 32);
        Packet.Acb->AtaCylinderHighExp   = (UINT8) RShiftU64 (StartLba, 40);
        Packet.Acb->AtaSectorCountExp = (UINT8) (SectorCount >> 8);  
    }else{
         Packet.Acb->AtaDeviceHead = (UINT8) (Packet.Acb->AtaDeviceHead | RShiftU64 (StartLba, 24));
    }    

    Status = AtaPassThroughProtocol->PassThru(AtaPassThroughProtocol,
        Port,
        PortMultiplierPort,
        &Packet,
        NULL);
    return Status;
}

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, 
                              IN EFI_SYSTEM_TABLE  *SystemTable)
{
    EFI_STATUS                              Status = 0;

   return Status;
} 
