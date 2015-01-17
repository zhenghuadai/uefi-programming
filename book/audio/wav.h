/**
 * =====================================================================================
 *       @file    wav.h
 *
 *       @brief   
 *
 *       @author  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *       @version 1.0
 *       @date    01/19/2011 08:38:08 PM
 *
 *       Revision none
 *       Compiler gcc
 *       Company  
 * =====================================================================================
 */
#ifndef  wav_header_INC
#define  wav_header_INC
#define u8  unsigned char
#define u16 unsigned short int
#define u32 unsigned int
#define u64 unsigned long long
//#include <stdio.h>
//#include <stdlib.h>
#include "shellfile.h"
#ifndef __cplusplus
typedef int bool;
enum{false=0, true =1};
#endif
//#undef __inline
//#define __inline inline static
enum{
    int_RIFF=0x46464952,
    int_WAVE=0x45564157,
    int_fact=0x74636166,
    int_data=0x61746164,
    int_fmt =0x20746d66,
};

typedef struct {
    union{
        u8  szRiffID[4];  //RIFF:46464952
        u32 dwRiffID;
    };
    u32 dwRiffSize;
    union{
        u8  szRiffFmt[4]; //WAVE:45564157
        u32 dwRiffFmt;
    };
} RIFF_HEADER;

__inline bool isRiff(u8* c){ if( *(u32*)c == int_RIFF) return true; else return false;}
__inline bool isWave(u8* c){ if( *(u32*)c == int_WAVE) return true; else return false;}
__inline bool isFact(u8* c){ if( *(u32*)c == int_fact) return true; else return false;}
__inline bool isData(u8* c){ if( *(u32*)c == int_data) return true; else return false;}
__inline bool isFmt(u8* c) { if( *(u32*)c == int_fmt) return true; else return false;}

#pragma pack(2)
typedef struct
{
    u16 wFmtTag;          //0x0001
    u16 wChannels;        //1-signle       2-double
    u32 dwSamplesPerSec;   //
    u32 dwAvgBytesPerSec; //
    u16 wBlockAlign;      
    u16 wBitsPerSample;   //bits per sample
    //u16 wReserved;
}WAVE_FORMAT;

typedef struct{
    union{
    u8 szFmtID[4];         //"fmt " :20746d66
    u32 dwFmtID;
    };
    u32 dwFmtSize;         //16 or 18 (including wReserved)
    WAVE_FORMAT wavFmt;
}FMT_HEADER;
#pragma pack()

typedef struct{
    union{
        u8 szFactID[4];  //FACT:74636166
        u32 dwFactID;
    };
    u32 dwFactSize;
} FACT_HEADER;

typedef struct{
    union{
        u8 szDataID[4]; //DATA:61746164
        u32 dwDataID;
    };
    u32 dwDataSize;
}DATA_HEADER;

typedef struct{
    RIFF_HEADER dmRiffHeader;
    FMT_HEADER  dmFmtHeader;
    FACT_HEADER dmFactHeader;
    DATA_HEADER dmDataHeader;
    unsigned char* wavFact;
    unsigned char* wavData;
    u32 nSamples;
}WAV_HEADER;
int       printf  (const char * __restrict format, ...);
__inline void printWavHeader(WAV_HEADER* pHeader)
{
    printf("riff ID          :%c%c%c%c\n", pHeader->dmRiffHeader.szRiffID[0],pHeader->dmRiffHeader.szRiffID[1],pHeader->dmRiffHeader.szRiffID[2],pHeader->dmRiffHeader.szRiffID[3]);
    printf("riff size        :%d\n", pHeader->dmRiffHeader.dwRiffSize);
    printf("riff Fmt         :%c%c%c%c\n", pHeader->dmRiffHeader.szRiffFmt[0],pHeader->dmRiffHeader.szRiffFmt[1],pHeader->dmRiffHeader.szRiffFmt[2],pHeader->dmRiffHeader.szRiffFmt[3]);

    printf("format ID        :%c%c%c%c\n", pHeader->dmFmtHeader.szFmtID[0],pHeader->dmFmtHeader.szFmtID[1],pHeader->dmFmtHeader.szFmtID[2],pHeader->dmFmtHeader.szFmtID[3]);
    printf("format Size      :%d\n", pHeader->dmFmtHeader.dwFmtSize);
    printf("format tag       :%d\n", pHeader->dmFmtHeader.wavFmt.wFmtTag);
    printf("format channels  :%d\n", pHeader->dmFmtHeader.wavFmt.wChannels);
    printf("Samples per Sec  :%d\n", pHeader->dmFmtHeader.wavFmt.dwSamplesPerSec);
    printf("Avg Bytes per Sec:%d\n", pHeader->dmFmtHeader.wavFmt.dwAvgBytesPerSec);
    printf("Block Align      :%d\n", pHeader->dmFmtHeader.wavFmt.wBlockAlign);
    printf("Bits per Sample  :%d\n", pHeader->dmFmtHeader.wavFmt.wBitsPerSample);

    if(pHeader->wavFact != NULL){
        printf("Fact ID          :\n");
        printf("Fact Size        :\n");
    }
    printf("Data ID          :%c%c%c%c\n",pHeader->dmDataHeader.szDataID[0],pHeader->dmDataHeader.szDataID[1],pHeader->dmDataHeader.szDataID[2],pHeader->dmDataHeader.szDataID[3]);
    printf("Data Size        :%d\n",pHeader->dmDataHeader.dwDataSize);
}

