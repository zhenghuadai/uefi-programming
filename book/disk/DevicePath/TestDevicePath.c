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

EFI_STATUS PrintNode(EFI_DEVICE_PATH_PROTOCOL * Node)
{
    Print(L"(%d %d)/", Node->Type, Node->SubType);
    return 0;
}

EFI_DEVICE_PATH_PROTOCOL* WalkthroughDevicePath(EFI_DEVICE_PATH_PROTOCOL* DevPath, EFI_STATUS (*Callbk)(EFI_DEVICE_PATH_PROTOCOL*))
{
    EFI_DEVICE_PATH_PROTOCOL* pDevPath = DevPath;
    while(!IsDevicePathEnd (pDevPath)){
        if(Callbk){
            EFI_STATUS Status = Callbk(pDevPath);
            if(Status != 0) {
                if(Status < 0) pDevPath = NULL;
                break;
            }
        }
        pDevPath = NextDevicePathNode (pDevPath); 
    }
    return pDevPath;
}

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, 
        IN EFI_SYSTEM_TABLE  *SystemTable)
{
    EFI_STATUS Status;
    UINTN HandleIndex, NumHandles;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL* Device2TextProtocol = 0;

    Status = gBS->LocateProtocol(
            &gEfiDevicePathToTextProtocolGuid,
            NULL,
            (VOID**)&Device2TextProtocol
            );

    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiDiskIoProtocolGuid,
            NULL,
            &NumHandles,
            &ControllerHandle);

    if (EFI_ERROR(Status)) {
        return Status;
    } 
    for (HandleIndex = 0; HandleIndex < NumHandles; HandleIndex++) {
        EFI_DEVICE_PATH_PROTOCOL* DiskDevicePath;   
        Status = gBS->OpenProtocol(
                ControllerHandle[HandleIndex],
                &gEfiDevicePathProtocolGuid, 
                (VOID**)&DiskDevicePath,
                gImageHandle,
                NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
        if (EFI_ERROR(Status)) {
            continue;
        } 

        {                             
            CHAR16*                TextDevicePath = 0;
            TextDevicePath = Device2TextProtocol->ConvertDevicePathToText(DiskDevicePath, FALSE, TRUE); 
            Print(L"%s\n", TextDevicePath);
            if(TextDevicePath)gBS->FreePool(TextDevicePath);
        }

        WalkthroughDevicePath(DiskDevicePath,PrintNode); 
        Print(L"\n\n");
    }
    return 0;
}
