#include "listing_0070_platform_metrics.c"

#define LOG 0

#if LOG
#define logf printf
#else
#define logf(...) (void)0
#endif

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
