#include <Uefi.h>
#include <Library\BaseMemoryLib.h>
#include <Library\MemoryAllocationLib.h>
#include <Library\BaseLib.h>

#pragma function(memcpy)
void * memcpy ( void * dest, const void * source, size_t count)
{
    return CopyMem(dest,source,(UINTN)(count));
}
