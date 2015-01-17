#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellLib.h>

INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
	EFI_STATUS Status = 0;
	EFI_STATUS RStatus = 0;	
	EFI_SHELL_PROTOCOL            *mEfiShellProtocol;
	CHAR16* Environment[] = { L"Var1=1", L"Var2=2", 0};
	Status = gBS -> LocateProtocol( &gEfiShellProtocolGuid, NULL, &mEfiShellProtocol);
	Status = mEfiShellProtocol -> Execute(&gImageHandle, L"-nomap ls fs0:",  NULL, &RStatus);
	Print(L"Execute return %r with Status %d\n", Status, RStatus);
	Status = mEfiShellProtocol -> Execute(&gImageHandle, L"-nomap -noversion -nostartup exit 1",  NULL, &RStatus);
	Print(L"Execute return %r with Status %d\n", Status, RStatus);
	Status = mEfiShellProtocol -> Execute(&gImageHandle, L"-nomap -noversion echo Var = %Var1%",  Environment, &RStatus);
	Print(L"Execute return %r with Status %d\n", Status, RStatus);
	return Status;
}

