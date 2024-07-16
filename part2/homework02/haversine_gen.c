#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

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

r64 CLUSTER_WIDTH = 10.0;
r64 INV_RAND_MAX = 1.0 / (r64)RAND_MAX;

inline r64
Random01()
{
    r64 Result = rand() * INV_RAND_MAX;
    return Result;
}

inline r64
RandomBetween(r64 Min, r64 Max)
{
    r64 Result = Min + Random01() * (Max - Min);
    return Result;
}

int main(int ArgCount, char **Args)
{
    if (ArgCount == 3)
    {
        unsigned int Seed = atoi(Args[1]);
        u32 Count = atoi(Args[2]);
        srand(Seed);
        r64 OriginATheta = Random01() * 360;
        r64 OriginAPsi   = Random01() * 180;
        r64 OriginBTheta = Random01() * 360;
        r64 OriginBPsi   = Random01() * 180;
        printf("{\n");
        printf("  \"pairs\": [\n");
        char *Endl[] = {"",","};
        while (Count--)
        {
            r64 ATheta = fmod(OriginATheta + RandomBetween(-CLUSTER_WIDTH, CLUSTER_WIDTH), 360.0);
            r64 APsi = fmod(OriginAPsi + RandomBetween(-CLUSTER_WIDTH, CLUSTER_WIDTH), 180.0);
            r64 BTheta = fmod(OriginBTheta + RandomBetween(-CLUSTER_WIDTH, CLUSTER_WIDTH), 360.0);
            r64 BPsi = fmod(OriginBPsi + RandomBetween(-CLUSTER_WIDTH, CLUSTER_WIDTH), 180.0);
            printf("    {\"x0\": %f, \"y0\": %f, \"x1\": %f, \"y1\": %f}%s\n", ATheta, APsi, BTheta, BPsi, Endl[!!Count]);
        }
        printf("  ]\n");
        printf("}\n");
    }
    else
    {
        printf("usage: %s seed count\n", Args[0]);
        exit(1);
    }
    return 0;
}
