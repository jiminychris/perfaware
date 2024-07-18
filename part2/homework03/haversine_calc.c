#include "haversine_shared.h"
#include "haversine_memory.h"
#include "listing_0065_haversine_formula.c"
#include "json_parse.c"
#include "profile.h"

#define EARTH_RADIUS 6372.8

static inline void PrintCycles(char *Label, u64 Cycles, r64 CyclesToPercentage)
{
    printf("  %s: %llu (%.2f%%)\n", Label, Cycles, Cycles * CyclesToPercentage);
}

int main(int ArgCount, char **Args)
{
    if (ArgCount == 2)
    {
        u64 TimerStart = ReadCPUTimer();
        FILE *Dump = fopen("haversine_calc.r64", "wb");
        struct memory_arena Arena;
        char *Memory = malloc(Gigabyte(8));
        InitializeArena(&Arena, Gigabyte(8), Memory);
        u64 TimerStartupEnd = ReadCPUTimer();

        FILE *Input = fopen(Args[1], "rb");
        struct memory_arena FileBufferArena = SubArena(&Arena, fread(Arena.Memory, 1, Arena.Size, Input));
        u64 TimerReadEnd = ReadCPUTimer();
        struct memory_arena ParseArena = SubArena(&Arena, Arena.Size - Arena.Used);
        struct json_element *Root = ParseJson(FileBufferArena, &ParseArena);
        u64 TimerParseEnd = ReadCPUTimer();
        Assert(Root);
        struct json_element *Pairs = GetElement(Root, "pairs");
        Assert(Pairs);
        struct json_element *Pair = Pairs->FirstChild;
        r64 Sum = 0.0;
        u64 NumPairs = 0;
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
        u64 TimerSumEnd = ReadCPUTimer();
        r64 Mean = Sum / NumPairs;
        printf("NumPairs: %llu\n", NumPairs);
        printf("Mean: %f\n", Mean);
        u64 TimerOutputEnd = ReadCPUTimer();

        u64 CPUFrequency = GetCPUFrequency(100);
        r64 CyclesToMilliseconds = 1000.0 / CPUFrequency;
        u64 TotalCycles = (TimerOutputEnd - TimerStart);
        r64 CyclesToPercentage = 100.0f / TotalCycles;
        printf("Total Time: %fms (CPU freq %llu)\n", TotalCycles * CyclesToMilliseconds, CPUFrequency);
        PrintCycles("Startup", TimerStartupEnd - TimerStart, CyclesToPercentage);
        PrintCycles("Read", TimerReadEnd - TimerStartupEnd, CyclesToPercentage);
        PrintCycles("Parse", TimerParseEnd - TimerReadEnd, CyclesToPercentage);
        PrintCycles("Sum", TimerSumEnd - TimerParseEnd, CyclesToPercentage);
        PrintCycles("Output", TimerOutputEnd - TimerSumEnd, CyclesToPercentage);
    }
    else
    {
        printf("usage: %s input.json\n", Args[0]);
        exit(1);
    }
    return 0;
}
