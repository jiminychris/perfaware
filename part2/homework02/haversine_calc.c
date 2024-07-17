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
        struct json_element *Root = ParseJson(FileBufferArena, &ParseArena);
        Assert(Root);
        struct json_element *Pairs = GetElement(Root, "pairs");
        Assert(Pairs);
        struct json_element *Pair = Pairs->FirstChild;
        while (Pair)
        {
            r64 X0 = GetElement(Pair, "x0")->Number;
            r64 Y0 = GetElement(Pair, "y0")->Number;
            r64 X1 = GetElement(Pair, "x1")->Number;
            r64 Y1 = GetElement(Pair, "y1")->Number;
            r64 Result = ReferenceHaversine(X0, Y0, X1, Y1, EARTH_RADIUS);
            fwrite(&Result, sizeof(Result), 1, Dump);
            printf("%f\n", Result);
            Pair = Pair->Sibling;
        }
    }
    else
    {
        printf("usage: %s input.json\n", Args[0]);
        exit(1);
    }
    return 0;
}
