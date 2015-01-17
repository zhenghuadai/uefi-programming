/*
 * =====================================================================================
 *
 *       Filename:  audio.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2012 08:07:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef  __EFI_AUDIO_H__
#define  __EFI_AUDIO_H__

#ifdef __cplusplus
extern "C"{
#endif
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/PlatformDriverOverride.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Protocol/PciIo.h>
#include <IndustryStandard/Pci22.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>


#define EFI_AUDIO_PROTOCOL_GUID \
{ \
    0xce345171, 0xabcd, 0x11d2, {0x8e, 0x4f, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
}

//
///// Protocol GUID name defined in EFI1.1.
// 
#define AUDIO_PROTOCOL  EFI_AUDIO_PROTOCOL_GUID

typedef struct _EFI_AUDIO_PROTOCOL EFI_AUDIO_PROTOCOL;

//
///// Protocol defined in EFI1.1.

#undef NULL
#define NULL 0
 
/**
  @param  This       Indicates a pointer to the calling context.

  @retval EFI_SUCCESS          . 
  @retval EFI_DEVICE_ERROR         . 

**/
typedef 
EFI_STATUS
(EFIAPI* EFI_AC97_RESET)(
        IN  EFI_AUDIO_PROTOCOL  *This
        );

/**
 *each sample is 4 bytes, the first 2 bytes are left channel, 
 * the second 2 bytes are right channel.
  @param  This       Indicates a pointer to the calling context.
  @param  PcmData    Pointer to PCM Data
  @param  Format     PCM Data Format
  @param  Size       How many bytes in PCM Data

  @retval EFI_SUCCESS          . 
  @retval EFI_DEVICE_ERROR         . 

**/
typedef 
EFI_STATUS
(EFIAPI* EFI_AC97_PLAY)(
        IN  EFI_AUDIO_PROTOCOL  *This,
	IN  UINT8* PcmData,
	IN  UINT32 Format,
	IN  UINTN Size
        );

/**
  @param  This       Indicates a pointer to the calling context.
  @param  Increase   How much should the volume change, 
                     +Number increase; -Number Decrease.
  @param  NewVolume   if *NewVolume >=0 , It will set the volume as *NewVolume;
                      if *NewVolume <0, the Volume will be changed by Increase, 
		      and *Newvolume returns the current Volume.

  @retval EFI_SUCCESS          . 
  @retval EFI_DEVICE_ERROR         . 

**/
typedef 
EFI_STATUS
(EFIAPI* EFI_AC97_VOLUME)(
        IN  EFI_AUDIO_PROTOCOL  *This,
	IN  INT32 Increase,
	IN OUT INT32 * NewVolume
	);

struct _EFI_AUDIO_PROTOCOL{
 UINT64          Revision;
 EFI_AC97_RESET  Reset;
 EFI_AC97_PLAY   Play;
 EFI_AC97_VOLUME Volume;
 EFI_EVENT       WaitForEndEvent;
};

extern EFI_GUID gEfiAudioProtocolGUID;
#ifdef __cplusplus
}//! extern C
#endif

 
 
#endif   /* ----- #ifndef __EFI_AUDIO_H__  ----- */
