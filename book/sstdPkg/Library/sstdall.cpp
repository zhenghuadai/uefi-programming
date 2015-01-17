extern "C" {
#include <Uefi.h>
#include <Library\BaseLib.h>
#undef NULL
#define NULL 0
typedef void (__cdecl *_PVFV)(void);
typedef int  (__cdecl *_PIFV)(void);
typedef void (__cdecl *_PVFI)(int); 
_PVFV *atexits  = (_PVFV*)NULL;
int num_atexit = 0;
int max_atexit =-1;
/** The atexit function registers the function pointed to by func, to be
    called without arguments at normal program termination.
    
	 @return   The atexit function returns zero if the registration succeeds,
	 nonzero if it fails.
**/
int
atexit(void (*handler)(void))
{	
	if(handler == NULL)
		return 0;
	if(num_atexit >= max_atexit){
		max_atexit += 32;
		_PVFV* old_handler = atexits;
		atexits	= new _PVFV[max_atexit];
		if(atexits == NULL) {
			atexits = old_handler;
			return -1;
		}
		for(int i=0;i<max_atexit-32;i++)
			atexits[i] = old_handler[i];
		delete old_handler;		
	}
	atexits[num_atexit++] = handler;
	return 0;
}

}

extern  "C"  int __cdecl _purecall ( void ) 
{ 
    return  0 ; 
}

extern  "C"  INT64 _allmul(INT64 sm1, INT64 sm2)
{ 
    return (INT64) MultS64x64(sm1, sm2);
     

}

extern  "C"  INT64 _alldiv(INT64 sm1, INT64 sm2)
{ 
    return   DivS64x64Remainder(sm1, sm2, NULL); 
}

extern  "C"  UINT64 _aulldiv(UINT64 sm1, UINT64 sm2)
{ 
    return   DivU64x64Remainder(sm1, sm2, NULL); 
}
