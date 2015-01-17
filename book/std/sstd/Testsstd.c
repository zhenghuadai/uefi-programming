/** @file
 **/
#include <stdio.h>
#include <stdlib.h>

VOID Test()
{
    CHAR16* pStr;
    pStr = malloc(1024);
    free(pStr);
}

EFI_STATUS UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
		)
{
    EFI_STATUS Status = 0;
    Test();
	return Status;
}
