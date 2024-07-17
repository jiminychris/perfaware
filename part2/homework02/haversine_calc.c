#include "haversine_shared.h"
#include "haversine_memory.h"
#include "listing_0065_haversine_formula.c"
#include "json_parse.c"

#define EARTH_RADIUS 6372.8

int main(int ArgCount, char **Args)
{
    if (ArgCount == 2)
    {
        FILE *Dump = fopen("haversine_calc.r64", "wb");
        struct memory_arena Arena;
        char *Memory = malloc(Gigabyte(1));
        InitializeArena(&Arena, Gigabyte(1), Memory);

        FILE *Input = fopen(Args[1], "rb");
        struct memory_arena FileBufferArena = SubArena(&Arena, fread(Arena.Memory, 1, Arena.Size, Input));
        struct memory_arena ParseArena = SubArena(&Arena, Arena.Size - Arena.Used);
        struct haversine_pair *Pairs;
        u64 PairCount = ParseHaversinePairs(FileBufferArena, &ParseArena, &Pairs);
        while (PairCount--)
        {
            struct haversine_pair Pair = *Pairs++;
            r64 Result = ReferenceHaversine(Pair.X0, Pair.Y0, Pair.X1, Pair.Y1, EARTH_RADIUS);
            fwrite(&Result, sizeof(Result), 1, Dump);
            printf("%f\n", Result);
        }
    }
    else
    {
        printf("usage: %s input.json\n", Args[0]);
        exit(1);
    }
    return 0;
}
