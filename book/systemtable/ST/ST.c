#include <Uefi.h>
EFI_STATUS UefiMain (
        IN EFI_HANDLE        ImageHandle,
        IN EFI_SYSTEM_TABLE  *SystemTable
        )
{
    EFI_STATUS Status;
    UINTN Index;
    EFI_INPUT_KEY  Key;
    CHAR16 StrBuffer[4] = {0};
    //¶ÁÈ¡¼üÅÌ
    SystemTable->BootServices->WaitForEvent(1, &SystemTable ->ConIn->WaitForKey, &Index);
    Status = SystemTable ->ConIn->ReadKeyStroke (SystemTable->ConIn, &Key);
    StrBuffer[0] = Key.UnicodeChar;
    StrBuffer[1] = '\n';
    SystemTable -> ConOut-> OutputString(SystemTable->ConOut, StrBuffer); 
    return EFI_SUCCESS;
}

