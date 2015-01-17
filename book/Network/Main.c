#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS TestNetwork (IN EFI_HANDLE ImageHandle);
EFI_STATUS
UefiMain (
          IN EFI_HANDLE        ImageHandle,
          IN EFI_SYSTEM_TABLE  *SystemTable
          )
{
   return TestNetwork(gImageHandle);
}

