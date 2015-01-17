/*
 * =====================================================================================
 *
 *       Filename:  ffdecoder.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/12/2012 09:13:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef  __FFDECODER_H__
#define  __FFDECODER_H__

#ifdef __cplusplus
extern "C"{
#endif
#include <Uefi.h> 
#include <Base.h> 
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Protocol/DiskIo.h>

#ifndef HAVE_FFMPEG_HEADER

typedef UINT8  uint8_t;
typedef UINT16 uint16_t;
typedef UINT32 uint32_t;
typedef UINT64 uint64_t;
typedef INT8   int8_t;
typedef INT16  int16_t;
typedef INT32  int32_t;
typedef INT64  int64_t;

#define FF_COMMON_FRAME \
 \
     uint8_t *data[4];\
     int linesize[4];\
 \
     uint8_t *base[4];\
 \
     int key_frame;\
 \
 \
     int pict_type;\
 \
 \
     int64_t pts;\
 \
 \
     int coded_picture_number;\
 \
     int display_picture_number;\
 \
 \
     int quality; \
 \
 \
     int age;\
 \
 \
     int reference;\
 \
 \
     int8_t *qscale_table;\
 \
     int qstride;\
 \
 \
     uint8_t *mbskip_table;\
 \
 \
     int16_t (*motion_val[2])[2];\
 \
 \
     uint32_t *mb_type;\
 \
 \
     uint8_t motion_subsample_log2;\
 \
 \
     void *opaque;\
 \
 \
     uint64_t error[4];\
 \
 \
     int type;\
     \
 \
     int repeat_pict;\
     \
 \
     int qscale_type;\
     \
 \
     int interlaced_frame;\
     \
 \
     int top_field_first;\
     \
 \
     AVPanScan *pan_scan;\
     \
 \
     int palette_has_changed;\
     \
 \
     int buffer_hints;\
 \
 \
     short *dct_coeff;\
 \
 \
     int8_t *ref_index[2];\
 \
 \
     int64_t reordered_opaque;\
 \
 \
     void *hwaccel_picture_private;\
 \
 \
     int64_t pkt_pts;\
 \
 \
     int64_t pkt_dts;\
 \
 \
     struct AVCodecContext *owner;\
 \
 \
     void *thread_opaque;\
 \
 \
     int64_t best_effort_timestamp;\
 
struct AVCodecContext ;
typedef void AVPanScan ;

typedef struct AVFrame {
    FF_COMMON_FRAME
} AVFrame;


#endif


#define EFI_FFDECODER_PROTOCOL_GUID \
{ \
    0xce345171, 0xabcd, 0x11d2, {0x8e, 0x4f, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
}

//
///// Protocol GUID name defined in EFI1.1.
// 
#define FFDECODER_PROTOCOL  EFI_FFDECODER_PROTOCOL_GUID

typedef struct _EFI_FFDECODER_PROTOCOL EFI_FFDECODER_PROTOCOL;

//
///// Protocol defined in EFI1.1.
// 
typedef EFI_FFDECODER_PROTOCOL  EFI_FFDECODER;

/**
  Open the video, the video handle is stored in default location. 

  @param  This       Indicates a pointer to the calling context.
  @param  FileName   File name of the video under current dir. 

  @retval EFI_SUCCESS           The video is opened successfully. 
  @retval EFI_NOT_FOUND         There is no such file.

**/
typedef 
EFI_STATUS
(EFIAPI* EFI_OPEN_VIDEO)(
        IN EFI_FFDECODER_PROTOCOL* This,
        IN CHAR16* FileName
        );

/**
  Open the video, the video handle is dynamically alloced. 

  @param  This       Indicates a pointer to the calling context.
  @param  FileName   File name of the video under current dir. 
  @param  VideoHandle The EFI_FFDECODER_PROTOCOL Handle for the opened video

  @retval EFI_SUCCESS           The video is opened successfully. 
  @retval EFI_NOT_FOUND         There is no such file.

**/
typedef 
EFI_STATUS
(EFIAPI* EFI_OPEN_VIDEO2)(
        IN EFI_FFDECODER_PROTOCOL* This,
        IN CHAR16* FileName,
        OUT EFI_FFDECODER_PROTOCOL** VideoHandle
        );


