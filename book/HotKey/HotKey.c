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

EFI_HANDLE notifyHandle;

EFI_STATUS myNotify(EFI_KEY_DATA *key)
{
	Print((CONST CHAR16*)L"Hot Key\n");
	return 0;
}
	EFI_STATUS 
testHotKey()
{
	EFI_STATUS  Status;
	EFI_KEY_DATA hotkey={0};
	EFI_KEY_DATA key = {0};
	EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL* InputEx = NULL;
	Status = gBS->LocateProtocol(
			&gEfiSimpleTextInputExProtocolGuid,
			NULL,
			(VOID**)&InputEx
			);
	Print(L"%r\n", Status);
	hotkey.Key.ScanCode = 0;
	hotkey.Key.UnicodeChar = 'c';
	hotkey.KeyState.KeyShiftState = EFI_LEFT_CONTROL_PRESSED | EFI_SHIFT_STATE_VALID;
	hotkey.KeyState.KeyToggleState = EFI_TOGGLE_STATE_VALID;


	Status = InputEx->RegisterKeyNotify(InputEx,
			&hotkey,
			myNotify,
                        (VOID**)&notifyHandle);
	Print(L"%r\n", Status);
	while( key.Key.UnicodeChar != 'q')
	{
		UINTN Index;
		gBS->WaitForEvent(1, &(InputEx->WaitForKeyEx),  &Index);
	        Status = InputEx->ReadKeyStrokeEx(InputEx,
				&key);
		Print(L"(Scan %x Unicode%x)%x %x %r\n", key.Key.ScanCode, key.Key.UnicodeChar, key.KeyState.KeyShiftState, key.KeyState.KeyToggleState, Status);
		if(key.Key.UnicodeChar == 'q')
			break;
	}
	Status = InputEx->UnregisterKeyNotify(InputEx, notifyHandle);
	return Status;

}

EFI_STATUS UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
		)
{
	return testHotKey(); 
}
