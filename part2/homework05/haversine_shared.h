#if !defined(HAVERSINE_SHARED_H)

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "string.h"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef float    r32;
typedef double   r64;
typedef u32      b32;
typedef size_t memory_size;

r64 INV_RAND_MAX = 1.0 / (r64)RAND_MAX;

#define Assert(Expr) {if(!(Expr)) {int A = *((volatile int *)0);}}

#define Kilobyte(X) X*(u64)1024
#define Megabyte(X) Kilobyte(X*1024)
#define Gigabyte(X) Megabyte(X*1024)

static inline b32 IsSet(u32 Flags, u32 Bit)
{
    b32 Result = (Flags & Bit);
    return(Result);
}

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

#define HAVERSINE_SHARED_H
#endif