/**
  Close the video 

  @param  This       Indicates a pointer to the calling context.

  @retval EFI_SUCCESS           The video is closed successfully. 

**/
typedef 
EFI_STATUS
(EFIAPI* EFI_CLOSE_VIDEO)(
        IN EFI_FFDECODER_PROTOCOL* This
        );

/**
  Query a frame from the video. 

  @param  This       Indicates a pointer to the calling context.
  @param  pFrame     Points to the current Frame.  

  @retval EFI_SUCCESS          The video is opened successfully. 
  @retval EFI_NO_MEDIA         There is no opened video. 
  @retval EFI_END_OF_MEDIA     There is no more Frame. 

**/
typedef 
EFI_STATUS
(EFIAPI* EFI_QUERY_FRAME)(
        IN  EFI_FFDECODER_PROTOCOL  *This,
        OUT AVFrame                 **pFrame
        );

/**
  Query the Width and height of a frame. 

  @param  This       Indicates a pointer to the calling context.
  @param  Width      Width(in pixels) of a frame.
  @param  Height     Height(in pixels) of a frame.

  @retval EFI_SUCCESS          Return the Size successfully. 
  @retval EFI_NO_MEDIA         There is no opened video. 

**/
typedef 
EFI_STATUS
(EFIAPI* EFI_QUERY_FRAME_SIZE)(
        IN  EFI_FFDECODER_PROTOCOL  *This,
        OUT UINT32                  *Width,
        OUT UINT32                  *Height 
        );

/**
  Query the Width and height of a frame. 

  @param  This       Indicates a pointer to the calling context.
  @param  Width      Width(in pixels) of a frame.
  @param  Height     Height(in pixels) of a frame.

  @retval EFI_SUCCESS          Return the Size successfully. 
  @retval EFI_NO_MEDIA         There is no opened video. 

**/
typedef 
EFI_STATUS
(EFIAPI* EFI_SET_FRAME_SIZE)(
        IN  EFI_FFDECODER_PROTOCOL  *This,
        OUT UINT32                  Width,
        OUT UINT32                  Height 
        );


/** 
 * **/
struct _EFI_FFDECODER_PROTOCOL{
 UINT64          Revision;
 EFI_OPEN_VIDEO  OpenVideo;
 EFI_CLOSE_VIDEO CloseVideo;
 EFI_QUERY_FRAME QueryFrame;
 EFI_QUERY_FRAME_SIZE QueryFrameSize;
 EFI_OPEN_VIDEO2 OpenVideo2;

 EFI_SET_FRAME_SIZE SetFrameSize;

};

extern EFI_GUID gEfiFFDecoderProtocolGUID;

#ifdef UFEI_DEBUG
#define sdbginfo() Print(L"%a:%d:Start\n", __func__, __LINE__)
#define dbginfo(fmt, ...) Print(L"%a:%d:" fmt, __func__, __LINE__, ##__VA_ARGS__)
#define ddbginfo() dbginfo(L"\n")
#define edbginfo(ret) Print(L"%a:%d:End with %d\n", __func__, __LINE__, ret)

#define dbglogStart(fmt,...) Print(L"%a %d::Start:" fmt,  __func__, (  __LINE__) , ##__VA_ARGS__);
#define dbglogEnd(fmt,...) Print(L"%a %d::End:" fmt,  __func__, (  __LINE__) , ##__VA_ARGS__);

#else
#define sdbginfo() 
#define dbginfo(fmt, ...) 
#define ddbginfo() 
#define edbginfo(ret) 
#endif


#ifdef __cplusplus
}
#endif
#endif   /* ----- #ifndef __FFDECODER_H__  ----- */

