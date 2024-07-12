#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#define REG_MODE_SOURCE 0
#define REG_MODE_DESTINATION 1

#define DATA_SIZE_BYTE 0
#define DATA_SIZE_WORD 1

#define MODE_MEMORY_DISPLACEMENT_NONE 0
#define MODE_MEMORY_DISPLACEMENT_8_BIT 1
#define MODE_MEMORY_DISPLACEMENT_16_BIT 2
#define MODE_REGISTER 3

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define s8 int8_t
#define s16 int16_t
#define s32 int32_t

u32 FlagZero = 0;
u32 FlagSign = 0;
u32 FlagCarry = 0;
u32 FlagAuxiliaryCarry = 0;
u32 FlagParity = 0;
u32 FlagOverflow = 0;
u32 FlagInterruptEnable = 0;
u32 FlagDirection = 0;
u32 FlagTrap = 0;

enum register_name
{
    register_name_AX,
    register_name_CX,
    register_name_DX,
    register_name_BX,
    register_name_SP,
    register_name_BP,
    register_name_SI,
    register_name_DI,
};

enum arithmetic_operator
{
    arithmetic_operator_ADD,
    arithmetic_operator_OR,
    arithmetic_operator_ADC,
    arithmetic_operator_SBB,
    arithmetic_operator_AND,
    arithmetic_operator_SUB,
    arithmetic_operator_XOR,
    arithmetic_operator_CMP,
};

enum jump_operator
{
    jump_operator_JO = 0x70,
    jump_operator_JNO,
    jump_operator_JB,
    jump_operator_JNB,
    jump_operator_JZ,
    jump_operator_JNZ,
    jump_operator_JBE,
    jump_operator_JA,
    jump_operator_JS,
    jump_operator_JNS,
    jump_operator_JP,
    jump_operator_JNP,
    jump_operator_JL,
    jump_operator_JNL,
    jump_operator_JLE,
    jump_operator_JG,
    jump_operator_LOOPNZ = 0xE0,
    jump_operator_LOOPZ,
    jump_operator_LOOP,
    jump_operator_JCXZ,
};

char *SegmentRegisterLookup[] =
{
    "ES",
    "CS",
    "SS",
    "DS",
};
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
enum memory_mode
{
    memory_mode_BX_SI,
    memory_mode_BX_DI,
    memory_mode_BP_SI,
    memory_mode_BP_DI,
    memory_mode_SI,
    memory_mode_DI,
    memory_mode_BP,
    memory_mode_BX,
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
    JumpLookup[0x74] = "JZ";
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

