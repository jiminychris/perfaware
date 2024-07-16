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

#include "listing_0065_haversine_formula.c"
#include "json_parse.c"

#define FILE_BUFFER_SIZE 1024*1024*1024
#define EARTH_RADIUS 6372.8
#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

int main(int ArgCount, char **Args)
{
    if (ArgCount == 2)
    {
        FILE *Input = fopen(Args[1], "rb");
        char *Memory = malloc(FILE_BUFFER_SIZE);
        u64 Size = fread(Memory, 1, FILE_BUFFER_SIZE, Input);
        struct token_stream_builder Builder = Tokenize(Size, Memory);
        printf("TokenCount: %lu\n", Builder.Count);
        printf("StringPoolSize: %llu\n", Builder.StringPoolSize);
//    ReferenceHaversine(X0, Y0, X1, Y1, EARTH_RADIUS);
    }
    else
    {
        printf("usage: %s input.json\n", Args[0]);
        exit(1);
    }
    return 0;
}
