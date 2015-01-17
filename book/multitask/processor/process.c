/** @file
**/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include "Pi/PiDxeCis.h"
#include "Protocol/MpService.h"
#include <Library/UefiBootServicesTableLib.h>

EFI_HANDLE mImageHandle = 0;
EFI_MP_SERVICES_PROTOCOL* mpp;

// FRAMEWORK_EFI_MP_SERVICES_PROTOCOL
// IntelFrameworkPkg\Include\Protocol\FrameworkMpService.h
EFI_STATUS
testMPP()
{
	EFI_STATUS Status;
	EFI_MP_SERVICES_PROTOCOL* mpp;
    Status = gBS->LocateProtocol( &gEfiMpServiceProtocolGuid, NULL, (void**)&mpp);
	Print(L"Locate MPP :%r\n", Status);
	return Status;
}

EFI_STATUS LocateMPP(EFI_MP_SERVICES_PROTOCOL** mpp)
{
    return  gBS->LocateProtocol( &gEfiMpServiceProtocolGuid, NULL, (void**)mpp);
}

EFI_STATUS testMPPInfo()
{
	EFI_STATUS Status;
    {
        UINTN nCores = 0, nRunning = 0;
        Status = mpp -> GetNumberOfProcessors(mpp, & nCores, &nRunning);
        Print(L"System has %d cores, %d cores are running\n", nCores, nRunning);
        {
           UINTN i = 0;
           for(i =0; i< nCores; i++){
               EFI_PROCESSOR_INFORMATION mcpuInfo;
               Status = mpp -> GetProcessorInfo( mpp, i, &mcpuInfo);
               Print(L"CORE %d:\n", i);
               Print(L"  ProcessorId\t:%d\n", mcpuInfo.ProcessorId);
               Print(L"  StatusFlag\t:%x\n", mcpuInfo.StatusFlag);
               Print(L"  Location\t:(%d %d %d)\n", mcpuInfo.Location.Package, mcpuInfo.Location.Core, mcpuInfo.Location.Thread);
           }
        }
    }
	return Status;
}

typedef struct {
    UINTN id;
    UINTN loops;
    UINTN ret;
}APParam;

VOID EFIAPI APFoo(VOID* context)
{
    APParam *param = (APParam*) context;
    UINTN i = 0;
    for(i=0;i< param->loops; i++){
        gBS->Stall(1000);
        Print(L"AP %d runs %d\n", param->id, i);
    }
    param -> ret = param -> loops * 10;
}

EFI_STATUS testMPPStartup()
{
	EFI_STATUS Status;

    {
        BOOLEAN Finshed = FALSE;
        APParam APParam1 = {1, 20, 0};
        Status = mpp -> StartupThisAP ( mpp,
                APFoo,
                1,
                NULL,
                0,
                (VOID*)&APParam1,
                &Finshed);
        Print(L"StatupThisAP :%r\n", Status);
        Print(L"AP %s and return %d\n", Finshed == TRUE?L"Finished":L"failed", APParam1.ret);
    }

    
    {
        BOOLEAN Finshed = FALSE;
        APParam APParam0 = {0, 5, 0};
        APParam APParam1 = {1, 20, 0};
        EFI_EVENT ApEvent;
        UINTN Index;
        Status = gBS -> CreateEvent(0, TPL_CALLBACK, NULL, NULL, &ApEvent);
        Status = mpp -> StartupThisAP ( mpp,
                APFoo,
                1,
                ApEvent,
                0,
                (VOID*)&APParam1,
                &Finshed);
        Print(L"StatupThisAP :%r\n", Status);
        if(Status == 0){
            APFoo((VOID*)&APParam0);
            gBS->WaitForEvent(1, &ApEvent, &Index);
            Print(L"AP %s and return %d\n", Finshed == TRUE?L"Finished":L"failed", APParam1.ret);
        }

    }
	return Status;
}
EFI_STATUS
EFIAPI
UefiMain (
          IN EFI_HANDLE        ImageHandle,
          IN EFI_SYSTEM_TABLE  *SystemTable
          )
{    
    EFI_STATUS  Status;
    Status = LocateMPP(&mpp);
	Print(L"Locate MPP :%r\n", Status);
    Status = testMPPInfo();
    Status = testMPPStartup();
    return EFI_SUCCESS;
}

