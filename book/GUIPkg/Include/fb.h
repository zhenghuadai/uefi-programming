#ifndef _FRAMEBUFFER_HEADER
#define _FRAMEBUFFER_HEADER
#ifdef __cplusplus
extern "C"{
#endif
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Guid/MdeModuleHii.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/DebugLib.h>
#include <Protocol/PciIo.h>
#include <IndustryStandard/Pci22.h>
 
#ifdef __cplusplus
}//! extern C
#endif
#include <UGui.h>
namespace UGUI{
const UINT64 HPQ_FRAMEBUFFER_BASE  = 0xD1000000;
const UINT64 QEMU_FRAMEBUFFER_BASE = 0xF0000000;

class UFrameBuffer
{
public:
        static UFrameBuffer* Create(int t);
public:
    UFrameBuffer():
        m_frame_width(0),
        m_frame_height(0),
        m_GraphicsOutput(0)
    {Init();}
    virtual ~UFrameBuffer();
    EFI_STATUS Init();

    EFI_STATUS UpdateFullScreen(EFI_GRAPHICS_OUTPUT_BLT_PIXEL* bitmap){
        return UpdateScreen(bitmap, RECT(0,0,(UINT16)m_frame_width,(UINT16)m_frame_height));
    };
public:
    virtual EFI_STATUS UpdateScreen(EFI_GRAPHICS_OUTPUT_BLT_PIXEL* bitmap, RECT aRect = Brick::ClientRect);
    virtual EFI_STATUS Blt(
            IN EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
            IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer, OPTIONAL
            IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,
            IN UINTN SourceX,
            IN UINTN SourceY,
            IN UINTN DestinationX,
            IN UINTN DestinationY,
            IN UINTN Width,
            IN UINTN Height,
            IN UINTN Delta OPTIONAL
          );
public:
// SourceX and SourceY is 0
static void SetToFrameBuffer(
        UEFI_PIXEL* FrameBuffer, UINT16 FrameBufferWidth, UINT16 StartX, UINT16 StartY, UINT16 Width, UINT16 Height,UEFI_PIXEL Color);

// SourceX and SourceY is 0
#define UTIL_PARAMETERS_LIST_0  OUT UEFI_PIXEL* FrameBuffer, IN UINT16 FrameBufferWidth, \
        IN UEFI_PIXEL* Buffer, IN UINT16 SrcStride, \
        IN UINT16 SrcX, IN UINT16 SrcY, \
        IN UINT16 StartX, IN UINT16 StartY, \
        IN UINT16 Width, IN UINT16 Height

#define UTIL_PARAMETERS_LIST_1 UEFI_PIXEL* FrameBuffer, UINT16 FrameBufferWidth, \
        bmp_t* bmp, UINT16 SrcStride, \
        UINT16 SrcX, UINT16 SrcY, \
        UINT16 DstX, UINT16 DstY, \
        UINT16 Width, UINT16 Height
static void CopyToFrameBuffer( UTIL_PARAMETERS_LIST_0);
static void MaskedCopyToFrameBuffer(UTIL_PARAMETERS_LIST_0);
static EFI_STATUS DrawtoFrameBuffer(UTIL_PARAMETERS_LIST_1);
static EFI_STATUS MaskedDrawtoFrameBuffer(UTIL_PARAMETERS_LIST_1, int adjust = 0);
/**
 DestinationX and DestinationY is 0
 Copy a sub region(StartX,StartY, Width, Height) 
 from FrameBuffer to Buffer(0, 0, Width,Height)
**/
static void CopyFromFrameBuffer(IN UEFI_PIXEL* FrameBuffer, IN UINT16 FrameBufferWidth, OUT UEFI_PIXEL* Buffer, IN UINT16 SrcStride, IN UINT16 StartX, IN UINT16 StartY, IN UINT16 Width, IN UINT16 Height);

protected:
    UINT32  m_frame_width;
    UINT32  m_frame_height;
    EFI_GRAPHICS_OUTPUT_PROTOCOL         *m_GraphicsOutput;
};

}
#endif
