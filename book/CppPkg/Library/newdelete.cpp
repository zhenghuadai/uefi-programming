#include "StdUefi.h"

__inline__ void *  operator new( size_t cb )
{
    void *res;
    res = malloc(cb);
    return res;
}


__inline__ void *  operator new[]( size_t cb )
{
    void *res = operator new(cb);
    return res;
}

__inline__ void operator delete( void * p )
{ 
    free( p );
}

__inline__ void operator delete[]( void * p )
{
    operator delete(p);
}

