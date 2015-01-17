/*
 * =====================================================================================
 *
 *       Filename:  math.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/10/2012 09:50:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <math.h>

float sqrtf(float s)   { return (float)sqrt((double)s);}
float cosf(float s)    { return (float)cos ((double)s);}
float sinf(float s)    { return (float)sin ((double)s);}
float logf(float s)    { return (float)log ((double)s);}
float log2f(float s)   { return (float)log2 ((double)s);}
float log10f(float s)  { return (float)log10 ((double)s);}
float lroundf(float s)  { return (float)lround ((double)s);}
float atanf(float s)   { return (float)atan  ((double)s);}
float cbrtf(float s)   { return (float)cbrt  ((double)s);}
float powf (float s,float s2)   { return (float)pow   ((double)s, (double)s2);}
float expf (float s)   { return (float)exp   ((double)s);}
float floorf(float s)  { return (float)floor ((double)s);}
float truncf(float s)  { return (float)trunc ((double)s);}
float ldexpf(float s,float s2)   { return (float)ldexpf   ((double)s, (double)s2);}

long int lrintf(float x)
{
    return (int)(rint(x));
}

void sincos(double x, double *_sin, double *_cos)
{
    *_sin = sin(x);
    *_cos = cos(x);
}
void sincosf(float x, float *_sin,float  *_cos)
{
    *_sin = sinf(x);
    *_cos = cosf(x);
}

double rint(double s) { return floor(s);}

long lrint(double s) { return (long) floor(s);}
long long llrint(double s) { return (long long) floor(s);}
#if 0
    float
__nanf (const char *tagp)
{
    if (tagp[0] != '\0')
    {
        char buf[6 + strlen (tagp)];
        sprintf (buf, "NAN(%s)", tagp);
        return strtof (buf, NULL);
    }

    return NAN;
}
#endif
const union __float_u __nanf =
#if BYTE_ORDER == BIG_ENDIAN
        { { 0x7f, 0xc0,    0,    0 } };
#else
        { {    0,    0, 0xc0, 0x7f } };
#endif
