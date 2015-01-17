#include "bmp.h"
extern "C"{
//#include <stdio.h>
//#include <stdlib.h>
}

#include "shellfile.h"
#ifndef __inline__
#define __inline__ 
#undef NULL
#define NULL 0
#endif
extern "C" int       printf  (const char * __restrict format, ...);

__inline__ void free_bmp(bmp_t* bmp)
{
    free(bmp->data);
    bmp->data = 0;
}

__inline__ unsigned char* read_bmp(char * filename, bmp_t* bmp )
{
    //int lwidth;  
    FILE * file = fopen(filename, "rb");
    if (!file) {
        printf("File open error: %s\n", filename);
        return NULL;
    }

    fread((void *)&(bmp->file_header), sizeof(bitmapFileHeader_t), 1, file);
    if (bmp->file_header.type != 0x4d42) {
        printf("Not a bmp file\n");
        fclose(file);
        return NULL;
    }

    fread((void *)&(bmp->info_header), sizeof(bitmapInfoHeader_t), 1, file);
    if(bmp->offsetbytes != 54)
        fseek(file, bmp->file_header.offsetbytes, SEEK_SET);
    //lwidth = ((bmp->info_header.width %4)?((bmp->info_header.width+4) &(~3)) :bmp->info_header.width);
    bmp->info_header.sizeimage =  real_width(3*bmp->info_header.width)* bmp->info_header.height ;
    rgb_t * p = (rgb_t*)malloc(bmp->info_header.sizeimage);
    if (!p) {
        printf("Memory allocation error.\n");
        return NULL;
    }
    fread((void *)p, bmp->info_header.sizeimage, 1, file);
    bmp->data =(unsigned char*) p;

    fclose(file);
    return (unsigned char*) p;
}

__inline__ int write_bmp(char * filename, bmp_t* bmp,
        rgb_t * pixel)
{
    FILE * file = fopen(filename, "wb");
    if (!file) {
        printf("File open error: %s\n", filename);
        return -1;
    }
    if(!pixel) 
        pixel = (rgb_t*) bmp->data;
    if(!pixel)
        return -1;
    fwrite((void *)&(bmp->file_header), sizeof(bitmapFileHeader_t), 1, file);
    fwrite((void *)&(bmp->info_header), sizeof(bitmapInfoHeader_t), 1, file);
    fwrite((void *)pixel, bmp->info_header.sizeimage, 1, file);
    fclose(file);
    return 0;
}

__inline__ bmp_t* create_bmp(
        int  width,
        int  height,
        rgb_t * pixel)
{
    bmp_t* bmp = (bmp_t*) malloc(sizeof(bmp_t));
    
    int sizeImage = height* (int)real_width(width*sizeof(rgb_t)); //lwidth * height * (int)sizeof(rgb_t);

    bmp->file_header.type = 19778;  
    bmp->file_header.reserved1 = bmp->file_header.reserved2 = 0;
    bmp->file_header.offsetbytes = 54;  
    bmp->file_header.fsize = 54 + sizeImage;

    bmp->info_header.hsize = sizeof(bitmapInfoHeader_t);
    bmp->info_header.width = width;
    bmp->info_header.height = height;
    bmp->info_header.planes = 1;
    bmp->info_header.bitcount = 8 * sizeof(rgb_t);
    bmp->info_header.compression = 0;
    bmp->info_header.sizeimage = sizeImage;
    bmp->info_header.xpelspermeter = 0;
    bmp->info_header.ypelspermeter = 0;
    bmp->info_header.colorsused = 0;
    bmp->info_header.colorsimportant = 0;
    if(pixel != NULL)
        bmp->data= (u8*)pixel;
    else
        bmp->data = (u8*)malloc(sizeImage); 

    return bmp;
}


__inline__ int write_bmp(char * filename,
        rgb_t * pixel,
        int  width,
        int  height)
{
    bmp_t bmp;
    int sizeImage = height* (int) real_width(width*sizeof(rgb_t));;

    bmp.file_header.type = 19778;  
    bmp.file_header.reserved1 = bmp.file_header.reserved2 = 0;
    bmp.file_header.offsetbytes = 54;  
    bmp.file_header.fsize = 54 + sizeImage;

    bmp.info_header.hsize = sizeof(bitmapInfoHeader_t);
    bmp.info_header.width = width;
    bmp.info_header.height = height;
    bmp.info_header.planes = 1;
    bmp.info_header.bitcount = 8 * sizeof(rgb_t);
    bmp.info_header.compression = 0;
    bmp.info_header.sizeimage = sizeImage;
    bmp.info_header.xpelspermeter = 0;
    bmp.info_header.ypelspermeter = 0;
    bmp.info_header.colorsused = 0;
    bmp.info_header.colorsimportant = 0;

    return write_bmp(filename, &bmp, pixel);
}

