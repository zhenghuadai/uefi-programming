#ifndef __UEFI_MACRO_STD__
#define __UEFI_MACRO_STD__
#ifdef __cplusplus
extern "C"{
#endif

#include <Library\MemoryAllocationLib.h>
#include <Library\BaseMemoryLib.h>
#include <Library\BaseLib.h>

#define malloc(size)               AllocatePool ((UINTN) size)
#define free(ptr)                  FreePool(ptr)
#define memcpy(dest,source,count)  CopyMem(dest,source,(UINTN)(count))
#define memset(dest,ch,count)      gBS->SetMem(dest,(UINTN)(count),(UINT8)(ch))
#define memchr(buf,ch,count)       ScanMem8(buf,(UINTN)(count),(UINT8)ch)
#define memcmp(buf1,buf2,count)    (int)(CompareMem(buf1,buf2,(UINTN)(count)))
#define memmove(dest,source,count) CopyMem(dest,source,(UINTN)(count))

#define strcmp                            AsciiStrCmp
#define strncmp(string1,string2,count)    (int)(AsciiStrnCmp(string1,string2,(UINTN)(count)))
#define strcpy(strDest,strSource)         AsciiStrCpy(strDest,strSource)
#define strncpy(strDest,strSource,count)  AsciiStrnCpy(strDest,strSource,(UINTN)count)
#define strlen(str)                       (size_t)(AsciiStrLen(str))
#define strcat(strDest,strSource)         AsciiStrCat(strDest,strSource)
#define strchr(str,ch)                    ScanMem8((VOID *)(str),AsciiStrSize(str),(UINT8)ch)
#define strstr(text, pattern)             AsciiStrStr(text, pattern)

#define wcscmp(s1, s2)                    StrCmp( (CONST CHAR16 *)s1, (CONST CHAR16 *)s2)
#define wcsstr(text, pattern)             StrStr(text, pattern)

#define abort()                           ASSERT (FALSE)
#define assert(expression)
#define localtime(timer)                  NULL
#define gmtime_r(timer,result)            (result = NULL)

#ifdef __cplusplus
}
#endif

#endif
