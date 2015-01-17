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

extern "C"
{
#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <stdlib.h>
}
#include <vector>

EFI_STATUS
EFIAPI
UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
        )
{
    EFI_STATUS                             Status = 0;
    Print((const CHAR16*)L" 您好UEFI Font");
    return Status;
}
int ga = 0;

class AABAA 
{
    public:
        AABAA(){ 
            ga ++;
            Print((const CHAR16*)L"AABAA\n");
        }
        //~AABAA(){ 
        //    ga ++;
        //    Print((const CHAR16*)L"~AABAA\n");
        //}
        // ~A(){ga --;}
    private:
        int a;
};


class Global
{
  public:
      Global(){Print((const CHAR16*)L"Hello, global constructor!\n");}
};
////////////////////////////////
//
//
//AABAA a;
Global gGlobal;


extern "C" void __do_global_ctors_aux (void);
//
//
//
//
int
cppMain (
        IN int Argc,
        IN char **Argv
        )
{
    //    Print((const CHAR16*)L"Hello CPP%p\n", __CTOR_USER__);
    std::vector<int> a;
    a.push_back(1);
    a.push_back(10);
    Print((const CHAR16*)L"Hello CPP\n");
    for(std::vector<int>::iterator it = a.begin(); it != a.end(); it++)
    {
        Print((const CHAR16*)L"%d\n", *it);
    }
    Print((const CHAR16*)L"ga:%d\n", ga);
    return 0;
}

