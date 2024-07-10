#include "stdio.h"

#define REG_MODE_SOURCE 0
#define REG_MODE_DESTINATION 1

#define DATA_SIZE_BYTE 0
#define DATA_SIZE_WORD 1

#define MODE_MEMORY_DISPLACEMENT_NONE 0
#define MODE_MEMORY_DISPLACEMENT_8_BIT 1
#define MODE_MEMORY_DISPLACEMENT_16_BIT 2
#define MODE_REGISTER 3

char *RegisterLookup[] =
{
    "AL",
    "CL",
    "DL",
    "BL",
    "AH",
    "CH",
    "DH",
    "BH",
    "AX",
    "CX",
    "DX",
    "BX",
    "SP",
    "BP",
    "SI",
    "DI",
};
char *MemoryModeLookup[] =
{
    "BX + SI",
    "BX + DI",
    "BP + SI",
    "BP + DI",
    "SI",
    "DI",
    "BP",
    "BX",
};

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

int main(int ArgCount, char **Args)
{
    if (2 == ArgCount)
    {
        char *FileName = Args[1];
        printf("; Disassembly for %s\n", FileName);
        printf("bits 16\n");
        FILE *MachineCodeFile = fopen(FileName, "rb");
        unsigned char InstructionStream[2048];
        unsigned char Byte0;
        size_t BytesRead = fread(InstructionStream, 1, ArrayCount(InstructionStream), MachineCodeFile);
        int InstructionIndex = 0;
        while (InstructionIndex < BytesRead)
        {
            Byte0 = InstructionStream[InstructionIndex++];
            // Immediate to register
            // Checking if Byte0 matches 1011wreg
            if (0xB0 == (Byte0 & 0xF0))
            {
                unsigned char DataSize = (Byte0 >> 3) & 1;
                unsigned char RegIndex = Byte0 & 0xF;
                int Data = InstructionStream[InstructionIndex++];
                if (DataSize == DATA_SIZE_WORD)
                {
                    Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                }
                printf("mov %.2s, %d\n", RegisterLookup[RegIndex], Data);
            }
            // Register/memory to/from register
            // Checking if Byte0 matches 100010dw
            else if (0x88 == (Byte0 & 0xFC))
            {
                unsigned char Byte1 = InstructionStream[InstructionIndex++];
                unsigned char RegMode = (Byte0 >> 1) & 1;
                unsigned char DataSize = Byte0 & 1;
                unsigned char Mode = (Byte1 >> 6);
                unsigned char Reg = (Byte1 >> 3) & 7;
                unsigned char RM = Byte1 & 7;
                unsigned char RegIndex = Reg | (DataSize << 3);
                char MemoryAddressBuffer[256];
                char MemoryAddressOperandBuffer[256];
                char *Operands[2];
                switch (Mode)
                {
                    case MODE_MEMORY_DISPLACEMENT_NONE:
                    {
                        char *MemoryAddress = MemoryModeLookup[RM];
                        if (6 == RM) {
                            unsigned char LowByte = InstructionStream[InstructionIndex++];
                            unsigned char HighByte = InstructionStream[InstructionIndex++];
                            int Data = (HighByte << 8) | LowByte;
                            if (Data)
                            {
                                snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%u", Data);
                                MemoryAddress = MemoryAddressBuffer;
                            }
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        Operands[!RegMode] = RegisterLookup[RegIndex];
                        Operands[RegMode] = MemoryAddressOperandBuffer;
                        printf("mov %s, %s\n", Operands[0], Operands[1]);
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_8_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[RM];
                        unsigned char Data = InstructionStream[InstructionIndex++];
                        if (Data)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hhd", MemoryAddress, Data);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        Operands[!RegMode] = RegisterLookup[RegIndex];
                        Operands[RegMode] = MemoryAddressOperandBuffer;
                        printf("mov %s, %s\n", Operands[0], Operands[1]);
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_16_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[RM];
                        unsigned char LowByte = InstructionStream[InstructionIndex++];
                        unsigned char HighByte = InstructionStream[InstructionIndex++];
                        int Data = HighByte << 8 | LowByte;
                        if (Data)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hd", MemoryAddress, Data);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        Operands[!RegMode] = RegisterLookup[RegIndex];
                        Operands[RegMode] = MemoryAddressOperandBuffer;
                        printf("mov %s, %s\n", Operands[0], Operands[1]);
                    } break;

                    case MODE_REGISTER:
                    {
                        unsigned char RMIndex = RM | (DataSize << 3);
                        Operands[!RegMode] = RegisterLookup[RegIndex];
                        Operands[RegMode] = RegisterLookup[RMIndex];
                        printf("mov %s, %s\n", Operands[0], Operands[1]);
                    } break;

                    default:
                    {
                        printf("Encountered unsupported mode 0x%02x\n", Mode);
                    } break;
                }
            }
            // Memory to accumulator
            // Checking if Byte0 matches 1010000w
            else if (0xa0 == (Byte0 & 0xfe))
            {
                unsigned char DataSize = Byte0 & 1;
                unsigned char AddressLo = InstructionStream[InstructionIndex++];
                unsigned char AddressHi = InstructionStream[InstructionIndex++];
                int Address = (AddressHi << 8) | AddressLo;
                printf("mov %s, [%u]\n", RegisterLookup[DataSize << 3], Address);
            }
            // Memory from accumulator
            // Checking if Byte0 matches 1010001w
            else if (0xa2 == (Byte0 & 0xfe))
            {
                unsigned char DataSize = Byte0 & 1;
                unsigned char AddressLo = InstructionStream[InstructionIndex++];
                unsigned char AddressHi = InstructionStream[InstructionIndex++];
                int Address = (AddressHi << 8) | AddressLo;
                printf("mov [%u], %s\n", Address, RegisterLookup[DataSize << 3]);
            }
            // Immediate to register/memory
            // Checking if Byte0 matches 11000110w
            else if (0xc6 == (Byte0 & 0xFE))
            {
                unsigned char Byte1 = InstructionStream[InstructionIndex++];
                unsigned char DataSize = Byte0 & 1;
                unsigned char Mode = (Byte1 >> 6);
                unsigned char RM = Byte1 & 7;
                char MemoryAddressBuffer[256];
                char DataBuffer[256];
                switch (Mode)
                {
                    case MODE_MEMORY_DISPLACEMENT_NONE:
                    {
                        char *MemoryAddress = MemoryModeLookup[RM];
                        if (6 == RM) {
                            unsigned char LowByte = InstructionStream[InstructionIndex++];
                            unsigned char HighByte = InstructionStream[InstructionIndex++];
                            int Disp = (HighByte << 8) | LowByte;
                            if (Disp)
                            {
                                snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%u", Disp);
                                MemoryAddress = MemoryAddressBuffer;
                            }
                        }
                        int Data = InstructionStream[InstructionIndex++];
                        if (DataSize == DATA_SIZE_WORD)
                        {
                            Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                            snprintf(DataBuffer, sizeof(DataBuffer), "word %d", Data);
                        }
                        else
                        {
                            snprintf(DataBuffer, sizeof(DataBuffer), "byte %d", Data);
                        }
                        printf("mov [%s], %s\n", MemoryAddress, DataBuffer);
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_8_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[RM];
                        unsigned char Disp = InstructionStream[InstructionIndex++];
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hhd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        int Data = InstructionStream[InstructionIndex++];
                        if (DataSize == DATA_SIZE_WORD)
                        {
                            Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                            snprintf(DataBuffer, sizeof(DataBuffer), "word %d", Data);
                        }
                        else
                        {
                            snprintf(DataBuffer, sizeof(DataBuffer), "byte %d", Data);
                        }
                        printf("mov [%s], %s\n", MemoryAddress, DataBuffer);
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_16_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[RM];
                        unsigned char LowByte = InstructionStream[InstructionIndex++];
                        unsigned char HighByte = InstructionStream[InstructionIndex++];
                        int Disp = HighByte << 8 | LowByte;
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        int Data = InstructionStream[InstructionIndex++];
                        if (DataSize == DATA_SIZE_WORD)
                        {
                            Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                            snprintf(DataBuffer, sizeof(DataBuffer), "word %d", Data);
                        }
                        else
                        {
                            snprintf(DataBuffer, sizeof(DataBuffer), "byte %d", Data);
                        }
                        printf("mov [%s], %s\n", MemoryAddress, DataBuffer);
                    } break;

                    case MODE_REGISTER:
                    {
                        unsigned char RMIndex = RM | (DataSize << 3);
                        int Data = InstructionStream[InstructionIndex++];
                        if (DataSize == DATA_SIZE_WORD)
                        {
                            Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                        }
                        printf("mov %s, %d\n", RegisterLookup[RMIndex], Data);
                    } break;

                    default:
                    {
                        printf("Encountered unsupported mode 0x%02x\n", Mode);
                    } break;
                }
            }
            else
            {
                printf("Encountered unknown opcode 0x%02x\n", Byte0);
            }
        }
    }
    else
    {
        printf("Please provide a single path argument to a machine code file.");
    }
}
