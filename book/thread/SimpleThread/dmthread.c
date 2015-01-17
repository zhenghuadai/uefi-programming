// cothread.c
#include <Uefi.h>
#include <Protocol/HiiFont.h>
#include "Protocol/HiiDatabase.h"
#include <Library/UefiHiiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Timer.h>
#include <Pi/PiDxeCis.h>
#include <Protocol/Cpu.h>
#include <Protocol/SmmBase2.h>
#include"dmthread.h" 

#define free(x) (void) gBS->FreePool(x)
void remove_thread();

INT32 volatile  gThreads ;

volatile unsigned int Inschedule= 0;
EFI_EVENT myEvent=0;
Scheduler sys={0};

void* mMalloc(UINTN s) { 
    void* m = 0; 
    (void) gBS->AllocatePool( EfiLoaderData, (UINTN)s, &m); 
    return m;
}
static thread_list* _new_thread(thread_func_t f, void *  arg)
{
    thread_list *  new_thread;
    new_thread  =  (thread_list * )mMalloc(sizeof(thread_list));
    new_thread -> thread.arg  =  arg;
    new_thread -> thread.stack  =   0 ;
    new_thread -> thread.kernel=  f;
    new_thread -> thread.status=  0;
    new_thread -> thread.priority=  0;
    new_thread -> thread.status_w = STATUS_IN_MAIN;
    new_thread -> next = new_thread;
    new_thread -> prev = new_thread;
    return new_thread;
}

static thread_list* getNext(thread_list* cur)
{
   if( cur -> next -> next -> thread.priority < cur -> next -> thread.priority )
       return (thread_list*)cur->next->next;
   else 
       return (thread_list*)cur->next;
}

static void _insert_thread(thread_list* newt, thread_list* prev)
{
    newt->next = prev->next;
    newt->prev = prev;
    newt->next->prev= newt;
    prev->next = newt;
}

//! return Previous( t->prev)
static thread_list* _remove_thread(thread_list* t)
{
    thread_list *  iter = t -> prev;

    //assert (iter  &&  iter -> next   ==  t);
    iter -> next   =  t -> next ;
    t -> next -> prev = iter;
    free(t);
    return iter;
}


void free_dead_stack()
{
    if  (sys.unfreed_stack){
        free(sys.unfreed_stack);
        sys.unfreed_stack  =   0 ;
    }
}

void remove_thread()
{
    free_dead_stack();
    if  (sys.current  ==  sys.threads) {
        sys.unfreed_stack  =  sys.current -> thread.stack;
        sys.threads  =  getNext(sys.current);
        thread_list *prev =_remove_thread(sys.current);
        sys.current  =  sys.threads;
        (void)prev;
    } else  {
        sys.unfreed_stack  =  sys.current -> thread.stack;
        thread_list *prev =_remove_thread(sys.current);
        sys.current = getNext(prev);
    }

    if  (sys.current  ==   0 ) {
        sys.current  =  sys.threads;
        //assert(0);
    }
}


void switchto( )
{
        LongJump(& sys.current->thread.sig_context, 1);
}

void Skedule()
{
    UINTN i;
    thread_list *  oldThread;
    BOOLEAN CouldSkedule ;
    CouldSkedule =  __sync_bool_compare_and_swap (&Inschedule, 0, 1);
    if(CouldSkedule == 0)
        return;
    //pebp();
    oldThread = sys.current;
    if(oldThread->thread.status == STATUS_DEAD){
        remove_thread();
        //switchto();
        LongJump(& sys.current->thread.sig_context, 1);
    }
    free_dead_stack();

    sys.current = getNext(sys.current);
    if(oldThread == sys.current)
        return;
    i = SetJump(&oldThread->thread.sig_context);
    if( i  ==  0)
    {
    //EnableInterrupt ();
        gBS->RestoreTPL(TPL_APPLICATION);
        LongJump(& sys.current->thread.sig_context, 1);
    }
    __sync_bool_compare_and_swap (&Inschedule, 1, 0);
    //pebp();
    // Print(L"Haha, I am resuming\n");
    //while(1);
}

VOID
ThreadTimerHandler(
        IN EFI_EVENT                Event,
        IN VOID                     *Context
        )
{
    Skedule();
}

void start_thread(dmthread_t*  thread)
{
    UINTN i = 0;
    ptr_size* gsp = 0;
    ptr_size* gbp = 0;
    int disp;
    char *  stack_btm;
    __asm( "mov %%" bp_register ",%0":"=m"(gbp):); 
    __asm( "mov %%" sp_register ",%0":"=m"(gsp):);   

    gThreads++; 
    gBS->AllocatePool(EfiBootServicesData, STACK_SIZE + 128, (void**)&thread->stack);

    disp = (char*)gbp - (char*)gsp;
    stack_btm  =  ((char*)thread->stack)+  STACK_SIZE - (gbp - gsp + 4)*sizeof(ptr_size);
    for(i =0;i<disp /sizeof(ptr_size)+ 4;i++){
        ((ptr_size*)stack_btm)[i] = gsp[i];
    }
    gbp = (ptr_size*)((char*)stack_btm + disp);
    //Print(L"Stack :%p\n", gbp);
    __asm( "mov %0, %%" sp_register ::"m"(stack_btm)); 
    __asm( "mov %0, %%" bp_register ::"m"(gbp)); 

    i = SetJump(&thread->sig_context);
    if( i > 0)
    {
        __sync_bool_compare_and_swap (&Inschedule, 1, 0);
        (thread->kernel)(thread->arg); 

        if  (sys.threads -> next ){
            sys.current->thread.status=STATUS_DEAD;
            gThreads --;
            // skedule
            Skedule();
        }else{
            *(int*)0 = 0;
        }

        // exit threads 
    }
}

void initTimer()
{
    EFI_STATUS  Status;
    Status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)Skedule, (VOID*)NULL, &myEvent);
    Status = gBS->SetTimer(myEvent,TimerPeriodic , 1 * 1000 * 1000);
}

void closeTimer()
{
    gBS->CloseEvent(myEvent);
    myEvent = 0;
}

void stopTimer()
{
    closeTimer();
}

void resumeTimer()
{
    initTimer();
}

void create_thread(thread_func_t f, void *  arg)
{
    // __asm enter // or __asm push ebp; mov esp, ebp
    thread_list *  new_thread;
    if(  myEvent == 0){
        initTimer();
    }
    new_thread = _new_thread(f, arg);
    if(sys.threads == 0){
        thread_list *  main_thread = _new_thread(0,0);
        sys.threads = sys.current = main_thread;
    }
    _insert_thread(new_thread, sys.threads->prev);
    sys.current = new_thread;
    start_thread(& new_thread->thread);
    sys.current = sys.threads;
// Thanks there is leave here, then the esp is correct
//     // __asm leave // or __asm mov ebp esp; pop ebp
}

void thread_join()
{
    while(sys.current->next && (sys.current->next != sys.current)){
        Skedule();
    }
    closeTimer();
}
