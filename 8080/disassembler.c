#include <stdio.h>
#include "disassembler.h"

typedef struct instruction
{
    char size;
    char mnemonic[255];
} instruction_t;

static instruction_t instructions[] = {
    { 1, "NOP" },         // 0x00
    { 3, "LXI    B" },    // 0x01
    { 1, "STAX   B" },    // 0x02
    { 1, "INX    B" },    // 0x03
    { 1, "INR    B" },    // 0x04
    { 1, "DCR    B" },    // 0x05
    { 2, "MVI    B" },    // 0x06
    { 1, "RLC" },         // 0x07
    { 0 },                // 0x08
    { 1, "DAD    B" },    // 0x09
    { 1, "LDAX   B" },    // 0x0A
    { 1, "DCX    B" },    // 0x0B
    { 1, "INR    C" },    // 0x0C
    { 1, "DCR    C" },    // 0x0D
    { 2, "MVI    C" },    // 0x0E
    { 1, "RRC" },         // 0x0F
    { 0 },                // 0x10
    { 3, "LXI    D" },    // 0x11
    { 1, "STAX   D" },    // 0x12
    { 1, "INX    D" },    // 0x13
    { 1, "INR    D" },    // 0x14
    { 1, "DCR    D" },    // 0x15
    { 2, "MVI    D" },    // 0x16
    { 1, "RAL" },         // 0x17
    { 0 },                // 0x18
    { 1, "DAD    D" },    // 0x19
    { 1, "LDAX   D" },    // 0x1A
    { 1, "DCX    D" },    // 0x1B
    { 1, "INR    E" },    // 0x1C
    { 1, "DCR    E" },    // 0x1D
    { 2, "MVI    E" },    // 0x1E
    { 1, "RAR" },         // 0x1F
    { 1, "RIM" },         // 0x20
    { 3, "LXI    H" },    // 0x21
    { 3, "SHLD    " },    // 0x22
    { 1, "INX    H" },    // 0x23
    { 1, "INR    H" },    // 0x24
    { 1, "DCR    H" },    // 0x25
    { 2, "MVI    H" },    // 0x26
    { 1, "DAA" },         // 0x27
    { 0 },                // 0x28
    { 1, "DAD    H" },    // 0x29
    { 3, "LHLD    " },    // 0x2A
    { 1, "DCX    H" },    // 0x2B
    { 1, "INR    L" },    // 0x2C
    { 1, "DCR    L" },    // 0x2D
    { 2, "MVI    L" },    // 0x2E
    { 1, "CMA" },         // 0x2F
    { 1, "SIM" },         // 0x30
    { 3, "LXI   SP" },    // 0x31
    { 3, "STA     " },    // 0x32
    { 1, "INX   SP" },    // 0x33
    { 1, "INR    M" },    // 0x34
    { 1, "DCR    M" },    // 0x35
    { 2, "MVI    M" },    // 0x36
    { 1, "STC" },         // 0x37
    { 0 },                // 0x38
    { 1, "DAD   SP" },    // 0x39
    { 3, "LDA     " },    // 0x3A
    { 1, "DCX   SP" },    // 0x3B
    { 1, "INR    A" },    // 0x3C
    { 1, "DCR    A" },    // 0x3D
    { 2, "MVI    A" },    // 0x3E
    { 1, "CMC" },         // 0x3F
    { 1, "MOV    B, B" }, // 0x40
    { 1, "MOV    B, C" }, // 0x41
    { 1, "MOV    B, D" }, // 0x42
    { 1, "MOV    B, E" }, // 0x43
    { 1, "MOV    B, H" }, // 0x44
    { 1, "MOV    B, L" }, // 0x45
    { 1, "MOV    B, M" }, // 0x46
    { 1, "MOV    B, A" }, // 0x47
    { 1, "MOV    C, B" }, // 0x48
    { 1, "MOV    C, C" }, // 0x49
    { 1, "MOV    C, D" }, // 0x4A
    { 1, "MOV    C, E" }, // 0x4B
    { 1, "MOV    C, H" }, // 0x4C
    { 1, "MOV    C, L" }, // 0x4D
    { 1, "MOV    C, M" }, // 0x4E
    { 1, "MOV    C, A" }, // 0x4F
    { 1, "MOV    D, B" }, // 0x50
    { 1, "MOV    D, C" }, // 0x51
    { 1, "MOV    D, D" }, // 0x52
    { 1, "MOV    D, E" }, // 0x53
    { 1, "MOV    D, H" }, // 0x54
    { 1, "MOV    D, L" }, // 0x55
    { 1, "MOV    D, M" }, // 0x56
    { 1, "MOV    D, A" }, // 0x57
    { 1, "MOV    E, B" }, // 0x58
    { 1, "MOV    E, C" }, // 0x59
    { 1, "MOV    E, D" }, // 0x5A
    { 1, "MOV    E, E" }, // 0x5B
    { 1, "MOV    E, H" }, // 0x5C
    { 1, "MOV    E, L" }, // 0x5D
    { 1, "MOV    E, M" }, // 0x5E
    { 1, "MOV    E, A" }, // 0x5F
    { 1, "MOV    H, B" }, // 0x60
    { 1, "MOV    H, C" }, // 0x61
    { 1, "MOV    H, D" }, // 0x62
    { 1, "MOV    H, E" }, // 0x63
    { 1, "MOV    H, H" }, // 0x64
    { 1, "MOV    H, L" }, // 0x65
    { 1, "MOV    H, M" }, // 0x66
    { 1, "MOV    H, A" }, // 0x67
    { 1, "MOV    L, B" }, // 0x68
    { 1, "MOV    L, C" }, // 0x69
    { 1, "MOV    L, D" }, // 0x6A
    { 1, "MOV    L, E" }, // 0x6B
    { 1, "MOV    L, H" }, // 0x6C
    { 1, "MOV    L, L" }, // 0x6D
    { 1, "MOV    L, M" }, // 0x6E
    { 1, "MOV    L, A" }, // 0x6F
    { 1, "MOV    M, B" }, // 0x70
    { 1, "MOV    M, C" }, // 0x71
    { 1, "MOV    M, D" }, // 0x72
    { 1, "MOV    M, E" }, // 0x73
    { 1, "MOV    M, H" }, // 0x74
    { 1, "MOV    M, L" }, // 0x75
    { 1, "HLT" },         // 0x76
    { 1, "MOV    M, A" }, // 0x77
    { 1, "MOV    A, B" }, // 0x78
    { 1, "MOV    A, C" }, // 0x79
    { 1, "MOV    A, D" }, // 0x7A
    { 1, "MOV    A, E" }, // 0x7B
    { 1, "MOV    A, H" }, // 0x7C
    { 1, "MOV    A, L" }, // 0x7D
    { 1, "MOV    A, M" }, // 0x7E
    { 1, "MOV    A, A" }, // 0x7F
    { 1, "ADD    B" },    // 0x80
    { 1, "ADD    C" },    // 0x81
    { 1, "ADD    D" },    // 0x82
    { 1, "ADD    E" },    // 0x83
    { 1, "ADD    H" },    // 0x84
    { 1, "ADD    L" },    // 0x85
    { 1, "ADD    M" },    // 0x86
    { 1, "ADD    A" },    // 0x87
    { 1, "ADC    B" },    // 0x88
    { 1, "ADC    C" },    // 0x89
    { 1, "ADC    D" },    // 0x8A
    { 1, "ADC    E" },    // 0x8B
    { 1, "ADC    H" },    // 0x8C
    { 1, "ADC    L" },    // 0x8D
    { 1, "ADC    M" },    // 0x8E
    { 1, "ADC    A" },    // 0x8F
    { 1, "SUB    B" },    // 0x90
    { 1, "SUB    C" },    // 0x91
    { 1, "SUB    D" },    // 0x92
    { 1, "SUB    E" },    // 0x93
    { 1, "SUB    H" },    // 0x94
    { 1, "SUB    L" },    // 0x95
    { 1, "SUB    M" },    // 0x96
    { 1, "SUB    A" },    // 0x97
    { 1, "SBB    B" },    // 0x98
    { 1, "SBB    C" },    // 0x99
    { 1, "SBB    D" },    // 0x9A
    { 1, "SBB    E" },    // 0x9B
    { 1, "SBB    H" },    // 0x9C
    { 1, "SBB    L" },    // 0x9D
    { 1, "SBB    M" },    // 0x9E
    { 1, "SBB    A" },    // 0x9F
    { 1, "ANA    B" },    // 0xA0
    { 1, "ANA    C" },    // 0xA1
    { 1, "ANA    D" },    // 0xA2
    { 1, "ANA    E" },    // 0xA3
    { 1, "ANA    H" },    // 0xA4
    { 1, "ANA    L" },    // 0xA5
    { 1, "ANA    M" },    // 0xA6
    { 1, "ANA    A" },    // 0xA7
    { 1, "XRA    B" },    // 0xA8
    { 1, "XRA    C" },    // 0xA9
    { 1, "XRA    D" },    // 0xAA
    { 1, "XRA    E" },    // 0xAB
    { 1, "XRA    H" },    // 0xAC
    { 1, "XRA    L" },    // 0xAD
    { 1, "XRA    M" },    // 0xAE
    { 1, "XRA    A" },    // 0xAF
    { 1, "ORA    B" },    // 0xB0
    { 1, "ORA    C" },    // 0xB1
    { 1, "ORA    D" },    // 0xB2
    { 1, "ORA    E" },    // 0xB3
    { 1, "ORA    H" },    // 0xB4
    { 1, "ORA    L" },    // 0xB5
    { 1, "ORA    M" },    // 0xB6
    { 1, "ORA    A" },    // 0xB7
    { 1, "CMP    B" },    // 0xB8
    { 1, "CMP     C" },   // 0xB9
    { 1, "CMP    D" },    // 0xBA
    { 1, "CMP    E" },    // 0xBB
    { 1, "CMP    H" },    // 0xBC
    { 1, "CMP    L" },    // 0xBD
    { 1, "CMP    M" },    // 0xBE
    { 1, "CMP    A" },    // 0xBF
    { 1, "RNZ     " },    // 0xC0
    { 1, "POP    B" },    // 0xC1
    { 3, "JNZ     " },    // 0xC2
    { 3, "JMP     " },    // 0xC3
    { 3, "CNZ     " },    // 0xC4
    { 1, "PUSH   B" },    // 0xc5
    { 2, "ADI     " },    // 0xC6
    { 1, "RST    0" },    // 0xC7
    { 1, "RZ" },          // 0xC8
    { 1, "RET" },         // 0xC9
    { 3, "JZ      " },    // 0xCA
    { 0 },                // 0xCB
    { 3, "CZ      " },    // 0xCC
    { 3, "CALL    " },    // 0xCD
    { 2, "ACI     " },    // 0xCE
    { 1, "RST    1" },    // 0xCF
    { 1, "RNC" },         // 0xD0
    { 1, "POP    D" },    // 0xD1
    { 3, "JNC     " },    // 0xD2
    { 2, "OUT     " },    // 0xD3
    { 3, "CNC     " },    // 0xD4
    { 1, "PUSH   D" },    // 0xD5
    { 2, "SUI     " },    // 0xD6
    { 1, "RST    2" },    // 0xD7
    { 1, "RC" },          // 0xD8
    { 0 },                // 0xD9
    { 3, "JC      " },    // 0xDA
    { 2, "IN      " },    // 0xDB
    { 3, "CC      " },    // 0xDC
    { 0 },                // 0xDD
    { 1, "SBI" },         // 0xDE
    { 1, "RST    3" },    // 0xDF
    { 1, "RPO" },         // 0xE0
    { 1, "POP    H" },    // 0xE1
    { 3, "JPO     " },    // 0xE2
    { 1, "XTHL" },        // 0xE3
    { 3, "CPO     " },    // 0xE4
    { 1, "PUSH   H" },    // 0xE5
    { 2, "ANI     " },    // 0xE6
    { 1, "RST    4" },    // 0xE7
    { 1, "RPE" },         // 0xE8
    { 1, "PCHL" },        // 0xE9
    { 3, "JPE     " },    // 0xEA
    { 1, "XCHG" },        // 0xEB
    { 3, "CPE     " },    // 0xEC
    { 0 },                // 0xED
    { 2, "XRI     " },    // 0xEE
    { 1, "RST    5" },    // 0xEF
    { 1, "RP" },          // 0xF0
    { 1, "POP  PSW" },    // 0xF1
    { 3, "JP      " },    // 0xF2
    { 1, "DI" },          // 0xF3
    { 1, "CP" },          // 0xF4
    { 1, "PUSH PSW" },    // 0xF5
    { 1, "RST    6" },    // 0xF7
    { 1, "RM" },          // 0xF8
    { 1, "SPHL" },        // 0xF9
    { 3, "JM      " },    // 0xFA
    { 1, "EI" },          // 0xFB
    { 3, "CM      " },    // 0xFC
    { 0 },                // 0xFD
    { 2, "CPI     " },    // 0xFE
    { 1, "RST    7" },    // 0xFF
};

int
disassemble(unsigned char * buf, int offset)
{
    unsigned char * opcode = &buf[offset];
    if (instructions[*opcode].size == 0)
    {
        // Illegal opcode.
        return 1;
    }
    else
    {
        printf("%04X [$%02X]: %s", offset, opcode[0], instructions[*opcode].mnemonic);
        if (instructions[*opcode].size == 2)
        {
            printf(",#$%02X", opcode[1]);
        }
        else if (instructions[*opcode].size == 3)
        {
            printf(",$%02X%02X", opcode[2], opcode[1]);
        }
        printf("\n");

        return instructions[*opcode].size;
    }
}
