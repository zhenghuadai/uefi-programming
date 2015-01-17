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
#include <Uefi/UefiGpt.h>
#include <Library/DevicePathLib.h>
EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, 
                              IN EFI_SYSTEM_TABLE  *SystemTable)
{
    EFI_STATUS                              Status;
    UINTN                             HandleIndex, HandleCount;
    EFI_HANDLE                            *DiskControllerHandles = NULL;
    EFI_DISK_IO_PROTOCOL               *DiskIo;

/*找到所有提供 EFI_DISK_IO_PROTOCOL 的设备*/
    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiDiskIoProtocolGuid,
            NULL,
            &HandleCount,
            &DiskControllerHandles);

   if (!EFI_ERROR(Status)) {
        CHAR8 gptHeaderSector[512];
        EFI_PARTITION_TABLE_HEADER* gptHeader = (EFI_PARTITION_TABLE_HEADER*)gptHeaderSector;
        for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
            /*打开EFI_DISK_IO_PROTOCOL  */ 
            Status = gBS->HandleProtocol(
                    DiskControllerHandles[HandleIndex],
                    &gEfiDiskIoProtocolGuid,
                    (VOID**)&DiskIo);

        if (!EFI_ERROR(Status)){
                {
                    EFI_DEVICE_PATH_PROTOCOL          *DiskDevicePath;
                    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL  *Device2TextProtocol = 0;
                    CHAR16*                   TextDevicePath = 0;
                    /*1. 打开EFI_DEVICE_PATH_PROTOCOL  */  
                    Status = gBS->OpenProtocol(
                            DiskControllerHandles[HandleIndex],
                            &gEfiDevicePathProtocolGuid,
                            (VOID**)&DiskDevicePath,
                            ImageHandle,
                            NULL,
                            EFI_OPEN_PROTOCOL_GET_PROTOCOL);
                    if(!EFI_ERROR(Status)){
                         if(Device2TextProtocol == 0)
                            Status = gBS->LocateProtocol(
                                   &gEfiDevicePathToTextProtocolGuid,
                                    NULL,
                                    (VOID**)&Device2TextProtocol);
             /*2. 使用 EFI_DEVICE_PATH_PROTOCOL  得到文本格式的Device Path  */  
                        TextDevicePath = Device2TextProtocol->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);
                       Print(L"%s\n", TextDevicePath);
                       if(TextDevicePath)gBS->FreePool(TextDevicePath);
                        /*3. 关闭 EFI_DEVICE_PATH_PROTOCO */   
                        Status = gBS->CloseProtocol(
                                 DiskControllerHandles[HandleIndex],
                                 &gEfiDevicePathProtocolGuid,
                                ImageHandle,
                                NULL
                                );
                    }
                }
                {
                    EFI_BLOCK_IO_PROTOCOL* BlockIo = *(EFI_BLOCK_IO_PROTOCOL**) (DiskIo + 1);
                    EFI_BLOCK_IO_MEDIA* Media = BlockIo->Media;
                    /*读1号扇区。  */   
                    Status = DiskIo->ReadDisk(DiskIo, Media->MediaId, 512, 512, gptHeader);
                    /*检查GPT标志。  */    
                    if((!EFI_ERROR(Status)) &&( gptHeader -> Header.Signature == 0x5452415020494645)){
                        UINT32 CRCsum;
                        UINT32 GPTHeaderCRCsum =  (gptHeader->Header.CRC32);
                        gptHeader->Header.CRC32 = 0;
                        gBS -> CalculateCrc32(gptHeader , 
                                (gptHeader->Header.HeaderSize), &CRCsum);
                        if(GPTHeaderCRCsum == CRCsum){
                            // 找到合法的GPT Header
                        }
                    }
                }
            }
        }
        gBS->FreePool(DiskControllerHandles);
    }
   return 0;
} 

