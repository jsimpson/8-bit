#include <stdio.h>
#include "disassembler.h"

int
disassemble(unsigned char * buf, int program_counter)
{
    static instruction_t instructions[] = {
        { 0, "NOP" },           // 0x00
        { 2, "LXI     B, #" },  // 0x01
        { 0, "STAX    B" },     // 0x02
        { 0, "INX     B" },     // 0x03
        { 0, "INR     B" },     // 0x04
        { 0, "DCR     B" },     // 0x05
        { 1, "MVI     B, #" },  // 0x06
        { 0, "RLC" },           // 0x07
        { 0 },                  // 0x08
        { 0, "DAD     B" },     // 0x09
        { 0, "LDAX    B" },     // 0x0A
        { 0, "DCX     B" },     // 0x0B
        { 0, "INR     C" },     // 0x0C
        { 0, "DCR     C" },     // 0x0D
        { 1, "MVI     C, #" },  // 0x0E
        { 0, "RRC" },           // 0x0F
        { 0 },                  // 0x10
        { 2, "LXI     D, #" },  // 0x11
        { 0, "STAX    D" },     // 0x12
        { 0, "INX     D" },     // 0x13
        { 0, "INR     D" },     // 0x14
        { 0, "DCR     D" },     // 0x15
        { 1, "MVI     D, #" },  // 0x16
        { 0, "RAL" },           // 0x17
        { 0 },                  // 0x18
        { 0, "DAD     D" },     // 0x19
        { 0, "LDAX    D" },     // 0x1A
        { 0, "DCX     D" },     // 0x1B
        { 0, "INR     E" },     // 0x1C
        { 0, "DCR     E" },     // 0x1D
        { 1, "MVI     E, " },   // 0x1E
        { 0, "RAR" },           // 0x1F
        { 0, "RIM" },           // 0x20
        { 2, "LXI     H, #" },  // 0x21
        { 2, "SHLD    " },      // 0x22
        { 0, "INX     H" },     // 0x23
        { 0, "INR     H" },     // 0x24
        { 0, "DCR     H" },     // 0x25
        { 1, "MVI     H, #" },  // 0x26
        { 0, "DAA" },           // 0x27
        { 0 },                  // 0x28
        { 0, "DAD     H" },     // 0x29
        { 2, "LHLD    , " },    // 0x2A
        { 0, "DCX     H" },     // 0x2B
        { 0, "INR     L" },     // 0x2C
        { 0, "DCR     L" },     // 0x2D
        { 1, "MVI     L, #" },  // 0x2E
        { 0, "CMA" },           // 0x2F
        { 0, "SIM" },           // 0x30
        { 2, "LXI     SP, #" }, // 0x31
        { 2, "STA     " },      // 0x32
        { 0, "INX     SP" },    // 0x33
        { 0, "INR     M" },     // 0x34
        { 0, "DCR     M" },     // 0x35
        { 1, "MVI     M, #" },  // 0x36
        { 0, "STC" },           // 0x37
        { 0 },                  // 0x38
        { 0, "DAD     SP" },    // 0x39
        { 2, "LDA     " },      // 0x3A
        { 0, "DCX     SP" },    // 0x3B
        { 0, "INR     A" },     // 0x3C
        { 0, "DCR     A" },     // 0x3D
        { 1, "MVI     A, #" },  // 0x3E
        { 0, "CMC" },           // 0x3F
        { 0, "MOV     B, B" },  // 0x40
        { 0, "MOV     B, C" },  // 0x41
        { 0, "MOV     B, D" },  // 0x42
        { 0, "MOV     B, E" },  // 0x43
        { 0, "MOV     B, H" },  // 0x44
        { 0, "MOV     B, L" },  // 0x45
        { 0, "MOV     B, M" },  // 0x46
        { 0, "MOV     B, A" },  // 0x47
        { 0, "MOV     C, B" },  // 0x48
        { 0, "MOV     C, C" },  // 0x49
        { 0, "MOV     C, D" },  // 0x4A
        { 0, "MOV     C, E" },  // 0x4B
        { 0, "MOV     C, H" },  // 0x4C
        { 0, "MOV     C, L" },  // 0x4D
        { 0, "MOV     C, M" },  // 0x4E
        { 0, "MOV     C, A" },  // 0x4F
        { 0, "MOV     D, B" },  // 0x50
        { 0, "MOV     D, C" },  // 0x51
        { 0, "MOV     D, D" },  // 0x52
        { 0, "MOV     D, E" },  // 0x53
        { 0, "MOV     D, H" },  // 0x54
        { 0, "MOV     D, L" },  // 0x55
        { 0, "MOV     D, M" },  // 0x56
        { 0, "MOV     D, A" },  // 0x57
        { 0, "MOV     E, B" },  // 0x58
        { 0, "MOV     E, C" },  // 0x59
        { 0, "MOV     E, D" },  // 0x5A
        { 0, "MOV     E, E" },  // 0x5B
        { 0, "MOV     E, H" },  // 0x5C
        { 0, "MOV     E, L" },  // 0x5D
        { 0, "MOV     E, M" },  // 0x5E
        { 0, "MOV     E, A" },  // 0x5F
        { 0, "MOV     H, B" },  // 0x60
        { 0, "MOV     H, C" },  // 0x61
        { 0, "MOV     H, D" },  // 0x62
        { 0, "MOV     H, E" },  // 0x63
        { 0, "MOV     H, H" },  // 0x64
        { 0, "MOV     H, L" },  // 0x65
        { 0, "MOV     H, M" },  // 0x66
        { 0, "MOV     H, A" },  // 0x67
        { 0, "MOV     L, B" },  // 0x68
        { 0, "MOV     L, C" },  // 0x69
        { 0, "MOV     L, D" },  // 0x6A
        { 0, "MOV     L, E" },  // 0x6B
        { 0, "MOV     L, H" },  // 0x6C
        { 0, "MOV     L, L" },  // 0x6D
        { 0, "MOV     L, M" },  // 0x6E
        { 0, "MOV     L, A" },  // 0x6F
        { 0, "MOV     M, B" },  // 0x70
        { 0, "MOV     M, C" },  // 0x71
        { 0, "MOV     M, D" },  // 0x72
        { 0, "MOV     M, E" },  // 0x73
        { 0, "MOV     M, H" },  // 0x74
        { 0, "MOV     M, L" },  // 0x75
        { 0, "HLT" },           // 0x76
        { 0, "MOV     M, A" },  // 0x77
        { 0, "MOV     A, B" },  // 0x78
        { 0, "MOV     A, C" },  // 0x79
        { 0, "MOV     A, D" },  // 0x7A
        { 0, "MOV     A, E" },  // 0x7B
        { 0, "MOV     A, H" },  // 0x7C
        { 0, "MOV     A, L" },  // 0x7D
        { 0, "MOV     A, M" },  // 0x7E
        { 0, "MOV     A, A" },  // 0x7F
        { 0, "ADD     B" },     // 0x80
        { 0, "ADD     C" },     // 0x81
        { 0, "ADD     D" },     // 0x82
        { 0, "ADD     E" },     // 0x83
        { 0, "ADD     H" },     // 0x84
        { 0, "ADD     L" },     // 0x85
        { 0, "ADD     M" },     // 0x86
        { 0, "ADD     A" },     // 0x87
        { 0, "ADC     B" },     // 0x88
        { 0, "ADC     C" },     // 0x89
        { 0, "ADC     D" },     // 0x8A
        { 0, "ADC     E" },     // 0x8B
        { 0, "ADC     H" },     // 0x8C
        { 0, "ADC     L" },     // 0x8D
        { 0, "ADC     M" },     // 0x8E
        { 0, "ADC     A" },     // 0x8F
        { 0, "SUB     B" },     // 0x90
        { 0, "SUB     C" },     // 0x91
        { 0, "SUB     D" },     // 0x92
        { 0, "SUB     E" },     // 0x93
        { 0, "SUB     H" },     // 0x94
        { 0, "SUB     L" },     // 0x95
        { 0, "SUB     M" },     // 0x96
        { 0, "SUB     A" },     // 0x97
        { 0, "SBB     B" },     // 0x98
        { 0, "SBB     C" },     // 0x99
        { 0, "SBB     D" },     // 0x9A
        { 0, "SBB     E" },     // 0x9B
        { 0, "SBB     H" },     // 0x9C
        { 0, "SBB     L" },     // 0x9D
        { 0, "SBB     M" },     // 0x9E
        { 0, "SBB     A" },     // 0x9F
        { 0, "ANA     B" },     // 0xA0
        { 0, "ANA     C" },     // 0xA1
        { 0, "ANA     D" },     // 0xA2
        { 0, "ANA     E" },     // 0xA3
        { 0, "ANA     H" },     // 0xA4
        { 0, "ANA     L" },     // 0xA5
        { 0, "ANA     M" },     // 0xA6
        { 0, "ANA     A" },     // 0xA7
        { 0, "XRA     B" },     // 0xA8
        { 0, "XRA     C" },     // 0xA9
        { 0, "XRA     D" },     // 0xAA
        { 0, "XRA     E" },     // 0xAB
        { 0, "XRA     H" },     // 0xAC
        { 0, "XRA     L" },     // 0xAD
        { 0, "XRA     M" },     // 0xAE
        { 0, "XRA     A" },     // 0xAF
        { 0, "ORA     B" },     // 0xB0
        { 0, "ORA     C" },     // 0xB1
        { 0, "ORA     D" },     // 0xB2
        { 0, "ORA     E" },     // 0xB3
        { 0, "ORA     H" },     // 0xB4
        { 0, "ORA     L" },     // 0xB5
        { 0, "ORA     M" },     // 0xB6
        { 0, "ORA     A" },     // 0xB7
        { 0, "CMP     B" },     // 0xB8
        { 0, "CMP     C" },     // 0xB9
        { 0, "CMP     D" },     // 0xBA
        { 0, "CMP     E" },     // 0xBB
        { 0, "CMP     H" },     // 0xBC
        { 0, "CMP     L" },     // 0xBD
        { 0, "CMP     M" },     // 0xBE
        { 0, "CMP     A" },     // 0xBF
        { 0, "RNZ" },           // 0xC0
        { 0, "POP     B" },     // 0xC1
        { 2, "JNZ     " },      // 0xC2
        { 2, "JMP     " },      // 0xC3
        { 2, "CNZ     " },      // 0xC4
        { 0, "PUSH    B" },     // 0xc5
        { 1, "ADI     #" },     // 0xC6
        { 0, "RST     0" },     // 0xC7
        { 0, "RZ" },            // 0xC8
        { 0, "RET" },           // 0xC9
        { 2, "JZ      " },      // 0xCA
        { 0 },                  // 0xCB
        { 2, "CZ      " },      // 0xCC
        { 2, "CALL    " },      // 0xCD
        { 1, "ACI     " },      // 0xCE
        { 0, "RST     1" },     // 0xCF
        { 0, "RNC" },           // 0xD0
        { 0, "POP     D" },     // 0xD1
        { 2, "JNC     " },      // 0xD2
        { 1, "OUT     #" },     // 0xD3
        { 2, "CNC     " },      // 0xD4
        { 0, "PUSH    D" },     // 0xD5
        { 1, "SUI     #" },     // 0xD6
        { 0, "RST     2" },     // 0xD7
        { 0, "RC" },            // 0xD8
        { 0 },                  // 0xD9
        { 2, "JC      " },      // 0xDA
        { 1, "IN      " },      // 0xDB
        { 2, "CC      " },      // 0xDC
        { 0 },                  // 0xDD
        { 0, "SBI" },           // 0xDE
        { 0, "RST     3" },     // 0xDF
        { 0, "RPO" },           // 0xE0
        { 0, "POP     H" },     // 0xE1
        { 2, "JPO     " },      // 0xE2
        { 0, "XTHL" },          // 0xE3
        { 2, "CPO     " },      // 0xE4
        { 0, "PUSH    H" },     // 0xE5
        { 1, "ANI     #" },     // 0xE6
        { 0, "RST     4" },     // 0xE7
        { 0, "RPE" },           // 0xE8
        { 0, "PCHL" },          // 0xE9
        { 2, "JPE     " },      // 0xEA
        { 0, "XCHG" },          // 0xEB
        { 2, "CPE     " },      // 0xEC
        { 0 },                  // 0xED
        { 1, "XRI     #" },     // 0xEE
        { 0, "RST     5" },     // 0xEF
        { 0, "RP" },            // 0xF0
        { 0, "POP     PSW" },   // 0xF1
        { 2, "JP      " },      // 0xF2
        { 0, "DI" },            // 0xF3
        { 0, "CP" },            // 0xF4
        { 0, "PUSH    PSW" },   // 0xF5
        { 1, "ORI     #" },     // 0xF6
        { 0, "RST     6" },     // 0xF7
        { 0, "RM" },            // 0xF8
        { 0, "SPHL" },          // 0xF9
        { 2, "JM      " },      // 0xFA
        { 0, "EI" },            // 0xFB
        { 2, "CM      " },      // 0xFC
        { 0 },                  // 0xFD
        { 1, "CPI     #" },     // 0xFE
        { 0, "RST     7" },     // 0xFF
    };

    unsigned char * opcode = &buf[program_counter];
    int bytes = 1;

    printf("%04X: ", program_counter);

    if (
            (*opcode == 0x08) ||
            (*opcode == 0x10) ||
            (*opcode == 0x18) ||
            (*opcode == 0x28) ||
            (*opcode == 0x38) ||
            (*opcode == 0xCB) ||
            (*opcode == 0xD9) ||
            (*opcode == 0xDD) ||
            (*opcode == 0xED) ||
            (*opcode == 0xFD)
       )
    {
        // Illegal / unsupported opcode.
        printf("%02X", *opcode);
    }
    else
    {
        printf("%s", instructions[*opcode].mnemonic);
        switch (instructions[*opcode].size)
        {
            case 1:
                printf("$%02X", opcode[1]);
                break;
            case 2:
                printf("$%02X%02X", opcode[2], opcode[1]);
                break;
        }

        bytes += instructions[*opcode].size;
    }

    printf("\n");
    return bytes;
}
