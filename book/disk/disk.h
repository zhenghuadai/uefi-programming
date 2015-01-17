/** @file
**/
extern "C"
{
#include <Uefi.h>
#include <Base.h> 
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/ShellLib.h>
#include <Protocol/PlatformDriverOverride.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/DiskIo.h> 
#include <Protocol/BlockIo.h> 
#include <Protocol/DiskIo2.h> 
#include <Protocol/BlockIo2.h> 
#include <Protocol/DevicePathToText.h>


#include <sstd.h>
#include <uutil.h>
#undef NULL
#define NULL 0
 
}//! extern C
 
#define Print(...) Print((CONST CHAR16*) __VA_ARGS__)

extern EFI_HANDLE mImageHandle ;
__inline EFI_STATUS 
_LocalOpenProtocol(
		IN EFI_HANDLE ImageHandle,
		EFI_GUID* ProtocolGUIDP,
		VOID** ProtocolHandleP
		)
{
	EFI_STATUS  Status;
	Status = gBS -> OpenProtocol(
			ImageHandle,
			ProtocolGUIDP,
			(VOID **)ProtocolHandleP,
			ImageHandle,
			NULL,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL
			);
	return Status;
}

__inline EFI_STATUS 
_LocalOpenProtocol(
		EFI_GUID* ProtocolGUIDP,
		VOID** ProtocolHandleP
		)
{
	EFI_STATUS  Status;
	Status = gBS->LocateProtocol(
			ProtocolGUIDP,
			NULL,
			(VOID **)ProtocolHandleP);
	return Status;
}

__inline void HexDump(UINT8* buf, UINTN n)
{
    UINTN Index;
    for(Index = 0; Index < n; Index ++){        
        if(Index % 8 ==0 ) Print(L" ");
        if(Index % 16 ==0 ) Print(L"\n");
        Print(L"%2x ", buf[Index]);
    }
}

__inline void dumpDevicePath(EFI_DEVICE_PATH_PROTOCOL* DevPath)
{
    EFI_DEVICE_PATH_PROTOCOL* pDevPath = DevPath;
    UINT16 Length;
    while((pDevPath -> Type != 0xFF )&&(pDevPath -> SubType != 0xFF)){
        UINT8* p8 = (UINT8*)pDevPath;
        UINTN i;
        Length = pDevPath -> Length[0] + 256 * pDevPath -> Length[1] ;
        Print(L"0x%x 0x%x 0x%x 0x%x(", pDevPath -> Type, pDevPath -> SubType, pDevPath -> Length[0], pDevPath -> Length[1]);
        for( i = 4; i< Length; i++){
            Print(L"0x%x ", p8[i]);
        }
        Print(L")/");
        pDevPath = (EFI_DEVICE_PATH_PROTOCOL*) &p8[Length];
    }
    Print(L"\n");
}


extern EFI_DEVICE_PATH_TO_TEXT_PROTOCOL* Device2TextProtocol;
class DiskDevice
{
public:
    DiskDevice(){memset(this, 0, sizeof(DiskDevice));}
    DiskDevice(EFI_HANDLE ControlHandle){}
    ~DiskDevice(){Close();}
public:
    //! Handle the Default Device
    void GetDefault(){
        _LocalOpenProtocol(&gEfiDiskIoProtocolGuid, (VOID**)&DiskIo);
        //Print(L"DiskIo :%0x\n", DiskIo);

        BlockIo = *(EFI_BLOCK_IO_PROTOCOL**) (DiskIo + 1);
        Media = BlockIo->Media;
        BlockSize = Media->BlockSize;
    }

    //! Handle the device from ControlHandle
    void GetFromController(EFI_HANDLE ControlHandle){
        EFI_STATUS  Status;	
        Status = gBS->OpenProtocol(
            ControlHandle,
            &gEfiDiskIoProtocolGuid, 
            (VOID**)&DiskIo,
            mImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
        if (EFI_ERROR(Status)) {
            return;
        }

        Status = gBS->OpenProtocol(
            ControlHandle,
            &gEfiBlockIoProtocolGuid, 
            (VOID**)&BlockIo,
            mImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
        if (EFI_ERROR(Status)) {
            return;
        }

        Status = gBS->OpenProtocol(
            ControlHandle,
            &gEfiDiskIo2ProtocolGuid, 
            (VOID**)&DiskIo2,
            mImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
        if (EFI_ERROR(Status)) {
            DiskIo2 = (EFI_DISK_IO2_PROTOCOL*)(DiskIo + 1);
        }

        Status = gBS->OpenProtocol(
            ControlHandle,
            &gEfiBlockIo2ProtocolGuid, 
            (VOID**)&BlockIo2,
            mImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
        if (EFI_ERROR(Status)) {
            BlockIo2 = ((EFI_BLOCK_IO2_PROTOCOL**)(DiskIo2 + 1))[1];
        }

        Media = BlockIo->Media;
        BlockSize = Media->BlockSize;

        Status = gBS->OpenProtocol(
            ControlHandle,
            &gEfiDevicePathProtocolGuid, 
            (VOID**)&DiskDevicePath,
            mImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
        TextDevicePath = Device2TextProtocol->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE); 
    }

    //!Close the protocol
    void Close(){
        EFI_STATUS  Status;	
        if(DiskIo)
        Status = gBS->CloseProtocol(
            Controller,
            &gEfiDiskIoProtocolGuid,              
            mImageHandle,
            NULL       
            );
        if(TextDevicePath)gBS->FreePool(TextDevicePath);
        memset(this, 0, sizeof(DiskDevice));
    }

    void dumpDevicePath(){
        ::dumpDevicePath(DiskDevicePath);
    }
public:
    UINT8*
        ReadDisk(
        UINTN Offset,
        UINTN SectorNum,
        UINT8* Buffer = 0
        )
    {
        UINT8* buf = Buffer;
        if(buf == NULL) buf = new UINT8[SectorNum* BlockSize];
        EFI_STATUS  Status;
        Status =  DiskIo->ReadDisk( DiskIo,  Media->MediaId, Offset*  BlockSize,  SectorNum* BlockSize, buf);
        Print(L"Read Disk :%r\n", Status);
        if(EFI_ERROR(Status)){
            if(Buffer == 0){
                delete buf;
            }
            buf = 0;
        }
        return buf;
    }

    EFI_STATUS WriteDisk(
        IN UINT64 Offset,
        UINTN SectorNum,
        IN VOID *Buffer
        ){
            return DiskIo->WriteDisk(DiskIo, Media->MediaId, Offset*  BlockSize ,  SectorNum* BlockSize, Buffer);
    }

    void HexDump(UINT8* buf, UINTN n)
    {
        ::HexDump(buf, n*BlockSize);
    }

    void listInfo();
public:
    EFI_DISK_IO_PROTOCOL*  DiskIo ;
    EFI_DISK_IO2_PROTOCOL*  DiskIo2 ;
    EFI_BLOCK_IO_PROTOCOL* BlockIo;
    EFI_BLOCK_IO2_PROTOCOL* BlockIo2;
    EFI_BLOCK_IO_MEDIA*    Media;
    UINT32                 BlockSize;
    EFI_HANDLE             Controller;
    CHAR16*                TextDevicePath;
    EFI_DEVICE_PATH_PROTOCOL* DiskDevicePath;
};
