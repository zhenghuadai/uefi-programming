/** @file
 **/
extern "C"{
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/SimpleFileSystem.h>
}


class UFile
{
    public:
        typedef EFI_STATUS Ustatus;
        UFile(CHAR16* FileName, UINT64 attr = 0);
        UFile(UFile& root, CHAR16* FileName, UINT64 attr = 0);
        ~UFile();
    public:
        //interface

       Ustatus Open(CHAR16* FileName, UINT64 mode, UINT64 attr); 
    private:
        EFI_FILE_PROTOCOL* m_file;
        static  EFI_FILE_PROTOCOL* m_root;
};
