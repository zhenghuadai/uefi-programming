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
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Timer.h>
#include <Pi/PiDxeCis.h>
#include <Protocol/Cpu.h>
#include <Protocol/SmmBase2.h>
#include "SimpleThread.h"

UINTN gStep = 0;
void thread0(void* arg )
{
    UINTN step = 0;
    while(step<20)
    {
        step ++;
        gStep++;
        Print(L"THREAD 0 : %d ;gStep:%d\n", step, gStep);
        gBS->Stall(1000* 1000);
    }
}


void thread1(void*arg )
{
    UINTN step = 0;
    while(step < 6 + (UINTN)arg * 20)
    {
        step ++;
        gStep++;
        Print(L"This is thread %d : %d; gStep:%d\n", arg, step, gStep);
        gBS->Stall(1000 * 1000);
    }
}

EFI_GUID gEfiSimpleThreadProtocolGUID = EFI_SIMPLETHREAD_PROTOCOL_GUID ;

EFI_STATUS
EFIAPI
UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
        )
{
    EFI_STATUS  Status;
    EFI_SIMPLETHREAD_PROTOCOL * SimpleThread; 
    Status = gBS->LocateProtocol(&gEfiSimpleThreadProtocolGUID , 0, (VOID**)&SimpleThread);
    SimpleThread ->  create_thread(SimpleThread, thread1, (void*) 1);
    SimpleThread ->  create_thread(SimpleThread, thread1, (void*) 2);
    thread0(0);
    SimpleThread ->  thread_join(SimpleThread );
    return 0;
}