    u16 Registers[8] = {0};
    u8 Memory[1024*1024];
    u16 SegmentRegisters[4] = {0};
    u16 PreviousInstructionPointer, InstructionPointer = 0;
    if (2 == ArgCount)
    {
        char *FileName = Args[1];
        printf("; Disassembly for %s\n", FileName);
        printf("bits 16\n");
        FILE *MachineCodeFile = fopen(FileName, "rb");
        u8 InstructionStream[2048];
        u8 Byte0;
        size_t BytesRead = fread(InstructionStream, 1, ArrayCount(InstructionStream), MachineCodeFile);
        while (InstructionPointer < BytesRead)
        {
            PreviousInstructionPointer = InstructionPointer;
            u16 DestValue = 0;
            u16 SourceValue = 0;
            u32 Result = 0;
            u8 SetFlags = 0;
            u32 IsSub = 0;
            Byte0 = InstructionStream[InstructionPointer++];
            // Immediate to register
            // Checking if Byte0 matches 1011wreg
            if (0xB0 == (Byte0 & 0xF0))
            {
                u8 DataSize = (Byte0 >> 3) & 1;
                u8 Reg = Byte0 & 0x7;
                u8 RegIndex = Byte0 & 0xF;
                u8 DataLo = InstructionStream[InstructionPointer++];
                u16 Data = (s8)DataLo;
                if (DataSize == DATA_SIZE_WORD)
                {
                    Data = (InstructionStream[InstructionPointer++] << 8) | DataLo;
                    Registers[Reg] = Data;
                }
                else
                {
                    u8 DestHi = RegIndex >> 2;
                    RegIndex &= 3;
                    u8 *Dest = ((u8 *)(Registers + RegIndex)) + DestHi;
                    *Dest = DataLo;
                }
                printf("MOV %s, %hd", RegisterLookup[RegIndex], Data);
            }
            // Register/memory to/from register
            // Checking if Byte0 matches 100010dw
            else if (0x88 == (Byte0 & 0xFC))
            {
                u8 Byte1 = InstructionStream[InstructionPointer++];
                u8 RegIsDest = (Byte0 >> 1) & 1;
                u8 DataSize = Byte0 & 1;
                u8 Mod = (Byte1 >> 6);
                u8 Reg = (Byte1 >> 3) & 7;
                u8 R_M = Byte1 & 7;
                u8 RegIndex = Reg | (DataSize << 3);
                char MemoryAddressBuffer[256];
                char MemoryAddressOperandBuffer[256];
                char *Operands[2];
                switch (Mod)
                {
                    case MODE_MEMORY_DISPLACEMENT_NONE:
                    {
                        u32 MemoryOffset = 0;
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        switch (R_M) {
                            case memory_mode_BX_SI:
                            {
                                MemoryOffset += Registers[register_name_BX] + Registers[register_name_SI];
                            } break;
                            case memory_mode_BX_DI:
                            {
                                MemoryOffset += Registers[register_name_BX] + Registers[register_name_DI];
                            } break;
                            case memory_mode_BP_SI:
                            {
                                MemoryOffset += Registers[register_name_BP] + Registers[register_name_SI];
                            } break;
                            case memory_mode_BP_DI:
                            {
                                MemoryOffset += Registers[register_name_BP] + Registers[register_name_DI];
                            } break;
                            case memory_mode_SI:
                            {
                                MemoryOffset += Registers[register_name_SI];
                            } break;
                            case memory_mode_DI:
                            {
                                MemoryOffset += Registers[register_name_DI];
                            } break;
                            case memory_mode_BX:
                            {
                                MemoryOffset += Registers[register_name_BX];
                            } break;
                            case memory_mode_BP:
                            {
                                u8 LowByte = InstructionStream[InstructionPointer++];
                                u8 HighByte = InstructionStream[InstructionPointer++];
                                u16 Disp = (HighByte << 8) | LowByte;
                                MemoryOffset += Disp;
                                if (Disp)
                                {
                                    snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%u", Disp);
                                    MemoryAddress = MemoryAddressBuffer;
                                }
                            } break;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        Operands[!RegIsDest] = RegisterLookup[RegIndex];
                        Operands[RegIsDest] = MemoryAddressOperandBuffer;
                        printf("MOV %s, %s", Operands[0], Operands[1]);
                        u8 RegOffset = ((Reg >> 2) & 1) & !DataSize;
                        u8 *Dest = RegIsDest ? (u8*)(Registers + Reg) + RegOffset : Memory + MemoryOffset;
                        u8 *Source = !RegIsDest ? (u8*)(Registers + Reg) + RegOffset: Memory + MemoryOffset;
                        if (DataSize)
                        {
                            *(u16*)Dest = *(u16*)Source;
                        }
                        else
                        {
                            *Dest = *Source;
                        }
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_8_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        u8 Disp = InstructionStream[InstructionPointer++];
                        u32 MemoryOffset = Disp;
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hhd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        switch (R_M) {
                            case memory_mode_BX_SI:
                            {
                                MemoryOffset += Registers[register_name_BX] + Registers[register_name_SI];
                            } break;
                            case memory_mode_BX_DI:
                            {
                                MemoryOffset += Registers[register_name_BX] + Registers[register_name_DI];
                            } break;
                            case memory_mode_BP_SI:
                            {
                                MemoryOffset += Registers[register_name_BP] + Registers[register_name_SI];
                            } break;
                            case memory_mode_BP_DI:
                            {
                                MemoryOffset += Registers[register_name_BP] + Registers[register_name_DI];
                            } break;
                            case memory_mode_SI:
                            {
                                MemoryOffset += Registers[register_name_SI];
                            } break;
                            case memory_mode_DI:
                            {
                                MemoryOffset += Registers[register_name_DI];
                            } break;
                            case memory_mode_BP:
                            {
                                MemoryOffset += Registers[register_name_BP];
                            } break;
                            case memory_mode_BX:
                            {
                                MemoryOffset += Registers[register_name_BX];
                            } break;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        Operands[!RegIsDest] = RegisterLookup[RegIndex];
                        Operands[RegIsDest] = MemoryAddressOperandBuffer;
                        printf("MOV %s, %s", Operands[0], Operands[1]);
                        u8 RegOffset = ((Reg >> 2) & 1) & !DataSize;
                        u8 *Dest = RegIsDest ? (u8*)(Registers + Reg) + RegOffset : Memory + MemoryOffset;
                        u8 *Source = !RegIsDest ? (u8*)(Registers + Reg) + RegOffset: Memory + MemoryOffset;
                        if (DataSize)
                        {
                            *(u16*)Dest = *(u16*)Source;
                        }
                        else
                        {
                            *Dest = *Source;
                        }
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_16_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        u8 LowByte = InstructionStream[InstructionPointer++];
                        u8 HighByte = InstructionStream[InstructionPointer++];
                        s16 Disp = HighByte << 8 | LowByte;
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        Operands[!RegIsDest] = RegisterLookup[RegIndex];
                        Operands[RegIsDest] = MemoryAddressOperandBuffer;
                        printf("MOV %s, %s", Operands[0], Operands[1]);
                    } break;

                    case MODE_REGISTER:
                    {
                        u8 R_MIndex = R_M | (DataSize << 3);
                        Operands[!RegIsDest] = RegisterLookup[RegIndex];
                        Operands[RegIsDest] = RegisterLookup[R_MIndex];
                        printf("MOV %s, %s", Operands[0], Operands[1]);
                        u8 SourceRegIndex = RegIsDest ? R_M : Reg;
                        u8 DestRegIndex = RegIsDest ? Reg : R_M;
                        if (DataSize)
                        {
                            Registers[DestRegIndex] = Registers[SourceRegIndex];
                        }
                        else
                        {
                            u8 SourceHi = SourceRegIndex >> 2;
                            u8 DestHi = DestRegIndex >> 2;
                            SourceRegIndex &= 3;
                            DestRegIndex &= 3;
                            u8 *Source = ((u8 *)(Registers + SourceRegIndex)) + SourceHi;
                            u8 *Dest = ((u8 *)(Registers + DestRegIndex)) + DestHi;
                            *Dest = *Source;
                        }
                    } break;

                    default:
                    {
                        printf("Encountered unsupported mode 0x%02x\n", Mod);
                        exit(1);
                    } break;
                }
            }
            // Memory to accumulator
            // Checking if Byte0 matches 1010000w
            else if (0xa0 == (Byte0 & 0xfe))
            {
                u8 DataSize = Byte0 & 1;
                u8 AddressLo = InstructionStream[InstructionPointer++];
                u8 AddressHi = InstructionStream[InstructionPointer++];
                int Address = (AddressHi << 8) | AddressLo;
                printf("MOV %s, [%u]", RegisterLookup[DataSize << 3], Address);
            }
            // Memory from accumulator
            // Checking if Byte0 matches 1010001w
            else if (0xa2 == (Byte0 & 0xfe))
            {
                u8 DataSize = Byte0 & 1;
                u8 AddressLo = InstructionStream[InstructionPointer++];
                u8 AddressHi = InstructionStream[InstructionPointer++];
                int Address = (AddressHi << 8) | AddressLo;
                printf("MOV [%u], %s", Address, RegisterLookup[DataSize << 3]);
            }
            // Immediate to register/memory
            // Checking if Byte0 matches 11000110w
            else if (0xc6 == (Byte0 & 0xFE))
            {
                u8 Byte1 = InstructionStream[InstructionPointer++];
                u8 DataSize = Byte0 & 1;
                u8 Mod = (Byte1 >> 6);
                u8 R_M = Byte1 & 7;
                char MemoryAddressBuffer[256];
                char DataBuffer[256];
                switch (Mod)
                {
                    case MODE_MEMORY_DISPLACEMENT_NONE:
                    {
                        u32 MemoryOffset = 0;
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        if (6 == R_M) {
                            u8 LowByte = InstructionStream[InstructionPointer++];
                            u8 HighByte = InstructionStream[InstructionPointer++];
                            u16 Disp = (HighByte << 8) | LowByte;
                            MemoryOffset += Disp;
                            if (Disp)
                            {
                                snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%u", Disp);
                                MemoryAddress = MemoryAddressBuffer;
                            }
                        }
                        else
                        {
                            MemoryOffset += Registers[R_M];
                        }
                        u8 DataLo = InstructionStream[InstructionPointer++];
                        Memory[MemoryOffset + 0] = DataLo;
                        if (DataSize == DATA_SIZE_WORD)
                        {
                            u8 DataHi = InstructionStream[InstructionPointer++];
                            Memory[MemoryOffset + 1] = DataHi;
                            u16 Data = (DataHi << 8) | DataLo;
                            snprintf(DataBuffer, sizeof(DataBuffer), "word %hd", Data);
                        }
                        else
                        {
                            snprintf(DataBuffer, sizeof(DataBuffer), "byte %hhd", DataLo);
                        }
                        printf("MOV [%s], %s", MemoryAddress, DataBuffer);
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_8_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        u8 Disp = InstructionStream[InstructionPointer++];
                        u32 MemoryOffset = Disp;
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hhd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        switch (R_M) {
                            case memory_mode_BX_SI:
                            {
                                MemoryOffset += Registers[register_name_BX] + Registers[register_name_SI];
                            } break;
                            case memory_mode_BX_DI:
                            {
                                MemoryOffset += Registers[register_name_BX] + Registers[register_name_DI];
                            } break;
                            case memory_mode_BP_SI:
                            {
                                MemoryOffset += Registers[register_name_BP] + Registers[register_name_SI];
                            } break;
                            case memory_mode_BP_DI:
                            {
                                MemoryOffset += Registers[register_name_BP] + Registers[register_name_DI];
                            } break;
                            case memory_mode_SI:
                            {
                                MemoryOffset += Registers[register_name_SI];
                            } break;
                            case memory_mode_DI:
                            {
                                MemoryOffset += Registers[register_name_DI];
                            } break;
                            case memory_mode_BP:
                            {
                                MemoryOffset += Registers[register_name_BP];
                            } break;
                            case memory_mode_BX:
                            {
                                MemoryOffset += Registers[register_name_BX];
                            } break;
                        }
                        u8 DataLo = InstructionStream[InstructionPointer++];
                        Memory[MemoryOffset + 0] = DataLo;
                        if (DataSize == DATA_SIZE_WORD)
                        {
                            u8 DataHi = InstructionStream[InstructionPointer++];
                            Memory[MemoryOffset + 1] = DataHi;
                            u16 Data = (DataHi << 8) | DataLo;
                            snprintf(DataBuffer, sizeof(DataBuffer), "word %hd", Data);
                        }
                        else
                        {
                            snprintf(DataBuffer, sizeof(DataBuffer), "byte %hhd", DataLo);
                        }
                        printf("MOV [%s], %s", MemoryAddress, DataBuffer);
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_16_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        u8 LowByte = InstructionStream[InstructionPointer++];
                        u8 HighByte = InstructionStream[InstructionPointer++];
                        int Disp = HighByte << 8 | LowByte;
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        u8 DataLo = InstructionStream[InstructionPointer++];
                        if (DataSize == DATA_SIZE_WORD)
                        {
                            u16 Data = (InstructionStream[InstructionPointer++] << 8) | DataLo;
                            snprintf(DataBuffer, sizeof(DataBuffer), "word %hd", Data);
                        }
                        else
                        {
                            snprintf(DataBuffer, sizeof(DataBuffer), "byte %hhd", DataLo);
                        }
                        printf("MOV [%s], %s", MemoryAddress, DataBuffer);
                    } break;

                    case MODE_REGISTER:
                    {
                        u8 R_MIndex = R_M | (DataSize << 3);
                        u8 DataLo = InstructionStream[InstructionPointer++];
                        u16 Data = (s8)DataLo;
                        if (DataSize == DATA_SIZE_WORD)
                        {
                            Data = (InstructionStream[InstructionPointer++] << 8) | DataLo;
                        }
                        printf("MOV %s, %hd", RegisterLookup[R_MIndex], Data);
                    } break;

                    default:
                    {
                        printf("Encountered unsupported mode 0x%02x\n", Mod);
                        exit(1);
                    } break;
                }
            }
            // Register/memory to segment register
            // Segment register to register/memory
            // Checking if Byte0 matches 100011x0
            else if (0x8C == (Byte0 & 0xFD))
            {
                u8 ToSegmentRegister = (Byte0 >> 1) & 1;
                u8 Byte1 = InstructionStream[InstructionPointer++];
                u8 Mod = (Byte1 >> 6) & 0x3;
                u8 SR = (Byte1 >> 3) & 0x3;
                u8 R_M = (Byte1 >> 0) & 0x7;
                switch (Mod)
                {
                    case MODE_REGISTER:
                    {
                        u8 R_MIndex = 8|R_M;
                        if (ToSegmentRegister)
                        {
                            printf("MOV %s, %s", SegmentRegisterLookup[SR], RegisterLookup[R_MIndex]);
                            SegmentRegisters[SR] = Registers[R_M];
                        }
                        else
                        {
                            printf("MOV %s, %s", RegisterLookup[R_MIndex], SegmentRegisterLookup[SR]);
                            Registers[R_M] = SegmentRegisters[SR];
                        }
                    } break;

                    default:
                    {
                        char MemoryAddressBuffer[256];
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        switch (Mod)
                        {
                            case MODE_MEMORY_DISPLACEMENT_NONE:
                            {
                                if (6 == R_M) {
                                    u8 LowByte = InstructionStream[InstructionPointer++];
                                    u8 HighByte = InstructionStream[InstructionPointer++];
                                    int Disp = (HighByte << 8) | LowByte;
                                    if (Disp)
                                    {
                                        snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%u", Disp);
                                        MemoryAddress = MemoryAddressBuffer;
                                    }
                                }
                            } break;

                            case MODE_MEMORY_DISPLACEMENT_8_BIT:
                            {
                                u8 Disp = InstructionStream[InstructionPointer++];
                                if (Disp)
                                {
                                    snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hhd", MemoryAddress, Disp);
                                    MemoryAddress = MemoryAddressBuffer;
                                }
                            } break;

                            case MODE_MEMORY_DISPLACEMENT_16_BIT:
                            {
                                u8 LowByte = InstructionStream[InstructionPointer++];
                                u8 HighByte = InstructionStream[InstructionPointer++];
                                int Disp = HighByte << 8 | LowByte;
                                if (Disp)
                                {
                                    snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hd", MemoryAddress, Disp);
                                    MemoryAddress = MemoryAddressBuffer;
                                }
                            } break;

                            default:
                            {
                                printf("Encountered unsupported mode 0x%02x\n", Mod);
                                exit(1);
                            } break;
                        }
                        if (ToSegmentRegister)
                        {
                            printf("MOV %s, [%s]", SegmentRegisterLookup[SR], MemoryAddress);
                        }
                        else
                        {
                            printf("MOV [%s], %s", MemoryAddress, SegmentRegisterLookup[SR]);
                        }
                    } break;
                }
            }
            // Reg/memory with register to either
            // Matching pattern 00xxx0dw
            else if (0 == (Byte0 & 0xC4))
            {
                u8 RegIsDest = (Byte0 >> 1) & 1;
                u8 DataSize = Byte0 & 1;
                u8 Byte1 = InstructionStream[InstructionPointer++];
                u8 Mod = (Byte1 >> 6) & 0x3;
                u8 Reg = (Byte1 >> 3) & 0x7;
                u8 R_M = (Byte1 >> 0) & 0x7;
                u8 OperatorIndex = (Byte0 >> 3) & 7;
                u8 RegIndex = Reg | (DataSize << 3);
                char *Operator = OperatorLookup[OperatorIndex];
                char MemoryAddressBuffer[256];
                char MemoryAddressOperandBuffer[256];
                char *Operands[2];
                switch (Mod)
                {
                    case MODE_MEMORY_DISPLACEMENT_NONE:
                    {
                        u32 MemoryOffset = 0;
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        switch (R_M) {
                            case memory_mode_BX_SI:
                            {
                                MemoryOffset += Registers[register_name_BX] + Registers[register_name_SI];
                            } break;
                            case memory_mode_BX_DI:
                            {
                                MemoryOffset += Registers[register_name_BX] + Registers[register_name_DI];
                            } break;
                            case memory_mode_BP_SI:
                            {
                                MemoryOffset += Registers[register_name_BP] + Registers[register_name_SI];
                            } break;
                            case memory_mode_BP_DI:
                            {
                                MemoryOffset += Registers[register_name_BP] + Registers[register_name_DI];
                            } break;
                            case memory_mode_SI:
                            {
                                MemoryOffset += Registers[register_name_SI];
                            } break;
                            case memory_mode_DI:
                            {
                                MemoryOffset += Registers[register_name_DI];
                            } break;
                            case memory_mode_BP:
                            {
                                u8 LowByte = InstructionStream[InstructionPointer++];
                                u8 HighByte = InstructionStream[InstructionPointer++];
                                u16 Disp = (HighByte << 8) | LowByte;
                                MemoryOffset += Disp;
                                if (Disp)
                                {
                                    snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%u", Disp);
                                    MemoryAddress = MemoryAddressBuffer;
                                }
                            } break;
                            case memory_mode_BX:
                            {
                                MemoryOffset += Registers[register_name_BX];
                            } break;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        Operands[!RegIsDest] = RegisterLookup[RegIndex];
                        Operands[RegIsDest] = MemoryAddressOperandBuffer;
                        u8 StoreResult = 0;
                        u8 RegOffset = ((Reg >> 2) & 1) & !DataSize;
                        u8 *Dest = RegIsDest ? (u8*)(Registers + Reg) + RegOffset : Memory + MemoryOffset;
                        u8 *Source = !RegIsDest ? (u8*)(Registers + Reg) + RegOffset : Memory + MemoryOffset;
                        if (DataSize)
                        {
                            DestValue = *(u16*)Dest;
                            SourceValue = *(u16*)Source;
                        }
                        else
                        {
                            DestValue = *Dest;
                            SourceValue = *Source;
                        }
                        switch (OperatorIndex)
                        {
                            case arithmetic_operator_ADD:
                            {
                                StoreResult = 1;
                                Result = DestValue + SourceValue;
                            } break;

                            case arithmetic_operator_SUB:
                            {
                                StoreResult = 1;
                            } // Fallthrough
                            case arithmetic_operator_CMP:
                            {
                                IsSub = 1;
                                SourceValue = -SourceValue;
                                Result = DestValue + SourceValue;
                            } break;
                        }
                        if (StoreResult)
                        {
                            if (DataSize)
                            {
                                *(u16*)Dest = (u16)Result;
                            }
                            else
                            {
                                *Dest = (u8)Result;
                            }
                        }
                        SetFlags = 1;
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_8_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        u8 Disp = InstructionStream[InstructionPointer++];
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hhd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        Operands[!RegIsDest] = RegisterLookup[RegIndex];
                        Operands[RegIsDest] = MemoryAddressOperandBuffer;
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_16_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        u8 LowByte = InstructionStream[InstructionPointer++];
                        u8 HighByte = InstructionStream[InstructionPointer++];
                        s16 Disp = HighByte << 8 | LowByte;
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        Operands[!RegIsDest] = RegisterLookup[RegIndex];
                        Operands[RegIsDest] = MemoryAddressOperandBuffer;
                    } break;

                    case MODE_REGISTER:
                    {
                        u8 R_MIndex = R_M | (DataSize << 3);
                        Operands[!RegIsDest] = RegisterLookup[RegIndex];
                        Operands[RegIsDest] = RegisterLookup[R_MIndex];
                        u8 StoreResult = 0;
                        switch (OperatorIndex)
                        {
                            case arithmetic_operator_ADD:
                            {
                                StoreResult = 1;
                                if (DataSize)
                                {
                                    DestValue = Registers[Reg];
                                    SourceValue = Registers[R_M];
                                    Result = DestValue + SourceValue;
                                }
                            } break;

                            case arithmetic_operator_SUB:
                            {
                                StoreResult = 1;
                            } // Fallthrough
                            case arithmetic_operator_CMP:
                            {
                                IsSub = 1;
                                if (DataSize)
                                {
                                    DestValue = RegIsDest ? Registers[Reg] : Registers[R_M];
                                    SourceValue = -(!RegIsDest ? Registers[Reg] : Registers[R_M]);
                                    Result = DestValue + SourceValue;
                                }
                            } break;
                        }
                        if (StoreResult)
                        {
                            if (DataSize)
                            {
                                Registers[RegIsDest ? Reg : R_M] = (u16)Result;
                            }
                        }
                        SetFlags = 1;
                    } break;

                    default:
                    {
                        printf("Encountered unsupported mode 0x%02x\n", Mod);
                        exit(1);
                    } break;
                }
                printf("%s %s, %s", Operator, Operands[0], Operands[1]);
            }
            // Immediate to register/memory
            // Matching pattern 100000sw
            else if (0x80 == (Byte0 & 0xFC))
            {
                u8 SignExtension = (Byte0 >> 1) & 1;
                u8 DataSize = Byte0 & 1;
                unsigned IsWord = (Byte0 & 3) == 1;
                u8 Byte1 = InstructionStream[InstructionPointer++];
                u8 Mod = (Byte1 >> 6) & 0x3;
                u8 OperatorIndex = (Byte1 >> 3) & 7;
                u8 R_M = (Byte1 >> 0) & 0x7;
                char *Operands[2];
                char MemoryAddressBuffer[256];
                char MemoryAddressOperandBuffer[256];
                char DataBuffer[256];
                switch (Mod)
                {
                    case MODE_MEMORY_DISPLACEMENT_NONE:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        if (6 == R_M) {
                            u8 LowByte = InstructionStream[InstructionPointer++];
                            u8 HighByte = InstructionStream[InstructionPointer++];
                            int Disp = (HighByte << 8) | LowByte;
                            if (Disp)
                            {
                                snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%u", Disp);
                                MemoryAddress = MemoryAddressBuffer;
                            }
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        u8 DataLo = InstructionStream[InstructionPointer++];
                        u16 Data = (s8)DataLo;
                        if (IsWord)
                        {
                            Data = (InstructionStream[InstructionPointer++] << 8) | DataLo;
                        }
                        snprintf(DataBuffer, sizeof(DataBuffer), "%s %hd", DataSize ? "word" : "byte", Data);
                        Operands[0] = MemoryAddressOperandBuffer;
                        Operands[1] = DataBuffer;
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_8_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        u8 Disp = InstructionStream[InstructionPointer++];
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hhd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        u8 DataLo = InstructionStream[InstructionPointer++];
                        u16 Data = (s8)DataLo;
                        if (IsWord)
                        {
                            Data = (InstructionStream[InstructionPointer++] << 8) | DataLo;
                        }
                        snprintf(DataBuffer, sizeof(DataBuffer), "%s %hd", DataSize ? "word" : "byte", Data);
                        Operands[0] = MemoryAddressOperandBuffer;
                        Operands[1] = DataBuffer;
                    } break;

                    case MODE_MEMORY_DISPLACEMENT_16_BIT:
                    {
                        char *MemoryAddress = MemoryModeLookup[R_M];
                        u8 LowByte = InstructionStream[InstructionPointer++];
                        u8 HighByte = InstructionStream[InstructionPointer++];
                        int Disp = HighByte << 8 | LowByte;
                        if (Disp)
                        {
                            snprintf(MemoryAddressBuffer, sizeof(MemoryAddressBuffer), "%s %+hd", MemoryAddress, Disp);
                            MemoryAddress = MemoryAddressBuffer;
                        }
                        snprintf(MemoryAddressOperandBuffer, sizeof(MemoryAddressOperandBuffer), "[%s]", MemoryAddress);
                        u8 DataLo = InstructionStream[InstructionPointer++];
                        u16 Data = (s8)DataLo;
                        if (IsWord)
                        {
                            Data = (InstructionStream[InstructionPointer++] << 8) | DataLo;
                        }
                        snprintf(DataBuffer, sizeof(DataBuffer), "%s %hd", DataSize ? "word" : "byte", Data);
                        Operands[0] = MemoryAddressOperandBuffer;
                        Operands[1] = DataBuffer;
                    } break;

                    case MODE_REGISTER:
                    {
                        u8 R_MIndex = R_M | (DataSize << 3);
                        u8 DataLo = InstructionStream[InstructionPointer++];
                        u16 Data = (s8)DataLo;
                        if (IsWord)
                        {
                            Data = (InstructionStream[InstructionPointer++] << 8) | DataLo;
                        }
                        snprintf(DataBuffer, sizeof(DataBuffer), "%hd", Data);
                        Operands[0] = RegisterLookup[R_MIndex];
                        Operands[1] = DataBuffer;
                        u8 StoreResult = 0;
                        DestValue = Registers[R_M];
                        SourceValue = Data;
                        switch (OperatorIndex)
                        {
                            case arithmetic_operator_ADD:
                            {
                                StoreResult = 1;
                                if (DataSize)
                                {
                                    Result = DestValue + SourceValue;
                                }
                            } break;

                            case arithmetic_operator_SUB:
                            {
                                StoreResult = 1;
                            } // Fallthrough
                            case arithmetic_operator_CMP:
                            {
                                IsSub = 1;
                                if (DataSize)
                                {
                                    SourceValue = -SourceValue;
                                    Result = DestValue + SourceValue;
                                }
                            } break;
                        }
                        if (StoreResult)
                        {
                            if (DataSize)
                            {
                                Registers[R_M] = (u16)Result;
                            }
                        }
                        SetFlags = 1;
                    } break;

                    default:
                    {
                        printf("Encountered unsupported mode 0x%02x\n", Mod);
                        exit(1);
                    } break;
                }

                char *Operator = OperatorLookup[OperatorIndex];
                printf("%s %s, %s", Operator, Operands[0], Operands[1]);
            }
            // Immediate to accumulator
            // Matching pattern 00xxx10w
            else if (0x04 == (Byte0 & 0xC6))
            {
                u8 DataSize = Byte0 & 1;
                u8 DataLo = InstructionStream[InstructionPointer++];
                u16 Data = (s8)DataLo;
                if (DataSize)
                {
                    Data = (InstructionStream[InstructionPointer++] << 8) | DataLo;
                }
                u8 OperatorIndex = (Byte0 >> 3) & 7;
                char *Operator = OperatorLookup[OperatorIndex];
                char *RegisterName = RegisterLookup[DataSize << 3];
                printf("%s %s, %hd", Operator, RegisterName, Data);
            }
#if 0
            else if (0xFF == Byte0)
            {
                u8 Byte1 = InstructionStream[InstructionPointer++];
                u8 Mod = (Byte1 >> 6) & 3;
                u8 ShortOpCode = (Byte1 >> 3) & 7;
                u8 R_M = Byte1 & 7;
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
                s8 IPInc8 = InstructionStream[InstructionPointer++];
                u32 Jump = 0;
                switch (Byte0)
                {
                    case jump_operator_JNZ:
                    {
                        Jump = !FlagZero;
                    } break;

                    case jump_operator_JZ:
                    {
                        Jump = FlagZero;
                    } break;

                    case jump_operator_JP:
                    {
                        Jump = FlagParity;
                    } break;

                    case jump_operator_JB:
                    {
                        Jump = FlagCarry;
                    } break;

                    case jump_operator_LOOPNZ:
                    {
                        u16 CX = Registers[register_name_CX] - 1;
                        Registers[register_name_CX] = CX;
                        Jump = CX && !FlagZero;
                    } break;
                }
                if (Jump)
                {
                    InstructionPointer += IPInc8;
                }
                printf("%s ($+2)%+hhd", JumpLookup[Byte0], IPInc8);
            }
            else if (ParameterlessLookup[Byte0])
            {
                printf("%s", ParameterlessLookup[Byte0]);
            }
            else
            {
                printf("Encountered unknown opcode 0x%02x\n", Byte0);
                exit(1);
            }
            if (SetFlags)
            {
                FlagZero = !(Result & 0xFFFF);
                FlagSign = Result & 0x8000;
                FlagCarry = (Result >> 16) ^ IsSub;
                u8 BitsSet = (((Result >>  0) & 1) +
                              ((Result >>  1) & 1) +
                              ((Result >>  2) & 1) +
                              ((Result >>  3) & 1) +
                              ((Result >>  4) & 1) +
                              ((Result >>  5) & 1) +
                              ((Result >>  6) & 1) +
                              ((Result >>  7) & 1) +
                              ((Result >>  8) & 1) +
                              ((Result >>  9) & 1) +
                              ((Result >> 10) & 1) +
                              ((Result >> 11) & 1) +
                              ((Result >> 12) & 1) +
                              ((Result >> 13) & 1) +
                              ((Result >> 14) & 1) +
                              ((Result >> 15) & 1));
#if 0
                printf("\n; DEBUG: 0x%04x %d\n", Result, BitsSet);
#endif
                FlagParity = !(BitsSet & 1);
                FlagOverflow = ((DestValue & SourceValue & ~(u16)Result) | (~DestValue & ~SourceValue & (u16)Result)) >> 15;
            }
            printf(" ; IP: 0x%04x, Flags: %s%s%s%s%s\n", PreviousInstructionPointer, FlagOverflow?"O":"",FlagCarry?"C":"", FlagZero?"Z":"", FlagSign?"S":"", FlagParity?"P":"");
#if 0
            printf("; DEBUG: 0x%04x, 0x%04x, 0x%04x, 0x%04x\n", DestValue, SourceValue, (u16)Result, FlagOverflow);
#endif
#if 0
            u8 FriendlyRegisterOrder[] = { 0, 3, 1, 2, 4, 5, 6, 7 };
            printf(";Registers:\n");
            for (int FregisterIndex = 0; FregisterIndex < ArrayCount(FriendlyRegisterOrder); ++FregisterIndex)
            {
                u8 RegisterIndex = FriendlyRegisterOrder[FregisterIndex];
                printf(";\t%s: 0x%04x (%hu)\n", RegisterLookup[8|RegisterIndex], (u16)Registers[RegisterIndex], Registers[RegisterIndex]);
            }
            u8 FriendlySegmentRegisterOrder[] = { 0, 1, 2, 3 };
            for (int FregisterIndex = 0; FregisterIndex < ArrayCount(FriendlySegmentRegisterOrder); ++FregisterIndex)
            {
                u8 RegisterIndex = FriendlySegmentRegisterOrder[FregisterIndex];
                printf(";\t%s: 0x%04x (%hu)\n", SegmentRegisterLookup[RegisterIndex], (u16)SegmentRegisters[RegisterIndex], SegmentRegisters[RegisterIndex]);
            }
#endif
        }
    }
    else
    {
        printf("Please provide a single path argument to a machine code file.");
    }
    u8 FriendlyRegisterOrder[] = { 0, 3, 1, 2, 4, 5, 6, 7 };
    printf(";Final registers:\n");
    for (int FregisterIndex = 0; FregisterIndex < ArrayCount(FriendlyRegisterOrder); ++FregisterIndex)
    {
        u8 RegisterIndex = FriendlyRegisterOrder[FregisterIndex];
        printf(";\t%s: 0x%04x (%hu)\n", RegisterLookup[8|RegisterIndex], (u16)Registers[RegisterIndex], Registers[RegisterIndex]);
    }
    u8 FriendlySegmentRegisterOrder[] = { 0, 1, 2, 3 };
    for (int FregisterIndex = 0; FregisterIndex < ArrayCount(FriendlySegmentRegisterOrder); ++FregisterIndex)
    {
        u8 RegisterIndex = FriendlySegmentRegisterOrder[FregisterIndex];
        printf(";\t%s: 0x%04x (%hu)\n", SegmentRegisterLookup[RegisterIndex], (u16)SegmentRegisters[RegisterIndex], SegmentRegisters[RegisterIndex]);
    }
    printf(";\tIP: 0x%04x (%hu)\n", InstructionPointer, InstructionPointer);
}
