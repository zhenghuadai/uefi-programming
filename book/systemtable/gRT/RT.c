#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#define INIT_NAME_BUFFER_SIZE  128
#define INIT_DATA_BUFFER_SIZE  1024
EFI_STATUS UefiMain (
        IN EFI_HANDLE        ImageHandle,
        IN EFI_SYSTEM_TABLE  *SystemTable
        )
{
    EFI_STATUS Status;
    CHAR16                    *FoundVarName;
    EFI_GUID                  FoundVarGuid;
    UINT8                     *DataBuffer;
    UINTN                     DataSize;
    UINT32                    Atts;
    UINTN                     NameBufferSize; // Allocated Name buffer size
    UINTN                     NameSize;
    CHAR16                    *OldName;
    UINTN                     OldNameBufferSize;
    UINTN                     DataBufferSize; // Allocated data buffer size

    NameBufferSize = INIT_NAME_BUFFER_SIZE;// 128
    DataBufferSize = INIT_DATA_BUFFER_SIZE;// 1024
    // 为变量名分配内存，并且必须初始化为0（至少第一个字符必须为0）。
    FoundVarName   = AllocateZeroPool (NameBufferSize);
    if (FoundVarName == NULL) {
        return (EFI_OUT_OF_RESOURCES);
    }  
    DataBuffer     = AllocatePool (DataBufferSize);
    if (DataBuffer == NULL) {
        FreePool (FoundVarName);
        return (EFI_OUT_OF_RESOURCES);
    }

    for (;;){
        NameSize  = NameBufferSize;
        Status    = gRT->GetNextVariableName (&NameSize, FoundVarName, &FoundVarGuid);
        if (Status == EFI_BUFFER_TOO_SMALL) {
            // 如果缓冲区不够大，重新分配内存后重新执行GetNextVariableName
            OldName           = FoundVarName;
            OldNameBufferSize = NameBufferSize;
            // 分配足够多的内存
            NameBufferSize = NameSize > NameBufferSize*2 ? NameSize : NameBufferSize * 2;
            FoundVarName           = AllocateZeroPool (NameBufferSize);
            if (FoundVarName == NULL) {
                Status = EFI_OUT_OF_RESOURCES;
                FreePool (OldName);
                break;
            }
            //将当前变量名字从旧缓冲区复制到新缓冲区。
            CopyMem (FoundVarName, OldName, OldNameBufferSize);
            FreePool (OldName);
            NameSize = NameBufferSize;
            // 重新执行GetNextVariableName
            Status = gRT->GetNextVariableName (&NameSize, FoundVarName, &FoundVarGuid);
        }
        if (Status == EFI_NOT_FOUND) {
            // 到达变量数据库末尾时退出循环
            break;
        }
        Print(L"%s\n",  FoundVarName);
        //找到一个新的变量，读取该变量的值和属性。
        DataSize  = DataBufferSize;
        Status    = gRT->GetVariable (FoundVarName, &FoundVarGuid, &Atts, &DataSize, DataBuffer);
        if (Status == EFI_BUFFER_TOO_SMALL) {
        }
    }// End for(;;)

    return Status;
}
