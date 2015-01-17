#include "fb.h"

namespace UGUI{

inline void bmp2Uefi(UEFI_PIXEL& d, rgb_t& s){ d.Blue = s.B; d.Green = s.G; d.Red = s.R;}

UFrameBuffer* UFrameBuffer::Create(int t)
{
	return new UFrameBuffer();
}

EFI_STATUS UFrameBuffer::Blt(
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
    Status = m_GraphicsOutput->Blt(This, BltBuffer, BltOperation, SourceX, SourceY, DestinationX, DestinationY, Width, Height, Delta);
    return Status;
}

EFI_STATUS UFrameBuffer::UpdateScreen(EFI_GRAPHICS_OUTPUT_BLT_PIXEL* bitmap, RECT aRect)
{
    EFI_STATUS Status= 0;
    if(Brick::ToDisplay == false)
        return Status;

    UINTN Width  = (aRect.W< m_frame_width? aRect.W:m_frame_width);
    UINTN Height = (aRect.H< m_frame_height?aRect.H:m_frame_height);
    UINTN StartX = (aRect.X< m_frame_width? aRect.X:0);
    UINTN StartY = (aRect.Y< m_frame_height?aRect.Y:0);

    if(Width == 0)  Width = m_frame_width;
    if(Height == 0) Height = m_frame_height;

    Status = m_GraphicsOutput->Blt(m_GraphicsOutput, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)bitmap, EfiBltBufferToVideo, StartX, StartY, StartX, StartY, Width, Height, m_frame_width*4);
    (VOID)Width;
    (VOID)Height;
    (VOID)StartX;
    (VOID)StartY;
    return Status;
}
#define EQ2(s, a, b) (s[0] == a && s[1] == b)
#define EQ4(s, a, b,c,d) (s[0] == a && s[1] == b && s[2] == c && s[3] == d)
EFI_STATUS UFrameBuffer::Init()
{
    EFI_STATUS Status;
    Status = gBS->LocateProtocol(
            &gEfiGraphicsOutputProtocolGuid,
            NULL,
            (VOID **)&m_GraphicsOutput);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    m_frame_width    = (UINT32)m_GraphicsOutput->Mode->Info->HorizontalResolution ;
    m_frame_height   = (UINT32)m_GraphicsOutput->Mode->Info->VerticalResolution;
    return EFI_SUCCESS;
}

UFrameBuffer::~UFrameBuffer()
{
}

/**
 * SourceX and SourceY is 0
 **/
void UFrameBuffer::SetToFrameBuffer(UEFI_PIXEL* FrameBuffer, UINT16 FrameBufferWidth, UINT16 startX, UINT16 startY, UINT16 Width, UINT16 Height,UEFI_PIXEL Color)
{
    UEFI_PIXEL *pDst = FrameBuffer + FrameBufferWidth * startY + startX;
    for(UINT16 row =0; row < Height; row ++, pDst += FrameBufferWidth){
        for(UINT16 col=0; col < Width; col++){
            pDst[col] = Color;           
        }
    }
}

/**
 * SourceX and SourceY is 0
 **/
void UFrameBuffer::CopyToFrameBuffer(
        UEFI_PIXEL* FrameBuffer, UINT16 FrameBufferWidth, 
        UEFI_PIXEL* Buffer, UINT16 SrcStride, 
        UINT16 SrcX, UINT16 SrcY, 
        UINT16 startX, UINT16 startY, 
        UINT16 Width, UINT16 Height)
{
    UEFI_PIXEL *pSrc = Buffer, *pDst;
    pDst = FrameBuffer + FrameBufferWidth * startY + startX;
    for(UINT16 row =0; row < Height; row ++,  pDst += FrameBufferWidth,  pSrc += SrcStride){
        for(UINT16 col=0; col < Width; col++){
            pDst[col] = pSrc[col];           
        }
    }
}

/**
 * SourceX and SourceY is 0
 **/
#define each_pixel(SW, DW) \
    for(UINT16 row =0; row < Height; row ++, pSrc+=(SW), pDst+=(DW)){ \
        for(UINT16 col=0; col < Width; col++)

void UFrameBuffer::MaskedCopyToFrameBuffer(
        UEFI_PIXEL* FrameBuffer, UINT16 FrameBufferWidth, 
        UEFI_PIXEL* Buffer, UINT16 SrcStride, 
        UINT16 SrcX, UINT16 SrcY, 
        UINT16 startX, UINT16 startY, 
        UINT16 Width, UINT16 Height)
{
    UEFI_PIXEL *pSrc = 0, *pDst = 0;
    pSrc = Buffer;
    pDst = FrameBuffer + FrameBufferWidth * startY + startX;
    each_pixel(SrcStride, FrameBufferWidth){
            if((*(UINT32*)&(pSrc[col])) != 0)
                pDst[col] = pSrc[col];           
        }
    }
}

