/*
 * =====================================================================================
 *
 *       Filename:  GcppCrt.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/17/2013 07:50:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

extern "C" void Print(const CHAR16*, ...);

typedef void (*func_ptr) (void);

func_ptr __CTOR_LIST__[1]
  __attribute__ ((__used__, section(".ctors"), aligned(sizeof(func_ptr))))
  = { (func_ptr) (-1) };


func_ptr __CTOR_END__[1]
  __attribute__((__used__, section(".ctors"), aligned(sizeof(func_ptr))))
  = { (func_ptr) 0 };

func_ptr __DTOR_LIST__[1]
  __attribute__ ((__used__, section(".dtors"), aligned(sizeof(func_ptr))))
  = { (func_ptr) (-1) };


func_ptr __DTOR_END__[1]
  __attribute__((__used__, section(".dtors"), aligned(sizeof(func_ptr))))
  = { (func_ptr) 0 };


 

    extern "C"
    void __attribute__((used))
__do_global_ctors_aux (void)
{
    func_ptr *p =  __CTOR_END__ - 1;
    //for (p = __CTOR_END__ - 1 ; p >= __CTOR_LIST__; p--)
    for (p = __CTOR_LIST__; p < __CTOR_END__ ; p++)
    {
        if (*p && *p != (func_ptr)-1)
            (*p) ();
    }
    {
        func_ptr *p =  __CTOR_END__ - 1;
        int i = 0;
        for(i = -5; i< 5; i++)
        {
            Print((const CHAR16*)L"%2d: %p %p\n", i,&p[i],  p[i]);

        }
        Print((const CHAR16*)L"[%p %p]\n", __CTOR_LIST__, __CTOR_END__);
    }
}


    extern "C"
    void __attribute__((used))
__do_global_dtors_aux (void)
{
    func_ptr *p =  __DTOR_END__ - 1;
    for (p = __DTOR_END__ - 1 ; p >= __DTOR_LIST__; p--)
    //for (p = __DTOR_LIST__; p < __DTOR_END__ ; p++)
    {
        if (*p && *p != (func_ptr)-1)
            (*p) ();
    }
    {
        func_ptr *p =  __DTOR_END__ - 1;
        int i = 0;
        for(i = -5; i< 5; i++)
        {
            Print((const CHAR16*)L"%2d: %p %p\n", i,&p[i],  p[i]);

        }
        Print((const CHAR16*)L"[%p %p]\n", __DTOR_LIST__, __DTOR_END__);
    }
}

int cppMain (  IN int Argc,  IN char **Argv  );

int
main (
  IN int Argc,
  IN char **Argv
  )
{
    int result;
    __do_global_ctors_aux ();

    result = cppMain(Argc, Argv);

    __do_global_dtors_aux ();

    return result;
}
