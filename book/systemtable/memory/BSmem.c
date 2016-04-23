#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>


EFI_STATUS    TestMMap()
{
    EFI_STATUS Status = 0;
    UINTN                       MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR       *MemoryMap = 0;
    UINTN                       MapKey = 0;
    UINTN                       DescriptorSize = 0;
    UINT32                      DescriptorVersion = 0;
    UINTN                       i;
    EFI_MEMORY_DESCRIPTOR       *MMap = 0;
    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if(Status != EFI_BUFFER_TOO_SMALL){
        return Status;
    }
    Status = gBS->AllocatePool(EfiBootServicesData, MemoryMapSize, (void**)&MemoryMap);
    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    for( i = 0; i< MemoryMapSize / (DescriptorSize); i++)
    {
        MMap = (EFI_MEMORY_DESCRIPTOR*) ( ((CHAR8*)MemoryMap) + i * DescriptorSize);
        Print(L"MemoryMap %4d %10d :", MMap[0].Type, MMap[0].NumberOfPages);
        Print(L"%10lx<->", MMap[0].PhysicalStart);
        Print(L"%10lx\n",   MMap[0].VirtualStart);
    }
    Status = gBS->FreePool(MemoryMap);
    return Status;
}

EFI_STATUS TestAllocateAddress()
{
    EFI_STATUS Status = 0;
    EFI_PHYSICAL_ADDRESS pages = 1024 *1024 * 10;
    Status = gBS->AllocatePages(AllocateAddress, EfiBootServicesData, 3, &pages);
    Print(L"AllocateAddress:%r %x\n", Status, pages);
    if(Status == 0){
        CHAR16* str;
        str = (CHAR16*) pages;
        str[0] = 'H';
        str[1] = 0;
        Print(L"Test Str:%s\n", str);
        Status = gBS->FreePages(pages, 3);
    }else {
    } 
    return Status;
}

EFI_STATUS TestAllocateAnyPages()
{
    EFI_STATUS Status = 0;
    EFI_PHYSICAL_ADDRESS pages;
    Status = gBS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 3, &pages);
    Print(L"AllocatePages:%r %x\n", Status, pages);
    if(Status == 0){
        CHAR16* str;
        str = (CHAR16*) pages;
        str[0] = 'H';
        str[1] = 0;
        Print(L"Test Str:%s\n", str);
        Status = gBS->FreePages(pages, 3);
    }else {
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
    EFI_STATUS Status = 0;
    Status = TestAllocateAnyPages();
    Status = TestAllocateAddress();
    Status = TestMMap();
    return Status;
}