EFI_STATUS UFrameBuffer::DrawtoFrameBuffer(
        UEFI_PIXEL* FrameBuffer, UINT16 FrameBufferWidth, 
        bmp_t* bmp, UINT16 SrcStride, 
        UINT16 SrcX, UINT16 SrcY, 
        UINT16 DstX, UINT16 DstY, 
        UINT16 Width, UINT16 Height)
{
    rgb_t* data =(rgb_t*) bmp->data ;
    size_t lwidth = real_width(bmp->width*3);
    rgb_t* dataEnd = (rgb_t*)(((char*)(bmp->data )) +( bmp->height * lwidth ));    
    data = (rgb_t*)(((char*)(bmp->data )) +( SrcY * lwidth + 3*SrcX));
    if(Width == 0) Width = (UINT16)(FrameBufferWidth <  bmp->width? FrameBufferWidth:bmp->width);
    if(bmp->width <  Width) Width = (UINT16)bmp->width;
    if(bmp->height <  Height) Height = (UINT16)bmp->height;
    UEFI_PIXEL* pFrameBuffer = FrameBuffer + DstY*FrameBufferWidth + DstX;
    for(UINT16 row =0; row < Height; row ++){        
        for(UINT16 col=0; col < Width; col++){
            bmp2Uefi(pFrameBuffer[col], data[col] );
        }
        data  = (rgb_t*)(((char*)(data)) + lwidth);
        if(data + Width > dataEnd)break;
        pFrameBuffer += FrameBufferWidth;
    }    
    (void)dataEnd;
    return EFI_SUCCESS;
}
#define THRESHHOLD(x) (~((((unsigned int)x<< 24)) | (((unsigned int)x<<  16)) | (((unsigned int)x<< 8)) | (unsigned int)x))


EFI_STATUS UFrameBuffer::MaskedDrawtoFrameBuffer(
        UEFI_PIXEL* FrameBuffer, UINT16 FrameBufferWidth, 
        bmp_t* bmp, UINT16 SrcStride, 
        UINT16 SrcX, UINT16 SrcY, 
        UINT16 DstX, UINT16 DstY, 
        UINT16 Width, UINT16 Height, int adjust)
{
    rgb_t* data =(rgb_t*) bmp->data ;
    size_t lwidth = real_width(bmp->width*3);
    rgb_t* dataEnd = (rgb_t*)(((char*)(bmp->data )) +( bmp->height * lwidth ));    
    data = (rgb_t*)(((char*)(bmp->data )) +( SrcY * lwidth + 3*SrcX));
    if(Width == 0) Width = (UINT16)(FrameBufferWidth <  bmp->width? FrameBufferWidth:bmp->width);
    if(bmp->width <  Width) Width = (UINT16)bmp->width;
    if(bmp->height <  Height) Height =(UINT16) bmp->height;
    UEFI_PIXEL* pFrameBuffer = FrameBuffer + DstY*FrameBufferWidth + DstX;
    for(UINT16 row =0; row < Height; row ++){        
        for(UINT16 col=0; col < Width; col++){
            if( ((*(unsigned int*) & data[col]) & THRESHHOLD(16)) != 0){
                bmp2Uefi(pFrameBuffer[col], data[col] );
            }
        }
        data  = (rgb_t*)(((char*)(data)) + lwidth);
        if(data + Width > dataEnd)break;
        pFrameBuffer += FrameBufferWidth;
    }    
    (void)dataEnd;
    return EFI_SUCCESS;
}
/**
  DestinationX and DestinationY is 0
  Copy a sub region(startX,startY, Width, Height) 
  from FrameBuffer to Buffer(0, 0, Width,Height)

 **/
void UFrameBuffer::CopyFromFrameBuffer(UEFI_PIXEL* FrameBuffer, UINT16 FrameBufferWidth, UEFI_PIXEL* Buffer, UINT16 SrcStride, UINT16 startX, UINT16 startY, UINT16 Width, UINT16 Height)
{
    UEFI_PIXEL *pSrc, *pDst;
    pSrc = Buffer;
    pDst = FrameBuffer + FrameBufferWidth * startY + startX;

    each_pixel(SrcStride, FrameBufferWidth){
            pSrc[col] = pDst[col];           
        }
    }
}

////////////////////////////////////////////////////////////////////////

}//namespace UGUI{