__inline__ int write_gray256(char* filename, bmp_t* bmp, u8* pixel)
{
    FILE * file;
    if(bmp->bitcount != 8)
        return -1;
    file= fopen(filename, "wb");
    if (!file) {
        printf("File open error: %s\n", filename);
        return -1;
    }
    if(!pixel) 
        pixel = (u8*) bmp->data;
    if(!pixel)
        return -1;
    fwrite((void *)&(bmp->file_header), sizeof(bitmapFileHeader_t), 1, file);
    fwrite((void *)&(bmp->info_header), sizeof(bitmapInfoHeader_t), 1, file);
    if(bmp->color_table !=NULL)
        fwrite((void *)bmp->color_table, sizeof(RGBQUAD)*256, 1, file);
    fwrite((void *)pixel, bmp->info_header.sizeimage, 1, file);
    fclose(file);
    return 0;
}

__inline__ bmp_t* create_gray256(
        int  width,
        int  height,
        u8* pixel)
{
    int i;
    bmp_t* bmp = (bmp_t*) malloc(sizeof(bmp_t));
    int lwidth = real_width(width);

    int sizeImage = lwidth * height * (int)sizeof(u8);

    bmp->file_header.type = 19778;  
    bmp->file_header.reserved1 = bmp->file_header.reserved2 = 0;
    bmp->file_header.offsetbytes = 54 + 1024;  
    bmp->file_header.fsize = 54 + 1024 + sizeImage;

    bmp->info_header.hsize = sizeof(bitmapInfoHeader_t);
    bmp->info_header.width = width;
    bmp->info_header.height = height;
    bmp->info_header.planes = 1;
    bmp->info_header.bitcount = 8;
    bmp->info_header.compression = 0;
    bmp->info_header.sizeimage = sizeImage;
    bmp->info_header.xpelspermeter = 0;
    bmp->info_header.ypelspermeter = 0;
    bmp->info_header.colorsused = 0;
    bmp->info_header.colorsimportant = 0;
    if(pixel != NULL)
        bmp->data= pixel;
    else
        bmp->data = (u8*)malloc(sizeImage); 
    bmp->color_table= (RGBQUAD*)malloc(((size_t)1 << (size_t)bmp->info_header.bitcount)*sizeof(RGBQUAD));
    for(i=0;i<(1<< bmp->info_header.bitcount);i++){
        bmp->color_table[i].B= (u8)i;
        bmp->color_table[i].G= (u8)i;
        bmp->color_table[i].R= (u8)i;
        bmp->color_table[i].Reserved=(u8)i;
    }

    return bmp;
}


__inline__ int write_gray256(char * filename,
        u8* pixel,
        int  width,
        int  height)
{
    int i;
    bmp_t bmp;
    int lwidth = real_width(width);

    int sizeImage = lwidth * height * (int)sizeof(u8);

    bmp.file_header.type = 19778;  
    bmp.file_header.reserved1 = bmp.file_header.reserved2 = 0;
    bmp.file_header.offsetbytes = 54 + 1024;  
    bmp.file_header.fsize = 54 + 1024 + sizeImage;

    bmp.info_header.hsize = sizeof(bitmapInfoHeader_t);
    bmp.info_header.width = width;
    bmp.info_header.height = height;
    bmp.info_header.planes = 1;
    bmp.info_header.bitcount = 8;
    bmp.info_header.compression = 0;
    bmp.info_header.sizeimage = sizeImage;
    bmp.info_header.xpelspermeter = 0;
    bmp.info_header.ypelspermeter = 0;
    bmp.info_header.colorsused = 0;
    bmp.info_header.colorsimportant = 0;
    bmp.data= pixel;
    bmp.color_table= (RGBQUAD*)malloc(((size_t)1<< (size_t)bmp.info_header.bitcount)*sizeof(RGBQUAD));
    for(i=0;i<(1<< bmp.info_header.bitcount);i++){
        bmp.color_table[i].B= (u8)i;
        bmp.color_table[i].G= (u8)i;
        bmp.color_table[i].R= (u8)i;
        bmp.color_table[i].Reserved=(u8)i;
    }

    return write_gray256(filename, &bmp, pixel);
}



size_t gHeight;
size_t gWidth;
size_t gBytesinLine;
