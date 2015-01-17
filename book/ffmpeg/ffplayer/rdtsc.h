#ifndef __MDTIME_HEADER__
#define __MDTIME_HEADER__

#ifndef CLOCK_NUM
#define CLOCK_NUM 10
#endif

#ifdef _cplusplus
#define DEFAULT(x) =x
#else
#define DEFAULT(x) 
#endif

#if defined(__GNUC__)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define rdtsc(low,high) \
__asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high));

#define rdtsc1(t) \
__asm__ __volatile__("rdtsc" : "=a" (*(int*)&t), "=d" (*(((int*)&t)+1)));

#define nasm0(op) __asm__( #op)
#elif defined(_MSC_VER)
#define rdtsc(low32, high32) \
__asm rdtsc \
__asm mov low32, eax \
__asm mov high32, edx 

#define rdtsc1(t) \
__asm rdtsc \
__asm mov t, eax \
__asm mov t+4, edx 

#define nasm0(x) __asm x

#endif

typedef unsigned long long U64;
#define diffTime(start,end) ((end)-(start))
#define cast2u64(x) x

#define timens(x) timenamespace ## x

static inline U64  getrdtsc(){
	nasm0(rdtsc);
}

static U64 startT[CLOCK_NUM], stopT[CLOCK_NUM];
static inline U64 startTime(int cid  DEFAULT(0)){
		startT[cid] = getrdtsc();
		return startT[cid];
}

static inline U64 stopTime(int cid DEFAULT(0)){
		stopT[cid] = getrdtsc();
		return (U64) diffTime(startT[cid],stopT[cid]);
}

static inline U64 getTime(int cid DEFAULT(0)){
	return (U64) diffTime(startT[cid],stopT[cid]);
}

static inline U64 pTime(int cid DEFAULT(0)){
    U64 t = diffTime(startT[cid], stopT[cid]);
    printf("time:%ld\n", t);
	return t;
}

static inline double mdtime(int id){
	static U64 startT, stopT;
	if(id){
		rdtsc1(stopT);
		return (double)(diffTime(startT,stopT));
	}else{
		rdtsc1(startT);
		return (double) cast2u64(startT);
	}
}

static inline double pdtime(int id){
	static U64 startT, stopT;
	if(id){
		rdtsc1(stopT);
		printf("time:%ld\n",diffTime(startT,stopT));
		return (double)(diffTime(startT,stopT));
	}else{
		rdtsc1(startT);
		return (double) cast2u64(startT);
	}
}

#if defined(__GNUC__)
static double getFrequency()
{
	double t3;
	U64 startT, stopT;
	rdtsc1(startT);
	sleep(1);
	rdtsc1(stopT);
	t3=  (double)(diffTime(startT,stopT));
	return t3;
}
#else
static double getFrequency()
{
    U64 freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    return freq;
}
#endif
static double RDTSC_CORE_FREQ = 0;
static inline double getSeconds(double c)
{
    if (RDTSC_CORE_FREQ ==0) RDTSC_CORE_FREQ = getFrequency();
    return c/ RDTSC_CORE_FREQ;
}

static inline U64 getmSeconds(int cid DEFAULT(=0)){
    if (RDTSC_CORE_FREQ ==0) RDTSC_CORE_FREQ = getFrequency();
	return (U64) diffTime(startT[cid],stopT[cid]) * 1000 / RDTSC_CORE_FREQ;
}

#endif
