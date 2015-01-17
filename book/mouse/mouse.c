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

	EFI_STATUS 
testMouseSimple()
{
	EFI_STATUS  Status;
	EFI_SIMPLE_POINTER_PROTOCOL* mouse = 0;
	EFI_SIMPLE_POINTER_STATE     State;
	EFI_EVENT events[2]; // = {0, gST->ConIn->WaitForKey};
	//显示光标
	gST->ConOut->EnableCursor (gST->ConOut, TRUE);
	//找出鼠标设备
	Status = gBS->LocateProtocol(
			&gEfiSimplePointerProtocolGuid,
			NULL,
			(VOID**)&mouse
			);
	// 重置鼠标设备
	Status = mouse->Reset(mouse, TRUE);
	// 将鼠标事件放到等待事件数组
	events[0] = mouse->WaitForInput;
	events[1] = gST->ConIn->WaitForKey;

	while(1)
	{
		EFI_INPUT_KEY	   Key;
		UINTN index;
		// 等待events中的任一事件发生
		Status = gBS->WaitForEvent(2, events, &index);
		if(index == 0){
			// 获取鼠标状态并输出
			Status = mouse->GetState(mouse, &State);
			Print(L"X:%d Y:%d Z:%d L:%d R:%d\n",
					State.RelativeMovementX,
					State.RelativeMovementY,
					State.RelativeMovementZ,
					State.LeftButton,
					State.RightButton
			     );
		} else{            
			Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
			if (Key.UnicodeChar == 'q')
				break;
		}
	}
	return EFI_SUCCESS;
}

EFI_STATUS UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
		)
{
	return testMouseSimple(); 
}
