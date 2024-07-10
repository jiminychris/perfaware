#include "stdio.h"

#define REG_MODE_SOURCE 0
#define REG_MODE_DESTINATION 1

#define DATA_SIZE_BYTE 0
#define DATA_SIZE_WORD 1

#define MODE_MEMORY_DISPLACEMENT_NONE 0
#define MODE_MEMORY_DISPLACEMENT_8_BIT 1
#define MODE_MEMORY_DISPLACEMENT_16_BIT 2
#define MODE_REGISTER 3

char *OperatorLookup[] =
{
    "ADD",
    "???",
    "???",
    "???",
    "???",
    "SUB",
    "???",
    "CMP",
};
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
char *JumpLookup[256];

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

int main(int ArgCount, char **Args)
{
    JumpLookup[0x75] = "jnz";
    JumpLookup[0x74] = "je";
    JumpLookup[0x7C] = "jl";
    JumpLookup[0x7E] = "jle";
    JumpLookup[0x72] = "jb";
    JumpLookup[0x76] = "jbe";
    JumpLookup[0x7A] = "jp";
    JumpLookup[0x70] = "jo";
    JumpLookup[0x78] = "js";
    JumpLookup[0x7D] = "jnl";
    JumpLookup[0x7F] = "jg";
    JumpLookup[0x73] = "jnb";
    JumpLookup[0x77] = "ja";
    JumpLookup[0x7B] = "jnp";
    JumpLookup[0x71] = "jno";
    JumpLookup[0x79] = "jns";
    JumpLookup[0xE2] = "loop";
    JumpLookup[0xE1] = "loopz";
    JumpLookup[0xE0] = "loopnz";
    JumpLookup[0xE3] = "jcxz";
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
                printf("mov %s, %d\n", RegisterLookup[RegIndex], Data);
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
                unsigned char R_M = Byte1 & 7;
                unsigned char RegIndex = Reg | (DataSize << 3);
                char MemoryAddressBuffer[256];
                char MemoryAddressOperandBuffer[256];
                char *Operands[2];
                switch (Mode)
                {
                    case MODE_MEMORY_DISPLACEMENT_NONE:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        if (6 == R_M) {
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
                        char *MemoryAddress = MemoryModeLookup[R_M];
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
                        char *MemoryAddress = MemoryModeLookup[R_M];
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
                        unsigned char R_MIndex = R_M | (DataSize << 3);
                        Operands[!RegMode] = RegisterLookup[RegIndex];
                        Operands[RegMode] = RegisterLookup[R_MIndex];
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
                unsigned char R_M = Byte1 & 7;
                char MemoryAddressBuffer[256];
                char DataBuffer[256];
                switch (Mode)
                {
                    case MODE_MEMORY_DISPLACEMENT_NONE:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        if (6 == R_M) {
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
                        char *MemoryAddress = MemoryModeLookup[R_M];
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
                        char *MemoryAddress = MemoryModeLookup[R_M];
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
                        unsigned char R_MIndex = R_M | (DataSize << 3);
                        int Data = InstructionStream[InstructionIndex++];
                        if (DataSize == DATA_SIZE_WORD)
                        {
                            Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                        }
                        printf("mov %s, %d\n", RegisterLookup[R_MIndex], Data);
                    } break;

                    default:
                    {
                        printf("Encountered unsupported mode 0x%02x\n", Mode);
                    } break;
                }
            }
            // Reg/memory with register to either
            // Matching pattern 00xxx0dw
            else if (0 == (Byte0 & 0xC4))
            {
                unsigned char RegMode = (Byte0 >> 1) & 1;
                unsigned char DataSize = Byte0 & 1;
                unsigned char Byte1 = InstructionStream[InstructionIndex++];
                unsigned char Mode = (Byte1 >> 6) & 0x3;
                unsigned char Reg = (Byte1 >> 3) & 0x7;
                unsigned char R_M = (Byte1 >> 0) & 0x7;
                unsigned char OperatorIndex = (Byte0 >> 3) & 7;
                unsigned char RegIndex = Reg | (DataSize << 3);
                char *Operator = OperatorLookup[OperatorIndex];
                char MemoryAddressBuffer[256];
                char MemoryAddressOperandBuffer[256];
                char *Operands[2];
                switch (Mode)
                {
                    case MODE_MEMORY_DISPLACEMENT_NONE:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        if (6 == R_M) {
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
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_8_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        unsigned char Data = InstructionStream[InstructionIndex++];
                        if (Data)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hhd", MemoryAddress, Data);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        Operands[!RegMode] = RegisterLookup[RegIndex];
                        Operands[RegMode] = MemoryAddressOperandBuffer;
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_16_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
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
                    } break;

                    case MODE_REGISTER:
                    {
                        unsigned char R_MIndex = R_M | (DataSize << 3);
                        Operands[!RegMode] = RegisterLookup[RegIndex];
                        Operands[RegMode] = RegisterLookup[R_MIndex];
                    } break;

                    default:
                    {
                        printf("Encountered unsupported mode 0x%02x\n", Mode);
                    } break;
                }
                printf("%s %s, %s\n", Operator, Operands[0], Operands[1]);
            }
            // Immediate to register/memory
            // Matching pattern 100000sw
            else if (0x80 == (Byte0 & 0xFC))
            {
                unsigned char SignExtension = (Byte0 >> 1) & 1;
                unsigned char DataSize = Byte0 & 1;
                unsigned IsWord = (Byte0 & 3) == 1;
                unsigned char Byte1 = InstructionStream[InstructionIndex++];
                unsigned char Mode = (Byte1 >> 6) & 0x3;
                unsigned char R_M = (Byte1 >> 0) & 0x7;
                char *Operands[2];
                char MemoryAddressBuffer[256];
                char MemoryAddressOperandBuffer[256];
                char DataBuffer[256];
                switch (Mode)
                {
                    case MODE_MEMORY_DISPLACEMENT_NONE:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        if (6 == R_M) {
                            unsigned char LowByte = InstructionStream[InstructionIndex++];
                            unsigned char HighByte = InstructionStream[InstructionIndex++];
                            int Disp = (HighByte << 8) | LowByte;
                            if (Disp)
                            {
                                snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%u", Disp);
                                MemoryAddress = MemoryAddressBuffer;
                            }
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        int Data = InstructionStream[InstructionIndex++];
                        if (IsWord)
                        {
                            Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                        }
                        snprintf(DataBuffer, sizeof(DataBuffer), "%s %d", DataSize ? "word" : "byte", Data);
                        Operands[0] = MemoryAddressOperandBuffer;
                        Operands[1] = DataBuffer;
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_8_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        unsigned char Disp = InstructionStream[InstructionIndex++];
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hhd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        int Data = InstructionStream[InstructionIndex++];
                        if (IsWord)
                        {
                            Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                        }
                        snprintf(DataBuffer, sizeof(DataBuffer), "%s %d", DataSize ? "word" : "byte", Data);
                        Operands[0] = MemoryAddressOperandBuffer;
                        Operands[1] = DataBuffer;
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_16_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        unsigned char LowByte = InstructionStream[InstructionIndex++];
                        unsigned char HighByte = InstructionStream[InstructionIndex++];
                        int Disp = HighByte << 8 | LowByte;
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        int Data = InstructionStream[InstructionIndex++];
                        if (IsWord)
                        {
                            Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                        }
                        snprintf(DataBuffer, sizeof(DataBuffer), "%s %d", DataSize ? "word" : "byte", Data);
                        Operands[0] = MemoryAddressOperandBuffer;
                        Operands[1] = DataBuffer;
                    } break;

                    case MODE_REGISTER:
                    {
                        unsigned char R_MIndex = R_M | (DataSize << 3);
                        int Data = InstructionStream[InstructionIndex++];
                        if (IsWord)
                        {
                            Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                        }
                        snprintf(DataBuffer, sizeof(DataBuffer), "%d", Data);
                        Operands[0] = RegisterLookup[R_MIndex];
                        Operands[1] = DataBuffer;
                    } break;

                    default:
                    {
                        printf("Encountered unsupported mode 0x%02x\n", Mode);
                    } break;
                }

                unsigned char OperatorIndex = (Byte1 >> 3) & 7;
                char *Operator = OperatorLookup[OperatorIndex];
                printf("%s %s, %s\n", Operator, Operands[0], Operands[1]);
            }
            // Immediate to accumulator
            // Matching pattern 00xxx10w
            else if (0x04 == (Byte0 & 0xC6))
            {
                unsigned char DataSize = Byte0 & 1;
                int Data = InstructionStream[InstructionIndex++];
                if (DataSize)
                {
                    Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                }
                unsigned char OperatorIndex = (Byte0 >> 3) & 7;
                char *Operator = OperatorLookup[OperatorIndex];
                char *RegisterName = RegisterLookup[DataSize << 3];
                printf("%s %s, %d\n", Operator, RegisterName, Data);
            }
            else if (JumpLookup[Byte0])
            {
                printf("%s ($+2)%+hhd\n", JumpLookup[Byte0], InstructionStream[InstructionIndex++]);
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
