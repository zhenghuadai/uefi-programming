#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
EFI_STATUS
UefiMain (
          IN EFI_HANDLE        ImageHandle,
          IN EFI_SYSTEM_TABLE  *SystemTable
          )
{
	BASE_LIBRARY_JUMP_BUFFER* jmpBuf;
	BASE_LIBRARY_JUMP_BUFFER* JumpBuffer;
	UINTN                         SetJumpFlag;
	SystemTable -> ConOut-> OutputString(SystemTable->ConOut, L"test SetJump\n"); 
	jmpBuf = AllocatePool (sizeof (BASE_LIBRARY_JUMP_BUFFER) + BASE_LIBRARY_JUMP_BUFFER_ALIGNMENT);
	JumpBuffer = ALIGN_POINTER (jmpBuf, BASE_LIBRARY_JUMP_BUFFER_ALIGNMENT);
        
	SetJumpFlag = SetJump (JumpBuffer);
	if(SetJumpFlag == 0){
		SystemTable -> ConOut-> OutputString(SystemTable->ConOut, L"SetJump return 0\n"); 
		LongJump(JumpBuffer, 1);
	}else if(SetJumpFlag == 1) {
		SystemTable -> ConOut-> OutputString(SystemTable->ConOut, L"SetJump return 1\n"); 
		LongJump(JumpBuffer, 2);
	}
	SystemTable -> ConOut-> OutputString(SystemTable->ConOut, L"SetJump return 2\n"); 
	FreePool(jmpBuf);
	return EFI_SUCCESS;
}

