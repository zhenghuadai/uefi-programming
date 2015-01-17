#ifndef _STD_HEADER_FOR_CPLUSPLUS
#define _STD_HEADER_FOR_CPLUSPLUS

#undef NULL
#define NULL    0 

#ifdef __cplusplus
extern "C"{
#endif
#include <Uefi.h> 
#include <Base.h> 
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#ifdef __UEFI_MACRO_STD__
#elif defined __UEFI_WRAPPER_STD__
void * __cdecl  memcpy(void * _Dst,  const void * _Src,  size_t _Size);
void * memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlen(const char *s);
int strcpy_s(char * _Dst, size_t _SizeInBytes,  const char * _Src);
size_t wcslen(   const wchar_t *str );
int strncmp(const char *s1, const char *s2, size_t n);
char* strstr(const char *s1, const char *s2);
int strcmp(const char *s1, const char *s2);
int wcscmp(const wchar_t *s1, const wchar_t *s2);
int wcsncmp(const wchar_t *s1, const wchar_t *s2, size_t n);
int stricmp(const char *s1, const char *s2);
int _wcsicmp( const wchar_t * _Str1,  const wchar_t * _Str2);
int memicmp(const void * _Buf1, const void * _Buf2,  size_t _Size);
wchar_t *wcsstr(const wchar_t *s1, const wchar_t *s2);
long int atol(const char *nptr);
int rand(void);
void srand(unsigned int seed);
void * malloc(size_t Size);
void free(void *Ptr);
int usleep(unsigned int Microseconds);
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#define va_list  VA_LIST
#define va_start VA_START
#define va_end   VA_END
#define va_arg   VA_ARG

int _vscwprintf(const wchar_t * _Format, va_list _ArgList);

int _vswprintf_s( wchar_t * _Dst, size_t _SizeInWords, const wchar_t * _Format, va_list _ArgList);

#ifdef UNICODE
#  define _vsctprintf _vscwprintf
#  define _vstprintf_s _vswprintf_s
#  define __T(x)      L ## x
#else
#  define __T(x)      
#endif
#define _T(x)       __T(x)


/**  UefiStdWrapper@} */

#ifdef __cplusplus
}// end of extern "C"
#endif


#endif
