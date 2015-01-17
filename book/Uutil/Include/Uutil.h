#define SAFECALL(x) if(EFI_SUCCESS != (x)){return Status;}
#define HANDLE_ERR(x) if(EFI_SUCCESS !=(x))
#define IF_ERR(x) if(EFI_SUCCESS != (x))
#define CheckStatus(x) if(EFI_SUCCESS != (Status)){if(((VOID*)(x)) != NULL){Print(L"%s %r", x, Status);} return Status;}

#define RETURN_ERR(x) SAFECALL(x)
#define BREAK_ERR(x) if(EFI_SUCCESS != (x)){break;}

#ifdef _cplusplus
#undef NULL
#define NULL 0
#endif


// File op

#define FOpen(FileHandle, NewHandle, FileName, OpenMode, Attr) (FileHandle) -> Open(FileHandle,  NewHandle, FileName, OpenMode, Attr)
#define FRead(FileHandle, BufferSize, Buffer) (FileHandle) -> Read(FileHandle, BufferSize, Buffer)


// end File op
