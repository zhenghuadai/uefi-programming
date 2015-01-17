/*
 * =====================================================================================
 *
 *       Filename:  SimpleThread.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/08/2013 07:49:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __SIMPLETHREADER_HEADER__
#define __SIMPLETHREADER_HEADER__
#include <Uefi.h> 
#include <Base.h> 

#define EFI_SIMPLETHREAD_PROTOCOL_GUID \
{ \
    0xda445171, 0xabcd, 0x11d2, {0x8e, 0x4f, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
}

//
///// Protocol GUID name defined in EFI1.1.
// 
#define SIMPLETHREAD_PROTOCOL  EFI_SIMPLETHREAD_PROTOCOL_GUID

typedef struct _EFI_SIMPLETHREAD_PROTOCOL EFI_SIMPLETHREAD_PROTOCOL;

//
///// Protocol defined in EFI1.1.
// 
typedef EFI_SIMPLETHREAD_PROTOCOL  EFI_SIMPLETHREAD;

typedef void ( * THREAD_FUNC_T)(void * );
/**
  

  @param  This       Indicates a pointer to the calling context.
  @param  
  @param  

  @retval EFI_SUCCESS          Return the Size successfully. 
  @retval EFI_NO_MEDIA         There is no opened video. 

**/
typedef 
EFI_STATUS
(EFIAPI* EFI_CREATE_THREAD  )(
        IN  EFI_SIMPLETHREAD_PROTOCOL  *This,
        IN  THREAD_FUNC_T Thread,
        IN  VOID *  Arg
        );


typedef 
EFI_STATUS
(EFIAPI* EFI_JOIN_THREAD  )(
        IN  EFI_SIMPLETHREAD_PROTOCOL  *This
        );

struct _EFI_SIMPLETHREAD_PROTOCOL{
    UINT64          Revision;
    EFI_CREATE_THREAD  create_thread;
    EFI_JOIN_THREAD  thread_join;
};


extern EFI_GUID gEfiSimpleThreadProtocolGUID;

#endif // __SIMPLETHREADER_HEADER__
