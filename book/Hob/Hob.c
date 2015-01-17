/** @file
 **/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/SimpleTextInEx.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Library/HobLib.h>

	EFI_STATUS 
testHotKey()
{
	EFI_STATUS  Status = 0;
	EFI_HOB_GENERIC_HEADER *Hob;
	UINT16 HobType;
	UINT16 HobLength;
	for(Hob = GetHobList();!END_OF_HOB_LIST(Hob);Hob = GET_NEXT_HOB(Hob)) {
		HobType = GET_HOB_TYPE (Hob);
		HobLength = GET_HOB_LENGTH (Hob);
		Print((CONST CHAR16*)L"Hob %x %x\n", HobType, HobLength);
	}

	return Status;
}

EFI_STATUS UefiMain(
		IN EFI_HANDLE           ImageHandle,
		IN EFI_SYSTEM_TABLE     *SystemTable
		)
{
	return testHotKey(); 
}
