#include "haversine_shared.h"
#include "haversine_memory.h"
#include "listing_0065_haversine_formula.c"
#include "json_parse.c"
#include "profile.h"

#define EARTH_RADIUS 6372.8

int main(int ArgCount, char **Args)
{
    BeginProfile();
    if (ArgCount == 2)
    {
        FILE *Dump = fopen("haversine_calc.r64", "wb");
        struct memory_arena Arena;
        char *Memory = malloc(Gigabyte(8));
        InitializeArena(&Arena, Gigabyte(8), Memory);

        TIMER_START(Read);
        FILE *Input = fopen(Args[1], "rb");
        struct memory_arena FileBufferArena = SubArena(&Arena, fread(Arena.Memory, 1, Arena.Size, Input));
        TIMER_END(Read);
        struct memory_arena ParseArena = SubArena(&Arena, Arena.Size - Arena.Used);
        TIMER_START(Parse);
        struct json_element *Root = ParseJson(FileBufferArena, &ParseArena);
        TIMER_END(Parse);
        Assert(Root);
        struct json_element *Pairs = GetElement(Root, "pairs");
        Assert(Pairs);
        struct json_element *Pair = Pairs->FirstChild;
        r64 Sum = 0.0;
        u64 NumPairs = 0;
        TIMER_START(Sum);
        while (Pair)
        {
            ++NumPairs;
            r64 X0 = GetElement(Pair, "x0")->Number;
            r64 Y0 = GetElement(Pair, "y0")->Number;
            r64 X1 = GetElement(Pair, "x1")->Number;
            r64 Y1 = GetElement(Pair, "y1")->Number;
            Sum += ReferenceHaversine(X0, Y0, X1, Y1, EARTH_RADIUS);
            Pair = Pair->Sibling;
        }
        TIMER_END(Sum);
        r64 Mean = Sum / NumPairs;
        printf("NumPairs: %llu\n", NumPairs);
        printf("Mean: %f\n", Mean);
    }
    else
    {
        printf("usage: %s input.json\n", Args[0]);
        exit(1);
    }
    EndProfile();
    return 0;
}
