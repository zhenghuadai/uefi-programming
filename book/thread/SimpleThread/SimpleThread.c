/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2013 08:22:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <Uefi.h>
#include <Protocol/HiiFont.h>
#include "Protocol/HiiDatabase.h"
#include <Library/UefiHiiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "dmthread.h"
#include "SimpleThread.h"

typedef struct {
    UINTN                 Signature;
    EFI_SIMPLETHREAD_PROTOCOL  SimpleThread; 
} SIMPLETHREAD_PRIVATE_DATA;

#define SIMPLETHREAD_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('T', 'R', 'E', 'D')

#define SIMPLETHREAD_PRIVATE_DATA_FROM_THIS(a) CR (a, SIMPLETHREAD_PRIVATE_DATA, SIMPLETHREAD, SIMPLETHREAD_PRIVATE_DATA_SIGNATURE)
static SIMPLETHREAD_PRIVATE_DATA gSIMPLETHREADPrivate;
EFI_GUID gEfiSimpleThreadProtocolGUID = EFI_SIMPLETHREAD_PROTOCOL_GUID ;

EFI_STATUS
EFIAPI 
CreateThread(
        IN  EFI_SIMPLETHREAD_PROTOCOL  *This,
        IN  THREAD_FUNC_T Thread,
        IN  VOID *  Arg
        )
{
     create_thread(Thread, Arg);
     return 0;
}

EFI_STATUS
EFIAPI
JoinThread(
        IN EFI_SIMPLETHREAD_PROTOCOL* This
        )
{
    //SIMPLETHREAD_PRIVATE_DATA* Private;
    //Private = SIMPLETHREAD_PRIVATE_DATA_FROM_THIS(This);
    //(void) Private;
    thread_join();
    return 0;
}


EFI_STATUS
EFIAPI
UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
        )
{
    EFI_STATUS Status;
    SIMPLETHREAD_PRIVATE_DATA* Private = &gSIMPLETHREADPrivate;
    gSIMPLETHREADPrivate.Signature = SIMPLETHREAD_PRIVATE_DATA_SIGNATURE  ;
    gSIMPLETHREADPrivate.SimpleThread.create_thread = CreateThread;
    gSIMPLETHREADPrivate.SimpleThread.thread_join = JoinThread;
    //Print(L"%a: %d \n", __func__, __LINE__);


    Status = gBS->InstallProtocolInterface (
            &ImageHandle,
            &gEfiSimpleThreadProtocolGUID,
            EFI_NATIVE_INTERFACE,
            &Private->SimpleThread
            );

    return 0;
}
