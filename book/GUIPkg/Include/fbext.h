#ifndef _FRAMEBUFFER_EXT_HEADER
#define _FRAMEBUFFER_EXT_HEADER
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
#include <fb.h>
namespace UGUI{


class ManuelFrameBuffer: public UFrameBuffer
{
    public:
        ManuelFrameBuffer():UFrameBuffer(),
        m_framebuffer_base(0),
        m_pixel_bits(0),
        m_VideoPciIo(0),
        m_Frambuffer(0) 
    {
        AutoInit();
    }
        virtual ~ManuelFrameBuffer();
    private:
        BOOLEAN UseFrameBuffer(){        return (m_framebuffer_base != 0);    }
        EFI_STATUS AutoInit();
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
    private:
        UINT64  m_framebuffer_base;
        UINT32  m_pixel_bits;
        UINT32  m_line_pixels;
        EFI_PCI_IO_PROTOCOL                  *m_VideoPciIo;
        UINT8                                *m_Frambuffer;
};

}
#endif
