#include "haversine_shared.h"
#include "listing_0065_haversine_formula.c"

r64 CLUSTER_WIDTH = 10.0;

#define EARTH_RADIUS 6372.8

static inline r64
Random01()
{
    r64 Result = rand() * INV_RAND_MAX;
    return Result;
}

static inline r64
RandomBetween(r64 Min, r64 Max)
{
    r64 Result = Min + Random01() * (Max - Min);
    return Result;
}

int main(int ArgCount, char **Args)
{
    FILE *Dump = fopen("haversine_gen.r64", "wb");
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
            r64 Result = ReferenceHaversine(ATheta, APsi, BTheta, BPsi, EARTH_RADIUS);
            fwrite(&Result, sizeof(Result), 1, Dump);
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
