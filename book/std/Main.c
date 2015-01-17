/*Main.c */
#include <Uefi.h>
#include <stdio.h>
int main (int argc, char **argv  )
{
  printf("HelloWorld\n"); 
  return 0;
}

EFI_STATUS
EFIAPI
TestConstrucotor(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status = 0;
    return Status ;
}

EFI_STATUS
EFIAPI
Testdestrucotor(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    return 0;
}
