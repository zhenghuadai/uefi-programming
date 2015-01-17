#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
EFI_STATUS UefiMain (
        IN EFI_HANDLE        ImageHandle,
        IN EFI_SYSTEM_TABLE  *SystemTable
        )
{
    EFI_STATUS Status;
    UINTN Index;
    EFI_INPUT_KEY  Key;
    CHAR16 StrBuffer[4] = {0};
    gST -> ConOut-> OutputString(gST ->ConOut, L"Please enter any key\n"); 
    //¶ÁÈ¡¼üÅÌ
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    StrBuffer[0] = Key.UnicodeChar;
    StrBuffer[1] = '\n';
    gST -> ConOut-> OutputString(gST ->ConOut, StrBuffer); 
    return EFI_SUCCESS;
}

