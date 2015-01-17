#ifndef WIN_TYPE_HEADER__
#define WIN_TYPE_HEADER__

typedef UINT16 WORD;
typedef UINT16* PWORD;
typedef UINT8* PCHAR;
typedef UINT8  UCHAR;
typedef UINT32 ULONG;
typedef UINT32 DWORD;
typedef UINT16 USHORT;
#define u8  unsigned char
#define u16 unsigned short int
#define u32 unsigned int
#define u64 unsigned long long
typedef union{
	struct{
	UINT32 low;
        UINT32	high; 
	};
	UINT64 QuadPart; 
} LARGE_INTEGER  ; 
typedef EFI_STATUS NTSTATUS;


typedef struct {
	UINT32 addr;
	UINT16 len;
	unsigned short reserved:14;
	unsigned short BUP:1;
	unsigned short IOC:1;
} BufferDescriptor;

NTSTATUS InitAC97 (void);
#define WriteBMControlRegister(addr,data) WriteMaster(addr,data)

#define AC_CONST static const

#endif
