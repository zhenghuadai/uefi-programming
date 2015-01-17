/** @file
**/
#include "disk.h" 
#include <Protocol/HiiString.h> 
 
void DiskDevice::listInfo()
{
    if(BlockIo->Media == 0)
        return;
    Print(L"MediaId:%0x\nRemovableMedia:%0x\nMediaPresent:%0x\nLogicalPartition:%0x\nReadOnly:%0x\nWriteCaching:%0x\nBlockSize:%0x\nIoAlign:%0x\nLastBlock:%0x\nLowestAlignedLba:%0x\nLogicalBlocksPerPhysicalBlock:%0x\nOptimalTransferLengthGranularity:%0x\n",
  BlockIo->Media->MediaId,            
  BlockIo->Media->RemovableMedia,   
  BlockIo->Media->MediaPresent,      
  BlockIo->Media->LogicalPartition, 
  BlockIo->Media->ReadOnly,          
  BlockIo->Media->WriteCaching,     
  BlockIo->Media->BlockSize,       
  BlockIo->Media->IoAlign, 
  BlockIo->Media->LastBlock,  
  BlockIo->Media->LowestAlignedLba,
  BlockIo->Media->LogicalBlocksPerPhysicalBlock,
  BlockIo->Media->OptimalTransferLengthGranularity
      );


}
