#include "fbext.h"
extern "C" void PrintNum(CHAR16* str, UINT64 num);
#define PCI_DEVICE_ENABLED  (EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE) 
static 
BOOLEAN
BiosVideoIsVga (
  IN  EFI_PCI_IO_PROTOCOL       *PciIo
  )
{
  EFI_STATUS    Status;
  BOOLEAN       VgaCompatible;
  PCI_TYPE00    Pci;

  VgaCompatible = FALSE;

  //
  // Read the PCI Configuration Header
  //
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        0,
                        sizeof (Pci) / sizeof (UINT32),
                        &Pci
                        );
  if (EFI_ERROR (Status)) {
    return VgaCompatible;
  }

  //
  // See if this is a VGA compatible controller or not
  //
  if ((Pci.Hdr.Command & PCI_DEVICE_ENABLED) == PCI_DEVICE_ENABLED) {
    if (Pci.Hdr.ClassCode[2] == PCI_CLASS_OLD && Pci.Hdr.ClassCode[1] == PCI_CLASS_OLD_VGA) {
      //
      // Base Class 0x00 Sub-Class 0x01 - Backward compatible VGA device
      //
      VgaCompatible = TRUE;
    }

    if (Pci.Hdr.ClassCode[2] == PCI_CLASS_DISPLAY && Pci.Hdr.ClassCode[1] == PCI_CLASS_DISPLAY_VGA && Pci.Hdr.ClassCode[0] == 0x00) {
      //
      // Base Class 3 Sub-Class 0 Programming interface 0 - VGA compatible Display controller
      //
      VgaCompatible = TRUE;
    }
  }

  return VgaCompatible;
}

static EFI_STATUS GetVideoController(EFI_HANDLE* VideoHandle, EFI_PCI_IO_PROTOCOL** VideoPciIo)
{
    EFI_STATUS Status;
    EFI_HANDLE  *ControllerHandle = NULL;    
    UINTN        i,  NumHandles;


    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiPciIoProtocolGuid,
        NULL,
        &NumHandles,
        &ControllerHandle);
     for( i =0; i<NumHandles; i++){
        EFI_PCI_IO_PROTOCOL* PciIo ;
        Status = gBS->HandleProtocol(
            ControllerHandle[i],
            &gEfiPciIoProtocolGuid,
            (VOID**)&PciIo
            );
        if(!EFI_ERROR(Status)){
            if(BiosVideoIsVga(PciIo)){
                if(VideoHandle)
                    *VideoHandle = ControllerHandle[i];
                *VideoPciIo = PciIo;
            } 
        }
    }
    if(ControllerHandle) gBS->FreePool(ControllerHandle);
    return Status;
}
 


namespace UGUI{

////////////////////////////////////////////////////////////////////////
EFI_STATUS ManuelFrameBuffer::Blt(
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
                      )
{
    EFI_STATUS Status = 0;
        if(m_pixel_bits == 32){
            UINT32 row = 0;
            UINT64 DstAddress = m_framebuffer_base  + 4*(DestinationY*m_line_pixels + DestinationX);
            UINT64 SrcAddress = ((UINT64)BltBuffer) + 4*(SourceY*m_line_pixels + SourceX);

            for(row =0; row < Height; row ++){
                Status = m_VideoPciIo->Mem.Write(m_VideoPciIo,
                    EfiPciIoWidthUint32,
                    EFI_PCI_IO_PASS_THROUGH_BAR,
                    (UINT64) (DstAddress),
                    Width,
                    (UINT8*)SrcAddress);
                DstAddress +=(m_line_pixels * 4);
                SrcAddress +=(m_line_pixels * 4);
            }
        }else{
        }

    return Status;
}

EFI_STATUS ManuelFrameBuffer::UpdateScreen(EFI_GRAPHICS_OUTPUT_BLT_PIXEL* bitmap, RECT aRect)
{
    EFI_STATUS Status= 0;
    UINT32 PixelBytes ;
    if(Brick::ToDisplay == false)
        return Status;

    UINTN Width  = (aRect.W< m_frame_width? aRect.W:m_frame_width);
    UINTN Height = (aRect.H< m_frame_height?aRect.H:m_frame_height);
    UINTN StartX = (aRect.X< m_frame_width? aRect.X:0);
    UINTN StartY = (aRect.Y< m_frame_height?aRect.Y:0);

    if(Width == 0)  Width = m_frame_width;
    if(Height == 0) Height = m_frame_height;

        if(m_pixel_bits == 32){
            if(Width == m_frame_width){
            Status = m_VideoPciIo->Mem.Write(m_VideoPciIo,
                EfiPciIoWidthUint8,
                EFI_PCI_IO_PASS_THROUGH_BAR,
                (UINT64) (m_framebuffer_base),
                m_frame_width*m_frame_height*(4),
                (UINT8*)bitmap);
            }else{
                Blt(m_GraphicsOutput, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)bitmap, EfiBltBufferToVideo, StartX, StartY, StartX, StartY, Width, Height, 0);
            }

        }else{
            PixelBytes =m_pixel_bits /8;
            for(UINT32 i =0; i< m_frame_width*m_frame_height; i++){
                m_Frambuffer[PixelBytes*i] = bitmap[i].Blue;
                m_Frambuffer[PixelBytes*i+1] = bitmap[i].Green;
                m_Frambuffer[PixelBytes*i+2] = bitmap[i].Red;
            }
            Status = m_VideoPciIo->Mem.Write(m_VideoPciIo,
                EfiPciIoWidthUint8,
                EFI_PCI_IO_PASS_THROUGH_BAR,
                (UINT64) (m_framebuffer_base),
                m_frame_width*m_frame_height*(4),
                (UINT8*)m_Frambuffer);
        }
 
    return Status;
}

EFI_STATUS ManuelFrameBuffer::AutoInit()
{
    EFI_STATUS Status = 0;
    m_framebuffer_base  = m_GraphicsOutput->Mode->FrameBufferBase;
    UINT32 m_framebuffer_bytes = (UINT32)m_GraphicsOutput->Mode->FrameBufferSize;
    if(m_framebuffer_bytes)
        m_pixel_bits      =  m_framebuffer_bytes/(m_frame_width*m_frame_height) ; 
    else
        m_pixel_bits      =  32; 

    m_line_pixels =  (UINT32)m_GraphicsOutput->Mode->Info->PixelsPerScanLine;  

    if( m_GraphicsOutput->Mode->Info->PixelFormat != PixelBltOnly){
        if(EQ2(gST->FirmwareVendor, 'H','P')){
            if(m_framebuffer_base == 0)
                m_framebuffer_base = HPQ_FRAMEBUFFER_BASE;
            m_pixel_bits = 32;
        }else  if(EQ4(gST->FirmwareVendor,'Q', 'E', 'M', 'U')){
            m_framebuffer_base = QEMU_FRAMEBUFFER_BASE; 
            m_pixel_bits = 24;
        }else{
        }
    }else{
	    m_framebuffer_base = 0;
    }

    GetVideoController(NULL, &m_VideoPciIo);
    m_Frambuffer = new UINT8[m_line_pixels*m_frame_height*(m_pixel_bits/8)];
    return Status;
}

ManuelFrameBuffer::~ManuelFrameBuffer()
{
    if(m_Frambuffer) delete m_Frambuffer;
}

}//namespace UGUI{
