#include "listing_0070_platform_metrics.c"

#define LOG 0

#if LOG
#define logf printf
#else
#define logf(...) (void)0
#endif

struct profile_chunk
{
    u64 Start;
    u64 End;
    char *Name;
};

static u32 ProfileChunkCount = 0;
static struct profile_chunk ProfileChunks[1024] = {0};

u64 GetCPUFrequency(u64 MillisecondsToWait)
{
	u64 OSFreq = GetOSTimerFreq();
	logf("    OS Freq: %llu (reported)\n", OSFreq);

	u64 CPUStart = ReadCPUTimer();
	u64 OSStart = ReadOSTimer();
	u64 OSEnd = 0;
	u64 OSElapsed = 0;
	u64 OSWaitTime = OSFreq * MillisecondsToWait / 1000;
	while(OSElapsed < OSWaitTime)
	{
		OSEnd = ReadOSTimer();
		OSElapsed = OSEnd - OSStart;
	}
	
	u64 CPUEnd = ReadCPUTimer();
	u64 CPUElapsed = CPUEnd - CPUStart;
	u64 CPUFreq = 0;
	if(OSElapsed)
	{
		CPUFreq = OSFreq * CPUElapsed / OSElapsed;
	}
	
	logf("   OS Timer: %llu -> %llu = %llu elapsed\n", OSStart, OSEnd, OSElapsed);
	logf(" OS Seconds: %.4f\n", (f64)OSElapsed/(f64)OSFreq);
	
	logf("  CPU Timer: %llu -> %llu = %llu elapsed\n", CPUStart, CPUEnd, CPUElapsed);
	logf("   CPU Freq: %llu (guessed)\n", CPUFreq);
	
    return CPUFreq;
}

#define _TIMER_START(Handle, _Name)                                     \
    Assert(ProfileChunkCount > 0);                                      \
    struct profile_chunk *__##Handle = ProfileChunks + ProfileChunkCount++; \
    __##Handle->Start = ReadCPUTimer();                                 \
    __##Handle->Name = _Name
#define TIMER_START(Handle) _TIMER_START(Handle, #Handle)
#define TIMER_END(Handle) __##Handle->End = ReadCPUTimer()

static void
BeginProfile()
{
    ProfileChunks[0].Start = ReadCPUTimer();
    ProfileChunkCount = 1;
}

static inline void PrintCycles(char *Label, u64 Cycles, r64 CyclesToPercentage)
{
    printf("  %s: %llu (%.2f%%)\n", Label, Cycles, Cycles * CyclesToPercentage);
}

static void
EndProfile()
{
    Assert(ProfileChunkCount > 0);
    ProfileChunks[0].End = ReadCPUTimer();

    struct profile_chunk *Chunk = ProfileChunks;
    u64 CPUFrequency = GetCPUFrequency(100);
    r64 CyclesToMilliseconds = 1000.0 / CPUFrequency;
    u64 TotalCycles = Chunk->End - Chunk->Start;
    r64 CyclesToPercentage = 100.0f / TotalCycles;

    printf("Total Time: %fms (CPU freq %llu)\n", TotalCycles * CyclesToMilliseconds, CPUFrequency);
    
    Chunk++;
    while (--ProfileChunkCount)
    {
        PrintCycles(Chunk->Name, Chunk->End - Chunk->Start, CyclesToPercentage);
        Chunk++;
    }
}
