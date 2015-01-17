#ifndef _BITMAP_HEADNER__
#define _BITMAP_HEADNER__

typedef unsigned char u8;
#define bitmapHeaderSize (54) // (sizeof(bitmapFileHeader_t) + sizeof(bitmapInfoHeader_t))

#pragma pack (2)
typedef struct { 
	unsigned short type;      /*Specifies the file type. It must be set to the signature word BM (0x4D42) to indicate bitmap. */
	unsigned int fsize;       /*Specifies the size, in u8s, of the bitmap file.*/
	unsigned short reserved1; /**/
	unsigned short reserved2; /**/
	unsigned int offsetbytes;  /*Specifies the offset, in u8s, from the bitmapFileHeader_t structure to the bitmap bits*/
} bitmapFileHeader_t; 

typedef struct { 
	unsigned int hsize;       /*sizeof(bitmapInfoHeader_t), 40.*/
	unsigned int width;      /**/
	unsigned int height;     /**/
	unsigned short planes;   /*Specifies the number of planes for the target device. This value must be set to 1*/
	unsigned short bitcount; /*1;4;8;24*/
	unsigned int compression;/**/ 
	unsigned int sizeimage;  /**/
	int xpelspermeter;       /**/
	int ypelspermeter;       /**/
	unsigned int colorsused; /**/
	unsigned int colorsimportant;/**/ 
} bitmapInfoHeader_t; 

typedef struct tagRGBQUAD {
    u8 B;//
    u8 G;//
    u8 R; //
    u8 Reserved;//
} RGBQUAD;

typedef struct { 
	unsigned char B; 
	unsigned char G; 
	unsigned char R; 
} rgb_t;

//#pragma (disable: C4201)
typedef struct {
	union{
		struct{
			bitmapFileHeader_t file_header; 
			bitmapInfoHeader_t info_header;
		};
		struct{
			unsigned short type; 
			unsigned int fsize; 
			unsigned short reserved1; 
			unsigned short reserved2; 
			unsigned int offsetbytes; 
			/**/
			unsigned int hSize; 
			unsigned int width; 
			unsigned int height; 
			unsigned short planes; 
			unsigned short bitcount; 
			unsigned int compression; 
			unsigned int sizeimage; 
			int xpelspermeter; 
			int ypelspermeter; 
			unsigned int colorsused; 
			unsigned int colorsimportant; 

		};
	};
	unsigned char* data;
    RGBQUAD* color_table;
}bmp_t;
#pragma pack ()
#ifdef __cplusplus
template<typename T> T real_width(T width){ return (( width %4)?(( width+4) &(~3)) : width);}
#endif
__inline size_t real_width(size_t w){
	//return ((w %4)?((w+4) &(~3)) :w);
	return ( w + 3) & (~3);
}

void free_bmp(bmp_t* bmp);

unsigned char* read_bmp(char * filename,	bmp_t* bmp);

int write_bmp(char * filename, bmp_t* bmp, rgb_t * pixel=0);

int write_bmp(char * filename, rgb_t * pixel, int  width, int  height);

int write_gray256(char* filename, bmp_t* bmp, u8* pixel = (u8*) NULL);
int write_gray256(char * filename, u8* pixel, int  width, int  height);

bmp_t* create_bmp(int width, int height, rgb_t* pixel=0);
bmp_t* create_gray256(int width, int height, u8* pixel=0);

/**  @} */
#endif	//_BITMAP_HEADNER__
