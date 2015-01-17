#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>

EFI_STATUS InitShellLib()
{
	if(gEfiShellProtocol == NULL)
	{
		gBS -> LocateProtocol(&gEfiShellProtocolGuid, NULL, &gEfiShellProtocol);
	}
	if(gEfiShellParametersProtocol == NULL)
	{
		gBS->OpenProtocol(
				gImageHandle,
				&gEfiShellParametersProtocolGuid,
				(VOID **)&gEfiShellParametersProtocol,
				gImageHandle,
				NULL,
				EFI_OPEN_PROTOCOL_GET_PROTOCOL
				);
	}
	return 0;
}

EFI_STATUS TestString(EFI_HANDLE HiiHandle )
{
	EFI_STATUS Status = 0;
	CHAR8* BestLanguage = "en-US";
	EFI_STRING TempString = NULL;
	UINTN StringSize = 0;
	Status = gHiiString -> GetString (
			 gHiiString,
                         BestLanguage,
                         HiiHandle,
                         STRING_TOKEN (STR_LANGUAGE_SELECT),
                         TempString,
                         &StringSize,
                         NULL
                         );

	if(Status == EFI_BUFFER_TOO_SMALL){
		TempString = AllocatePool (StringSize);
		Status = gHiiString -> GetString (
			 gHiiString,
                         BestLanguage,
                         HiiHandle,
                         STRING_TOKEN (STR_LANGUAGE_SELECT),
                         TempString,
                         &StringSize,
                         NULL
                         );
		Print(L"%s\n", TempString);
		FreePool(TempString);

	}else{
		Print(L"GetString %r\n", Status);
	}
	return 0;
}

EFI_STATUS TestLang(EFI_HANDLE HiiHandle )
{
	EFI_STATUS Status = 0;
	CHAR8 buf[256];
	CHAR16 buf16[256];
	UINTN LangSize = 256;
	UINTN i = 0;

	Status = gHiiString -> GetLanguages(
			 gHiiString,
			 HiiHandle,
			 buf,
			 &LangSize);
	for(i =0; i< LangSize; i++){
		buf16[i] = buf[i];
	}
	buf16[LangSize] = 0;
	Print(L"Lang %s\n", buf16);
	return Status;
}

INTN
EFIAPI
ShellAppMain (
		IN UINTN Argc,
		IN CHAR16 **Argv
	     )
{
	EFI_GUID   mStrPackageGuid = { 0xedd31def, 0xf262, 0xc24e, 0xa2, 0xe4, 0xde, 0xf7, 0xde, 0xcd, 0xcd, 0xee };
	//首先注册字符串资源文件：将我们的字符串package加入到Hii数据库中
	EFI_HANDLE HiiHandle = HiiAddPackages (&mStrPackageGuid ,  gImageHandle, exampleStrings, NULL); 
	//通过字符串标识符访问字符串。 
	InitShellLib();
	ShellPrintHiiEx(-1,-1,(const CHAR8*)"en-US",STRING_TOKEN (STR_LANGUAGE_SELECT), HiiHandle);  

	TestString(HiiHandle);
	TestLang(HiiHandle);
	return 0;
}

