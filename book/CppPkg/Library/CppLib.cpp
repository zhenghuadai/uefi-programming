/*
 * =====================================================================================
 *
 *       Filename:  GcppLib.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/13/2013 10:35:14 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
extern"C"
{
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>

}

    extern "C"
void _Unwind_Resume(struct _Unwind_Exception *object)
{

}

extern "C" int __cdecl sprintf_s(char * _DstBuf,  size_t _SizeInBytes,  const char * _Format, ...)
{
    int ret;
    VA_LIST ap;
    VA_START(ap, _Format);
    ret = (int)AsciiVSPrint(_DstBuf, _SizeInBytes, _Format, ap);
    return (ret);
}

