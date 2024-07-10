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
    "OR",
    "ADC",
    "SBB",
    "AND",
    "SUB",
    "XOR",
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
char *ParameterlessLookup[256];

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

int main(int ArgCount, char **Args)
{
    ParameterlessLookup[0x06] = "PUSH ES";
    ParameterlessLookup[0x07] = "POP ES";
    //
    ParameterlessLookup[0x0E] = "PUSH CS";
    //
    ParameterlessLookup[0x16] = "PUSH SS";
    ParameterlessLookup[0x17] = "POP SS";
    //
    ParameterlessLookup[0x1E] = "PUSH DS";
    ParameterlessLookup[0x1F] = "POP DS";
    //
    ParameterlessLookup[0x26] = "ES";
    ParameterlessLookup[0x27] = "DAA";
    //
    ParameterlessLookup[0x2E] = "CS";
    ParameterlessLookup[0x2F] = "DAS";
    //
    ParameterlessLookup[0x36] = "SS";
    ParameterlessLookup[0x37] = "AAA";
    //
    ParameterlessLookup[0x3E] = "DS";
    ParameterlessLookup[0x3F] = "AAS";
    ParameterlessLookup[0x40] = "INC AX";
    ParameterlessLookup[0x41] = "INC CX";
    ParameterlessLookup[0x42] = "INC DX";
    ParameterlessLookup[0x43] = "INC BX";
    ParameterlessLookup[0x44] = "INC SP";
    ParameterlessLookup[0x45] = "INC BP";
    ParameterlessLookup[0x46] = "INC SI";
    ParameterlessLookup[0x47] = "INC DI";
    ParameterlessLookup[0x48] = "DEC AX";
    ParameterlessLookup[0x49] = "DEC CX";
    ParameterlessLookup[0x4A] = "DEC DX";
    ParameterlessLookup[0x4B] = "DEC BX";
    ParameterlessLookup[0x4C] = "DEC SP";
    ParameterlessLookup[0x4D] = "DEC BP";
    ParameterlessLookup[0x4E] = "DEC SI";
    ParameterlessLookup[0x4F] = "DEC DI";
    ParameterlessLookup[0x50] = "PUSH AX";
    ParameterlessLookup[0x51] = "PUSH CX";
    ParameterlessLookup[0x52] = "PUSH DX";
    ParameterlessLookup[0x53] = "PUSH BX";
    ParameterlessLookup[0x54] = "PUSH SP";
    ParameterlessLookup[0x55] = "PUSH BP";
    ParameterlessLookup[0x56] = "PUSH SI";
    ParameterlessLookup[0x57] = "PUSH DI";
    ParameterlessLookup[0x58] = "POP AX";
    ParameterlessLookup[0x59] = "POP CX";
    ParameterlessLookup[0x5A] = "POP DX";
    ParameterlessLookup[0x5B] = "POP BX";
    ParameterlessLookup[0x5C] = "POP SP";
    ParameterlessLookup[0x5D] = "POP BP";
    ParameterlessLookup[0x5E] = "POP SI";
    ParameterlessLookup[0x5F] = "POP DI";
    //
    JumpLookup[0x70] = "JO";
    JumpLookup[0x71] = "JNO";
    JumpLookup[0x72] = "JB";
    JumpLookup[0x73] = "JNB";
    JumpLookup[0x74] = "JE";
    JumpLookup[0x75] = "JNZ";
    JumpLookup[0x76] = "JBE";
    JumpLookup[0x77] = "JA";
    JumpLookup[0x78] = "JS";
    JumpLookup[0x79] = "JNS";
    JumpLookup[0x7A] = "JP";
    JumpLookup[0x7B] = "JNP";
    JumpLookup[0x7C] = "JL";
    JumpLookup[0x7D] = "JNL";
    JumpLookup[0x7E] = "JLE";
    JumpLookup[0x7F] = "JG";
    //
    ParameterlessLookup[0x90] = "NOP";
    ParameterlessLookup[0x91] = "XCHG AX, CX";
    ParameterlessLookup[0x92] = "XCHG AX, DX";
    ParameterlessLookup[0x93] = "XCHG AX, BX";
    ParameterlessLookup[0x94] = "XCHG AX, SP";
    ParameterlessLookup[0x95] = "XCHG AX, BP";
    ParameterlessLookup[0x96] = "XCHG AX, SI";
    ParameterlessLookup[0x97] = "XCHG AX, DI";
    ParameterlessLookup[0x98] = "CBW";
    ParameterlessLookup[0x99] = "CWD";
    //
    ParameterlessLookup[0x9B] = "WAIT";
    ParameterlessLookup[0x9C] = "PUSHF";
    ParameterlessLookup[0x9D] = "POPF";
    ParameterlessLookup[0x9E] = "SAHF";
    ParameterlessLookup[0x9F] = "LAHF";
    //
    ParameterlessLookup[0xC3] = "RET";
    //
    ParameterlessLookup[0xCB] = "RET";
    //
    ParameterlessLookup[0xCC] = "INT 3";
    //
    ParameterlessLookup[0xCE] = "INTO";
    ParameterlessLookup[0xCF] = "IRET";
    //
    JumpLookup[0xE0] = "LOOPNZ";
    JumpLookup[0xE1] = "LOOPZ";
    JumpLookup[0xE2] = "LOOP";
    JumpLookup[0xE3] = "JCXZ";
    //
    ParameterlessLookup[0xEC] = "IN AL, DX";
    ParameterlessLookup[0xED] = "IN AX, DX";
    ParameterlessLookup[0xEE] = "OUT AL, DX";
    ParameterlessLookup[0xEF] = "OUT AX, DX";
    ParameterlessLookup[0xF0] = "LOCK";
    //
    ParameterlessLookup[0xF2] = "REPNE";
    ParameterlessLookup[0xF3] = "REP";
    ParameterlessLookup[0xF4] = "HLT";
    ParameterlessLookup[0xF5] = "CMC";
    //
    ParameterlessLookup[0xF8] = "CLC";
    ParameterlessLookup[0xF9] = "STC";
    ParameterlessLookup[0xFA] = "CLI";
    ParameterlessLookup[0xFB] = "STI";
    ParameterlessLookup[0xFC] = "CLD";
    ParameterlessLookup[0xFD] = "STD";

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
                printf("MOV %s, %d\n", RegisterLookup[RegIndex], Data);
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
                        printf("MOV %s, %s\n", Operands[0], Operands[1]);
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
                        printf("MOV %s, %s\n", Operands[0], Operands[1]);
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
                        printf("MOV %s, %s\n", Operands[0], Operands[1]);
                    } break;

                    case MODE_REGISTER:
                    {
                        unsigned char R_MIndex = R_M | (DataSize << 3);
                        Operands[!RegMode] = RegisterLookup[RegIndex];
                        Operands[RegMode] = RegisterLookup[R_MIndex];
                        printf("MOV %s, %s\n", Operands[0], Operands[1]);
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
                printf("MOV %s, [%u]\n", RegisterLookup[DataSize << 3], Address);
            }
            // Memory from accumulator
            // Checking if Byte0 matches 1010001w
            else if (0xa2 == (Byte0 & 0xfe))
            {
                unsigned char DataSize = Byte0 & 1;
                unsigned char AddressLo = InstructionStream[InstructionIndex++];
                unsigned char AddressHi = InstructionStream[InstructionIndex++];
                int Address = (AddressHi << 8) | AddressLo;
                printf("MOV [%u], %s\n", Address, RegisterLookup[DataSize << 3]);
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
                        printf("MOV [%s], %s\n", MemoryAddress, DataBuffer);
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
                        printf("MOV [%s], %s\n", MemoryAddress, DataBuffer);
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
                        printf("MOV [%s], %s\n", MemoryAddress, DataBuffer);
                    } break;

                    case MODE_REGISTER:
                    {
                        unsigned char R_MIndex = R_M | (DataSize << 3);
                        int Data = InstructionStream[InstructionIndex++];
                        if (DataSize == DATA_SIZE_WORD)
                        {
                            Data = (InstructionStream[InstructionIndex++] << 8) | Data;
                        }
                        printf("MOV %s, %d\n", RegisterLookup[R_MIndex], Data);
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
#if 0
            else if (0xFF == Byte0)
            {
                unsigned char Byte1 = InstructionStream[InstructionIndex++];
                unsigned char Mode = (Byte1 >> 6) & 3;
                unsigned char ShortOpCode = (Byte1 >> 3) & 7;
                unsigned char R_M = Byte1 & 7;
                // 000: INC  MEM16
                // 001: DEC  MEM16
                // 010: CALL REG16/MEM16 (intra)
                // 011: CALL MEM16 (intersegment)
                // 100: JMP  REG16/MEM16 (intra)
                // 101: JMP  MEM16 (intersegment)
                // 110: PUSH MEM16
                // 111: (not used)
            }
#endif
            else if (JumpLookup[Byte0])
            {
                printf("%s ($+2)%+hhd\n", JumpLookup[Byte0], InstructionStream[InstructionIndex++]);
            }
            else if (ParameterlessLookup[Byte0])
            {
                printf("%s\n", ParameterlessLookup[Byte0]);
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
