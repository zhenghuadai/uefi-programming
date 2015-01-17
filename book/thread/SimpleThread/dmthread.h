
#ifndef  DMTHREAD_HEADER
#define  DMTHREAD_HEADER
#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Timer.h>
#include <Library/BaseLib.h>

# if __WORDSIZE == 64
#define bp_register "rbp"
#define sp_register "rsp"
#define OFFSET_TO_dmtcontext 7 
# else
#define bp_register "ebp"
#define sp_register "esp"
#define OFFSET_TO_dmtcontext 3 
# endif
#if defined(__GNUC__)
#define CHANGE_STACK(newaddr) __asm( "mov %0, %%rsp"::"m"(newaddr));
#else
#define CHANGE_STACK(newaddr) _asm mov esp, newaddr
#endif

#define STACK_SIZE  65536 
#define RESERVED_STACK  4 


typedef void ( * thread_func_t)(void * );

typedef BASE_LIBRARY_JUMP_BUFFER dmtcontext;
typedef unsigned long long ptr_size ;

enum{FRAME_JMPBUF=1, FRAME_SIGCONTEXT=0, FRAME_TWO=2, FRAME_UCONTEXT=3};
enum{STATUS_DEAD=1, STATUS_READY=2};
enum{STATUS_IN_MAIN=0, STATUS_CREATED=1, STATUS_IN_THREAD=2 };

typedef struct dmthread_t
{
    dmtcontext sig_context;
    unsigned short status;
    unsigned short priority;
    thread_func_t kernel;
    void *  arg;
    char *  stack;
    volatile unsigned int status_w;
} dmthread_t;


typedef struct thread_list
{
    dmthread_t thread;
    volatile struct thread_list *   next ;
    struct thread_list *   prev;
} thread_list;

typedef struct Scheduler
{
    thread_list *  threads;
    thread_list *  current;
    char  * unfreed_stack;
    int  retaddr;
} Scheduler;


extern Scheduler sys;

void create_thread(thread_func_t f, void *  arg);
void thread_join();

int setTimer ();
#if 1
#define myprintf(...) fprintf(stdout, __VA_ARGS__); fflush(stdout);
#else
#define myprintf(...)
#endif
#endif     /* -----  not DMTHREAD_HEADER  ----- */
