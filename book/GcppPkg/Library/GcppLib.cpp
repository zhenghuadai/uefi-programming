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
}

typedef UINTN size_t;
void *  operator new( size_t Size )
{
    void       *RetVal;
    EFI_STATUS  Status;

    if( Size == 0) {
        return NULL;
    }

    Status = gBS->AllocatePool( EfiLoaderData, (UINTN)Size, &RetVal);
    if( Status != EFI_SUCCESS) {
        RetVal  = NULL;
    }
    return RetVal;
}


void *  operator new[]( size_t cb )
{
    void *res = operator new(cb);
    return res;
}



void operator delete( void * p )
{ 
  if(p != NULL) 
    (void) gBS->FreePool (p);
}

void operator delete[]( void * p )
{
    operator delete(p);
}

    extern "C"
void _Unwind_Resume(struct _Unwind_Exception *object)
{

}

#if defined(__GNUC__) 
#include "GcppCrt.cpp"
#elif defined(_MSC_VER)
#include "VcppCrt.cpp"
#endif
