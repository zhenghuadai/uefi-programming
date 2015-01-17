/** @file
**/
#include "disk.h" 
 

EFI_DEVICE_PATH_TO_TEXT_PROTOCOL* Device2TextProtocol;
EFI_HANDLE mImageHandle = 0;

DiskDevice mDiskDevices[16];
UINTN numDiskDevices = 0;


UINTN  FindoutAllDiskDevices(DiskDevice* pDiskDevices)
{
    EFI_STATUS  Status;	
    EFI_HANDLE  *ControllerHandle = NULL;	
    UINTN	    HandleIndex,  NumHandles;
  
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiDiskIoProtocolGuid,
        NULL,
        &NumHandles,
        &ControllerHandle);
    if (EFI_ERROR(Status)) {
        Print(L"LocateHandleBuffer %r\n", Status);
        return 0;
    }

    for (HandleIndex = 0; HandleIndex < NumHandles; HandleIndex++) {
        pDiskDevices[HandleIndex].Controller = ControllerHandle[HandleIndex];
        pDiskDevices[HandleIndex].GetFromController(ControllerHandle[HandleIndex]);
    }

    //numDiskDevices = NumHandles;
    if (ControllerHandle != NULL)
        Status = gBS->FreePool(ControllerHandle);
    return NumHandles;
}



UINTN  ListDevicePath( )
{
    EFI_STATUS  Status;	
    EFI_HANDLE  *ControllerHandle = NULL;	
    UINTN	    HandleIndex,  NumHandles;
    EFI_DEVICE_PATH_PROTOCOL* DiskDevicePath; 

    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiDevicePathProtocolGuid,
        NULL,
        &NumHandles,
        &ControllerHandle);
    if (EFI_ERROR(Status)) {
        Print(L"LocateHandleBuffer %r\n", Status);
        return 0;
    }

    for (HandleIndex = 0; HandleIndex < NumHandles; HandleIndex++) {
        Status = gBS->OpenProtocol(
            ControllerHandle[HandleIndex],
            &gEfiDevicePathProtocolGuid, 
            (VOID**)&DiskDevicePath,
            mImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
        dumpDevicePath(DiskDevicePath);
    }

    //numDiskDevices = NumHandles;
    if (ControllerHandle != NULL)
        Status = gBS->FreePool(ControllerHandle);
    return NumHandles;
}

