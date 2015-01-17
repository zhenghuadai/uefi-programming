#ifndef __HEADER_SHELL_FILE__
#define __HEADER_SHELL_FILE__

#ifdef __cplusplus
extern "C"{
#endif
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EfiShellInterface.h> 
#include <Protocol/EfiShellParameters.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/MemoryAllocationLib.h>

#define malloc(size)               AllocatePool ((UINTN) size)
#define free(ptr)                  FreePool(ptr)

typedef void FILE;

EFI_STATUS
EFIAPI
LocateShellProtocol(
)
{
    EFI_STATUS    Status;
    if(gEfiShellProtocol != NULL) return 0;
    Status = gBS->OpenProtocol(gImageHandle,
                               &gEfiShellProtocolGuid,
                               (VOID **)&gEfiShellProtocol,
                               gImageHandle,
                               NULL,
                               EFI_OPEN_PROTOCOL_GET_PROTOCOL
                              );
    return Status;
}

static __inline FILE* fopen(CHAR8* filename, CHAR8* mode)
{
    EFI_STATUS                      Status;
    FILE*                           LFileHandle = NULL;
    CHAR16*                         FilenameUnicode;
    UINTN                           Index;
    UINT64                          EfiFileMode = 0;

    if(gEfiShellProtocol == NULL) LocateShellProtocol();
    FilenameUnicode = (CHAR16*)malloc( ((AsciiStrLen (filename) + 1) * sizeof (CHAR16)));
    AsciiStrToUnicodeStr(filename,FilenameUnicode);
    for(Index =0 ; Index <AsciiStrLen(mode); Index ++){
        switch (mode[Index])
        {
            case 'r':EfiFileMode |= EFI_FILE_MODE_READ; break;
            case 'w':EfiFileMode |= EFI_FILE_MODE_WRITE; break;
        }
    }
    Status = gEfiShellProtocol ? gEfiShellProtocol->OpenFileByName((CONST CHAR16*)FilenameUnicode, &LFileHandle, EfiFileMode) : EFI_NOT_FOUND;
    if(EFI_ERROR(Status)){
        Print(L"OpenFileByName %s:%r\n", FilenameUnicode, Status);
    }

    free(FilenameUnicode);
    return LFileHandle;
}

static __inline FILE* fcreate(CHAR8* filename, CHAR8* mode)
{
    EFI_STATUS                      Status;
    FILE                            *LFileHandle = NULL;
    CHAR16*                         FilenameUnicode;

    FilenameUnicode = (CHAR16*)malloc( ((AsciiStrLen (filename) + 1) * sizeof (CHAR16)));
    AsciiStrToUnicodeStr(filename,FilenameUnicode);
    Status = gEfiShellProtocol->CreateFile((CONST CHAR16*)FilenameUnicode, EFI_FILE_MODE_WRITE, &LFileHandle);
    if(EFI_ERROR(Status)){
    }
    free(FilenameUnicode);
    return LFileHandle;
}

static __inline size_t __cdecl fread(void* _Str,      size_t _ElementSize,   size_t _Count,   FILE* _File)
{
    EFI_STATUS                      Status;
    UINTN                           BufSize =  _ElementSize * _Count;
    
    Status = gEfiShellProtocol->ReadFile(_File, &BufSize, _Str);
    return BufSize;
}

static __inline size_t __cdecl fwrite( const void * _Str,   size_t _Size,   size_t _Count,   FILE* _File)
{
    EFI_STATUS                      Status;
    UINTN                           BufSize =  _Size * _Count;
    Status = gEfiShellProtocol->WriteFile(_File, &BufSize,(void*) _Str);
    return BufSize;
}

static __inline int __cdecl fclose(FILE* _File)
{
    EFI_STATUS                      Status;
    Status = gEfiShellProtocol->CloseFile(_File);
    return (int)Status;
}

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
static __inline int __cdecl fseek( FILE * _File,   long _Offset,   int _Origin)
{
    EFI_STATUS                      Status;
    INTN                            Abs_Offset = (INTN)_Offset;
    switch (_Origin)
	{
		case SEEK_CUR:
            {
                UINT64 curPos ;
                Status = gEfiShellProtocol->GetFilePosition(_File, &curPos);
                _Offset = _Offset + (long)curPos;
            }
			break;

		case SEEK_END:
			break;

		case SEEK_SET:
			break;
	}
    Status = gEfiShellProtocol->SetFilePosition(_File, Abs_Offset);
    return (int)Status;
}

#ifdef __cplusplus
}
#endif

#endif 
