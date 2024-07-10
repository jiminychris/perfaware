#include "stdio.h"

struct instruction
{
    unsigned char Byte0;
    unsigned char Byte1;
};

#define REG_MODE_SOURCE 0
#define REG_MODE_DESTINATION 1

#define DATA_SIZE_BYTE 0
#define DATA_SIZE_WORD 1

#define MODE_MEMORY_DISPLACEMENT_NONE 0
#define MODE_MEMORY_DISPLACEMENT_8_BIT 1
#define MODE_MEMORY_DISPLACEMENT_16_BIT 2
#define MODE_REGISTER 3

#define OPCODE_MOV 0x88

char RegisterLookup[] =
{
    "AL"
    "CL"
    "DL"
    "BL"
    "AH"
    "CH"
    "DH"
    "BH"
    "AX"
    "CX"
    "DX"
    "BX"
    "SP"
    "BP"
    "SI"
    "DI"
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
        struct instruction Instructions[2048];
        struct instruction Instruction;
        size_t InstructionsRead = fread(Instructions, sizeof(struct instruction), ArrayCount(Instructions), MachineCodeFile);
        int InstructionIndex = 0;
        while (InstructionsRead--)
        {
            Instruction = Instructions[InstructionIndex++];
            if (OPCODE_MOV == (Instruction.Byte0 & 0xFC))
            {
                unsigned char RegMode = (Instruction.Byte0 >> 1) & 1;
                unsigned char DataSize = Instruction.Byte0 & 1;
                unsigned char Mode = (Instruction.Byte1 >> 6);
                unsigned char Reg = (Instruction.Byte1 >> 3) & 7;
                unsigned char RM = Instruction.Byte1 & 7;
                if (Mode == MODE_REGISTER)
                {
                    char *Registers[2];
                    unsigned char RegIndex = Reg | (DataSize << 3);
                    unsigned char RMIndex = RM | (DataSize << 3);
                    Registers[!RegMode] = RegisterLookup + (RegIndex << 1);
                    Registers[RegMode] = RegisterLookup + (RMIndex << 1);
                    printf("mov %.2s, %.2s\n", Registers[0], Registers[1]);
                }
                else
                {
                    printf("Encountered unsupported mode %#04x\n", Mode);
                }
            }
            else
            {
                printf("Encountered unknown opcode %#04x\n", Instruction.Byte0);
            }
        }
    }
    else
    {
        printf("Please provide a single path argument to a machine code file.");
    }
}
