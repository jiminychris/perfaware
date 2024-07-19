#if !defined(HAVERSINE_MEMORY_H)

struct memory_arena
{
    memory_size Size;
    memory_size Used;
    void *Memory;
};

static inline void
InitializeArena(struct memory_arena *Arena, memory_size SizeInit, void *MemoryInit)
{
    Arena->Size = SizeInit;
    Arena->Used = 0;
    Arena->Memory = MemoryInit;
}

enum push_flags
{
    None = 0,
    PushFlag_Zero = 1 << 0,
};

static inline b32
HasRoom(struct memory_arena *Arena, memory_size Size)
{
    b32 Result = ((Arena->Used + Size) <= Arena->Size);
    return(Result);
}

#define HasRoomForStruct(Arena, type, ...) HasRoom(Arena, sizeof(type), ##__VA_ARGS__)
#define HasRoomForArray(Arena, Count, type, ...) HasRoom(Arena, Count*sizeof(type), ##__VA_ARGS__)

static inline void *
PushSizeWithFlags(struct memory_arena *Arena, memory_size Size, u32 Flags)
{
    void *Result = 0;
    if((Arena->Used + Size) > Arena->Size)
    {
        Assert(!"Allocated too much memory!");
    }
    else
    {
        Result = (u8 *)Arena->Memory + Arena->Used;
        Arena->Used += Size;
        if(IsSet(Flags, PushFlag_Zero))
        {
            u8 *End = (u8 *)Arena->Memory + Arena->Used;
            for(u8 *At = (u8 *)Result;
                At < End;
                ++At)
            {
                *At = 0;
            }
        }
    }

    return(Result);
}

static inline void *
PushSize(struct memory_arena *Arena, memory_size Size)
{
    void *Result = PushSizeWithFlags(Arena, Size, 0);
    return Result;
}

#define PushStruct(Arena, type, ...) ((type *)PushSize(Arena, sizeof(type), ##__VA_ARGS__))
#define PushArray(Arena, Count, type, ...) ((type *)PushSize(Arena, Count*sizeof(type), ##__VA_ARGS__))

static inline struct memory_arena
SubArena(struct memory_arena *Arena, memory_size Size)
{
    void *Memory = PushSize(Arena, Size);
    struct memory_arena Result;
    InitializeArena(&Result, Size, Memory);
    return(Result);
}

struct temporary_memory
{
    memory_size Used;
    struct memory_arena *Arena;
};

static inline struct temporary_memory
BeginTemporaryMemory(struct memory_arena *Arena)
{
    struct temporary_memory Result;
    Result.Used = Arena->Used;
    Result.Arena = Arena;
    return(Result);
}

static inline void
EndTemporaryMemory(struct temporary_memory Temp)
{
    Temp.Arena->Used = Temp.Used;
}

#define HAVERSINE_MEMORY_H
#endif