__inline bool checkRiffHeader(RIFF_HEADER* pHeader){
    return ((pHeader-> dwRiffID == int_RIFF) &&(pHeader-> dwRiffFmt== int_WAVE));
}
__inline bool checkFmtHeader(FMT_HEADER* pHeader){
    return (pHeader-> dwFmtID == int_fmt );
}
__inline bool checkFactHeader(FACT_HEADER* pHeader){
    return (pHeader-> dwFactID == int_fact);
}
__inline bool checkDataHeader(DATA_HEADER* pHeader){
    return (pHeader-> dwDataID == int_data);
}

/**
 * @brief read wave file
 *
 * @param infp :wavefile FILE*(file decription) 
 *
 * @return WAV_HEADER* 
 */
__inline WAV_HEADER* readWaveFile(FILE* infp)
{
    WAV_HEADER* pheader;
    if (infp == NULL) return NULL;
    //Print(L"this is read.\n");
    pheader = (WAV_HEADER*) malloc(sizeof(WAV_HEADER));
    memset(pheader, 0, sizeof(WAV_HEADER));
    //! read RIFF HEADER
    fread( &(pheader->dmRiffHeader), sizeof(RIFF_HEADER), 1, infp);
    if(checkRiffHeader(& (pheader->dmRiffHeader)) == false){
        //Print(L"It is not wav\n");
        return NULL;
    }
    //! read Format Header 
    fread( &(pheader->dmFmtHeader), sizeof(FMT_HEADER), 1, infp);
    if(pheader->dmFmtHeader.dwFmtSize == 18){
        // read 2 bytes 
        CHAR16 c;
        fread( &c, 2, 1, infp);
    }
    fread( &(pheader->dmFactHeader), sizeof(FACT_HEADER), 1, infp);
    if(checkFactHeader(& (pheader->dmFactHeader)) == true){
        pheader->wavFact = (unsigned char*)malloc(pheader->dmFactHeader.dwFactSize);
        fread(pheader->wavFact,pheader->dmFactHeader.dwFactSize, 1, infp);
        //! read DataHeader
        fread( &(pheader->dmDataHeader), sizeof(DATA_HEADER), 1, infp);
    }else{
        //! copy DataHeader
        pheader->wavFact=NULL;
        pheader->dmDataHeader.dwDataID   = pheader->dmFactHeader.dwFactID;
        pheader->dmFactHeader.dwFactID = 0;
        pheader->dmDataHeader.dwDataSize = pheader->dmFactHeader.dwFactSize;
        pheader->dmFactHeader.dwFactSize = 0;

    }

    if(checkDataHeader( &(pheader->dmDataHeader)) == false){
        return NULL;
    }

    //printWavHeader(pheader);
    //Print(L"read size:%d\n", pheader->dmDataHeader.dwDataSize);
    pheader->wavData= (unsigned char*)malloc(pheader->dmDataHeader.dwDataSize);
    fread(pheader->wavData,pheader->dmDataHeader.dwDataSize, 1, infp);

    return pheader;
}

/**
 * @brief :read wave file
 *
 * @param infn : file name 
 *
 * @return 
 */
__inline WAV_HEADER* readWaveFile2(char* infn)
{
    FILE* fp= fopen(infn, "rb");
    WAV_HEADER* wh ;
    if(fp == NULL) return NULL;
    //printf("infn %s\n", infn);
    //Print(L"file: %0x\n", fp);
    wh = readWaveFile(fp);
    fclose(fp);
    return wh;
}

/**
 * @brief write wave file
 *
 * @param ofp    :FILE*
 * @param pheader:WAV_HEADER*
 *
 * @return 
 */
__inline int writeWaveFile(FILE* ofp, WAV_HEADER* pheader)
{
    fwrite( &(pheader->dmRiffHeader), sizeof(RIFF_HEADER), 1, ofp);
    fwrite( &(pheader->dmFmtHeader),8 + pheader->dmFmtHeader.dwFmtSize, 1, ofp);
    if(pheader->wavFact !=NULL){
        fwrite( &(pheader->dmFactHeader), sizeof(FACT_HEADER), 1, ofp);
        fwrite( pheader->wavFact, pheader->dmFactHeader.dwFactSize, 1, ofp);
    }
    fwrite( &(pheader->dmDataHeader), sizeof(DATA_HEADER), 1, ofp);
    fwrite( pheader->wavData, pheader->dmDataHeader.dwDataSize, 1, ofp);
    return 0;
}

__inline int writeWaveFile2(char* ofn, WAV_HEADER* pheader)
{
    FILE* fp= fopen(ofn, "w");
    int ret ;
    ret = writeWaveFile(fp, pheader);
    fclose(fp);
    return ret;
}

__inline void freeWavHeader(WAV_HEADER* wavHeader)
{
    if (wavHeader->wavFact ) free(wavHeader->wavFact);
    if (wavHeader->wavData ) free(wavHeader->wavData);
    if (wavHeader ) free(wavHeader);
}

#endif   /* ----- #ifndef wav_header_INC  ----- */
