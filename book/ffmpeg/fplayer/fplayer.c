// avcodec_sample.cpp

// A small sample program that shows how to use libavformat and libavcodec to
// read video from a file.
//
// Use
//
// g++ -o avcodec_sample avcodec_sample.cpp -lavformat -lavcodec -lz
//
// to build (assuming libavformat and libavcodec are correctly installed on
// your system).
//
// Run using
//
// avcodec_sample myvideofile.mpg
//
// to write the first five frames from "myvideofile.mpg" to disk in PPM
// format.

#if 1
#define dbginfo() av_log(0,0, "%s:%d\n", __func__, __LINE__)
#else
#define dbginfo() 
#endif
#ifdef __cplusplus
extern "C"{
#endif
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>

#ifdef __UEFI__
#include <Uefi.h> 
#include <Base.h> 
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;

EFI_STATUS LocateGraphicsOutput()
{
    EFI_STATUS Status = gBS->LocateProtocol(
            &gEfiGraphicsOutputProtocolGuid,
            NULL,
            (VOID **)&GraphicsOutput);
    if (EFI_ERROR(Status)) {
        Print(L"LocateProtocol %r\n", Status);
    }
    return Status;
}

void ShowFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
#if 1
    if(GraphicsOutput)
    GraphicsOutput->Blt(
            GraphicsOutput,
            pFrame->data[0],
            EfiBltBufferToVideo,
            0,0,
            0,0,
            width, height,
            0
            );
#endif
}


#endif //__UEFI__

#ifdef __cplusplus
}
#endif

#include <stdio.h>
#define true 1
#define false 0 

int GetNextFrame(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx, 
    int videoStream, AVFrame *pFrame)
{
    static AVPacket packet;
    static int      bytesRemaining=0;
    static uint8_t  *rawData;
    static int     fFirstTime=true;
    int             bytesDecoded;
    int             frameFinished = 0;

    // First time we're called, set packet.data to NULL to indicate it
    // doesn't have to be freed
    if(fFirstTime)
    {
        fFirstTime=false;
        packet.data=NULL;
    }

    // Decode packets until we have decoded a complete frame
    while(true)
    {
        // Work on the current packet until we have decoded all of it
        while(bytesRemaining > 0)
        {
            // Decode the next chunk of data
            //bytesDecoded=avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, rawData, bytesRemaining);
            bytesDecoded=avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Was there an error?
            if(bytesDecoded < 0)
            {
                //fprintf(stderr, "Error while decoding frame\n");
                return false;
            }

            bytesRemaining-=bytesDecoded;

            // Did we finish the current frame? Then we can return
            if(frameFinished)
                return true;
        }

        // Read the next packet, skipping all packets that aren't for this
        // stream
        do
        {
            // Free old packet
            int ret = 0;
            if(packet.data!=NULL)
                av_free_packet(&packet);

            // Read new packet
            if((ret = av_read_packet(pFormatCtx, &packet))<0){
                dbginfo(); 
                goto loop_exit;
            }
            av_log(0,0,"Read channel:%d, size:%d\n", packet.stream_index, packet.size);
            //if(ret == 32)
            {
                dbginfo(); 
             //   goto loop_exit;
            }
        } while(packet.stream_index!=videoStream);
        bytesRemaining=packet.size;
    }

loop_exit:

    // Decode the rest of the last frame
    //bytesDecoded=avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, rawData, bytesRemaining);
    bytesDecoded=avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,  &packet);

    // Free last packet
    if(packet.data!=NULL)
        av_free_packet(&packet);

    av_log(0,0,"Parse ret :%d\n", frameFinished);
    return frameFinished!=0;
}
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    snprintf(szFilename, 16,"frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;

    // Write header
    //fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

    // Close file
    fclose(pFile);
}

int main(int argc, char *argv[])
{
    AVFormatContext *pFormatCtx;
    int             i, videoStream;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVFrame         *pFrame; 
    AVFrame         *pFrameRGB;
    int             numBytes;
    uint8_t         *buffer;
    struct SwsContext  *img_convert_ctx ;
#ifdef __UEFI__
    LocateGraphicsOutput();
#endif
    // Register all formats and codecs
    av_register_all();

    dbginfo();
    // Open video file
    if(av_open_input_file(&pFormatCtx, argv[1], NULL, 0, NULL)!=0)
        return -1; // Couldn't open file

    dbginfo();
    // Retrieve stream information
    if(av_find_stream_info(pFormatCtx)<0)
        return -1; // Couldn't find stream information

    // Dump information about file onto standard error
    //dump_format(pFormatCtx, 0, argv[1], false);

    dbginfo();
    // Find the first video stream
    videoStream=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++){
        //Print(L"type %d ",pFormatCtx->streams[i]->codec->codec_type  );
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoStream=i;
            break;

        }
    }
    if(videoStream==-1)
        return -1; // Didn't find a video stream

    dbginfo();
    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)
        return -1; // Codec not found

    dbginfo();
    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
    if(pCodec->capabilities & CODEC_CAP_TRUNCATED)
        pCodecCtx->flags|=CODEC_FLAG_TRUNCATED;

    dbginfo();
    // Open codec
    if(avcodec_open(pCodecCtx, pCodec)<0)
        return -1; // Could not open codec

    // Hack to correct wrong frame rates that seem to be generated by some 
    // codecs
    //if(pCodecCtx->frame_rate>1000 && pCodecCtx->frame_rate_base==1)
    //    pCodecCtx->frame_rate_base=1000;

    dbginfo();
    // Allocate video frame
    pFrame=avcodec_alloc_frame();

    // Allocate an AVFrame structure
    pFrameRGB=avcodec_alloc_frame();
    if(pFrameRGB==NULL)
        return -1;

    dbginfo();
    // Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(PIX_FMT_RGBA, pCodecCtx->width,
        pCodecCtx->height);
    buffer=(uint8_t*) av_malloc(numBytes);

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGBA,
        pCodecCtx->width, pCodecCtx->height);

    dbginfo();
    // 
    img_convert_ctx = sws_getContext( pCodecCtx->width, pCodecCtx->height,
            PIX_FMT_YUV420P, 
            pCodecCtx->width, pCodecCtx->height,
            PIX_FMT_RGBA, 
            SWS_SPLINE, NULL, NULL, NULL); 
    // Read frames and save first five frames to disk
    i=0;
    dbginfo();
    while(GetNextFrame(pFormatCtx, pCodecCtx, videoStream, pFrame))
    {
        dbginfo();
        //img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, (AVPicture*)pFrame, 
        //    pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
        sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 
                0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize); 


        // Save the frame to disk
#ifdef __UEFI__
        //if(++i<=5)  SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
        ShowFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
#endif
    }

    // Free the RGB image
    av_free( buffer);
    av_free(pFrameRGB);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codec
    avcodec_close(pCodecCtx);

    // Close the video file
    av_close_input_file(pFormatCtx);

    dbginfo();
    return 0;
}