EFI_STATUS 
GetParameter(
		IN EFI_HANDLE                            ImageHandle,
        OUT UINTN*                               Argc,
        OUT CHAR16***                            Argv
		)
{
	EFI_STATUS  Status;

	EFI_SHELL_PARAMETERS_PROTOCOL *EfiShellParametersProtocol; 
	EFI_SHELL_INTERFACE *EfiShellInterface;

	EFI_GUID mEfiShellParametersProtocolGuid= EFI_SHELL_PARAMETERS_PROTOCOL_GUID;
	EFI_GUID mEfiShellInterfaceGuid= SHELL_INTERFACE_PROTOCOL_GUID;

	//if( gEfiShellParametersProtocol == NULL){
	//}else{
	//}

	Status = gBS -> OpenProtocol(
			ImageHandle,
			&mEfiShellParametersProtocolGuid,
			(VOID **)&EfiShellParametersProtocol,
			ImageHandle,
			NULL,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
    if (!EFI_ERROR(Status)) {
        *Argc = EfiShellParametersProtocol -> Argc;
        *Argv = EfiShellParametersProtocol -> Argv;
    }else{
        Status = gBS -> OpenProtocol(
            ImageHandle,
            &mEfiShellInterfaceGuid,
            (VOID**)&EfiShellInterface,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
        if (!EFI_ERROR(Status)) {
            *Argc = EfiShellInterface -> Argc;
            *Argv = EfiShellInterface -> Argv;
		}else{
		}

	}

	return EFI_SUCCESS;
}

enum{DO_READ='r', DO_WRITE='w', DO_ZERO='z', DO_SET='s', DO_LIST='l', DO_TEST='t'};

EFI_STATUS TestReadBlocks( EFI_BLOCK_IO_PROTOCOL* BlockIo)
{
    EFI_STATUS Status = 0;
    UINT8* Buf;
    UINT32 BlockSize = BlockIo->Media->BlockSize;
    SAFECALL(Status = gBS->AllocatePool(EfiBootServicesCode, BlockSize, (VOID**)&Buf));
    SAFECALL(Status = BlockIo->ReadBlocks(BlockIo, BlockIo->Media->MediaId, 0, BlockSize, (VOID**)Buf));
    SAFECALL(Status = gBS->FreePool(Buf));
    return Status;
}

EFI_STATUS TestWriteBlocks( EFI_BLOCK_IO_PROTOCOL* BlockIo)
{
    EFI_STATUS Status = 0;
    UINT8* Buf;
    UINT32 BlockSize = BlockIo->Media->BlockSize;
    SAFECALL(Status = gBS->AllocatePool(EfiBootServicesCode, BlockSize, (VOID**)&Buf));
    gBS->SetMem(Buf, BlockSize, 0);
    SAFECALL(Status = BlockIo->WriteBlocks(BlockIo, BlockIo->Media->MediaId, 0, BlockSize, (VOID**)Buf));
    SAFECALL(Status = gBS->FreePool(Buf));
    return Status;
}

EFI_STATUS TestReadBlocks2( EFI_BLOCK_IO2_PROTOCOL* BlockIo2)
{
    EFI_STATUS Status = 0;
    UINT8* Buf;
    EFI_BLOCK_IO2_TOKEN b2Token;
    UINTN Index;
    UINT32 BlockSize = BlockIo2->Media->BlockSize;
    SAFECALL(Status = gBS->CreateEvent(0, TPL_NOTIFY, NULL, NULL, &b2Token.Event));
    SAFECALL(Status = gBS->AllocatePool(EfiBootServicesCode, BlockSize, (VOID**)&Buf));
    Status = BlockIo2->ReadBlocksEx(BlockIo2, BlockIo2->Media->MediaId, 0, &b2Token, BlockSize, (VOID**)Buf);
    if(EFI_ERROR(Status)){
        //处理错误
    }else{
        gBS->WaitForEvent(1, &b2Token.Event, &Index);
    }
    SAFECALL(Status = gBS->FreePool(Buf));
    return Status;
}

VOID EFIAPI BlockIo2OnReadWriteComplete (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  )
{
    UINT8* Buf = (UINT8*)Context;
    (VOID)Buf;
    gBS->FreePool(Buf);
}

EFI_STATUS TestReadBlocks2Signal( EFI_BLOCK_IO2_PROTOCOL* BlockIo2)
{
    EFI_STATUS Status = 0;
    UINT8* Buf = NULL;
    EFI_BLOCK_IO2_TOKEN b2Token;
    UINT32 BlockSize = BlockIo2->Media->BlockSize;
    SAFECALL(Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_NOTIFY, BlockIo2OnReadWriteComplete, (VOID*)Buf, &b2Token.Event));
    SAFECALL(Status = gBS->AllocatePool(EfiBootServicesCode, BlockSize, (VOID**)&Buf));
    Status = BlockIo2->ReadBlocksEx(BlockIo2, BlockIo2->Media->MediaId, 0, &b2Token, BlockSize, (VOID**)Buf);
    if(EFI_ERROR(Status)){
        //处理错误
        SAFECALL(Status = gBS->FreePool(Buf));
    }else{
    }
    return Status;
}

EFI_STATUS TestReadDisk2( EFI_DISK_IO2_PROTOCOL* DiskIo2, UINT32 MediaId)
{
    EFI_STATUS Status = 0;
    UINT8* Buf;
    EFI_DISK_IO2_TOKEN d2Token;
    UINTN BufSize = 512;
    UINTN Index;
    SAFECALL(Status = gBS->CreateEvent(0, TPL_NOTIFY, NULL, NULL, &d2Token.Event));
    SAFECALL(Status = gBS->AllocatePool(EfiBootServicesCode, BufSize, (VOID**)&Buf));
    Status = DiskIo2->ReadDiskEx(DiskIo2, MediaId, 0, &d2Token, BufSize, (VOID**)Buf);
    if(EFI_ERROR(Status)){
        //处理错误
    }else{
        gBS->WaitForEvent(1, &d2Token.Event, &Index);
    }
    SAFECALL(Status = gBS->FreePool(Buf));
    return Status;
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{    
    UINTN Argc;
    CHAR16** Argv;
    UINT8* SectorData;
    UINTN Offset=0;
    UINTN SectorNum=1;
    EFI_STATUS  Status;
    UINTN DeviceID = 0xFFFF;
    int action = DO_READ;

    mImageHandle = ImageHandle;

    Status = gBS->LocateProtocol(
        &gEfiDevicePathToTextProtocolGuid,
        NULL,
        (VOID**)&Device2TextProtocol
        );
    GetParameter(ImageHandle, &Argc, &Argv);

    numDiskDevices =   FindoutAllDiskDevices(  mDiskDevices);
    if(Argc == 1){
        Print(L"Usage:  disksector [-[r|w|z|s]][deviceid] Offset #Sectors\n");
        Print(L"Default -r Offset:%d, Number of Sectors: %d \n", Offset, SectorNum);
    }else if( Argc >=2){
        if( Argv[1][0] == '-'){
            action = Argv[1][1];
            if(Argv[1][1] != 0 && Argv[1][2] != 0){
                DeviceID = Argv[1][2] - '0';
            }
            if(Argc >=3)
            Offset    = StrDecimalToUintn(Argv[2]);
            if(Argc >=4)
            SectorNum = StrDecimalToUintn(Argv[3]);
        }
        Print(L"Offset:%d, Number of Sectors: %d \n", Offset, SectorNum);
    }
    if(DeviceID == 0xFFFF)
    {
        UINTN index=0;
        EFI_INPUT_KEY	   Key;
        Print(L"There are %d DiskIo Devices, Please choose one from 0 to %d\n", 
            numDiskDevices,numDiskDevices-1);
        for(UINTN i =0;i <numDiskDevices ;i++){
            Print(L"%d: %s  %d\n", i, mDiskDevices[i].TextDevicePath, mDiskDevices[i].Media->MediaId);
        } 
        Status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &index);
        Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
        switch (Key.UnicodeChar){
             case '0': DeviceID = 0; break;
             case '1': DeviceID = 1; break;
             case '2': DeviceID = 2; break;
             case '3': DeviceID = 3; break;
             case 'q': return 0; break;
             default:  DeviceID = 0; break;
        }
        if(DeviceID >=numDiskDevices) DeviceID = 0;
        Print(L"Using Device %d\n", DeviceID);
    }

    if(action == DO_READ){
        SectorData = mDiskDevices[DeviceID].ReadDisk(Offset, SectorNum);
        mDiskDevices[DeviceID].HexDump(SectorData, SectorNum);     
        delete SectorData;
        Print(L"\n");
    }else if(action == DO_WRITE){
    }else if(action == DO_ZERO){
        UINT8 * buf = new UINT8[mDiskDevices[DeviceID].BlockSize * SectorNum];
        for(UINTN i=0;i<mDiskDevices[DeviceID].BlockSize * SectorNum ;i++) buf[i] = 0;
        EFI_STATUS status = mDiskDevices[DeviceID].WriteDisk(Offset, SectorNum, buf);
        Print(L"write :%r\n", status);
    }else if(action == DO_SET){
        UINT8 * buf = new UINT8[mDiskDevices[DeviceID].BlockSize * SectorNum];
        for(UINTN i=0;i<mDiskDevices[DeviceID].BlockSize * SectorNum ;i++) buf[i] = 255;
        EFI_STATUS status = mDiskDevices[DeviceID].WriteDisk(Offset, SectorNum, buf);
        Print(L"write :%r\n", status);
    }else if(action == DO_LIST){
        mDiskDevices[DeviceID].listInfo();
    }else if(action == DO_TEST){
        TestReadDisk2(mDiskDevices[DeviceID].DiskIo2, mDiskDevices[DeviceID].BlockIo->Media->MediaId);
    }
    return EFI_SUCCESS;
}
