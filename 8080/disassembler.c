#include <stdio.h>
#include "disassembler.h"

int
disassemble(unsigned char * buf, char * disassembled, int program_counter)
{
    enum { BYTE = 1, WORD = 2, ADDRESS = 1 };

    // https://pastraiser.com/cpu/i8080/i8080_opcodes.html
    static instruction_t instructions[] = {
        { 0,    "NOP",        0       }, // 0x00
        { WORD, "LXI    B,",  0       }, // 0x01
        { 0,    "STAX   B",   0       }, // 0x02
        { 0,    "INX    B",   0       }, // 0x03
        { 0,    "INR    B",   0       }, // 0x04
        { 0,    "DCR    B",   0       }, // 0x05
        { BYTE, "MVI    B,",  0       }, // 0x06
        { 0,    "RLC",        0       }, // 0x07
        { 0,    "NOP",        0       }, // 0x08
        { 0,    "DAD    B",   0       }, // 0x09
        { 0,    "LDAX   B",   0       }, // 0x0A
        { 0,    "DCX    B",   0       }, // 0x0B
        { 0,    "INR    C",   0       }, // 0x0C
        { 0,    "DCR    C",   0       }, // 0x0D
        { BYTE, "MVI    C,",  0       }, // 0x0E
        { 0,    "RRC",        0       }, // 0x0F

        { 0,    "NOP",        0       }, // 0x10
        { WORD, "LXI    D,",  0       }, // 0x11
        { 0,    "STAX   D",   0       }, // 0x12
        { 0,    "INX    D",   0       }, // 0x13
        { 0,    "INR    D",   0       }, // 0x14
        { 0,    "DCR    D",   0       }, // 0x15
        { BYTE, "MVI    D,",  0       }, // 0x16
        { 0,    "RAL",        0       }, // 0x17
        { 0,    "NOP",        0       }, // 0x18
        { 0,    "DAD    D",   0       }, // 0x19
        { 0,    "LDAX   D",   0       }, // 0x1A
        { 0,    "DCX    D",   0       }, // 0x1B
        { 0,    "INR    E",   0       }, // 0x1C
        { 0,    "DCR    E",   0       }, // 0x1D
        { BYTE, "MVI    E,",  0       }, // 0x1E
        { 0,    "RAR",        0       }, // 0x1F

        { 0,    "NOP",        0       }, // 0x20
        { WORD, "LXI    H,",  0       }, // 0x21
        { WORD, "SHLD   ",    ADDRESS }, // 0x22
        { 0,    "INX    H",   0       }, // 0x23
        { 0,    "INR    H",   0       }, // 0x24
        { 0,    "DCR    H",   0       }, // 0x25
        { BYTE, "MVI    H,",  0       }, // 0x26
        { 0,    "DAA",        0       }, // 0x27
        { 0,    "NOP",        0       }, // 0x28
        { 0,    "DAD    H",   0       }, // 0x29
        { WORD, "LHLD   ",    ADDRESS }, // 0x2A
        { 0,    "DCX    H",   0       }, // 0x2B
        { 0,    "INR    L",   0       }, // 0x2C
        { 0,    "DCR    L",   0       }, // 0x2D
        { BYTE, "MVI    L,",  0       }, // 0x2E
        { 0,    "CMA",        0       }, // 0x2F

        { 0,    "NOP",        0       }, // 0x30
        { WORD, "LXI    SP,", 0       }, // 0x31
        { WORD, "STA    ",    ADDRESS }, // 0x32
        { 0,    "INX    SP",  0       }, // 0x33
        { 0,    "INR    M",   0       }, // 0x34
        { 0,    "DCR    M",   0       }, // 0x35
        { BYTE, "MVI    M,",  0       }, // 0x36
        { 0,    "STC",        0       }, // 0x37
        { 0,    "NOP",        0       }, // 0x38
        { 0,    "DAD    SP",  0       }, // 0x39
        { WORD, "LDA    ",    ADDRESS }, // 0x3A
        { 0,    "DCX    SP",  0       }, // 0x3B
        { 0,    "INR    A",   0       }, // 0x3C
        { 0,    "DCR    A",   0       }, // 0x3D
        { BYTE, "MVI    A,",  0       }, // 0x3E
        { 0,    "CMC",        0       }, // 0x3F

        { 0,    "MOV    B,B", 0       }, // 0x40
        { 0,    "MOV    B,C", 0       }, // 0x41
        { 0,    "MOV    B,D", 0       }, // 0x42
        { 0,    "MOV    B,E", 0       }, // 0x43
        { 0,    "MOV    B,H", 0       }, // 0x44
        { 0,    "MOV    B,L", 0       }, // 0x45
        { 0,    "MOV    B,M", 0       }, // 0x46
        { 0,    "MOV    B,A", 0       }, // 0x47
        { 0,    "MOV    C,B", 0       }, // 0x48
        { 0,    "MOV    C,C", 0       }, // 0x49
        { 0,    "MOV    C,D", 0       }, // 0x4A
        { 0,    "MOV    C,E", 0       }, // 0x4B
        { 0,    "MOV    C,H", 0       }, // 0x4C
        { 0,    "MOV    C,L", 0       }, // 0x4D
        { 0,    "MOV    C,M", 0       }, // 0x4E
        { 0,    "MOV    C,A", 0       }, // 0x4F

        { 0,    "MOV    D,B", 0       }, // 0x50
        { 0,    "MOV    D,C", 0       }, // 0x51
        { 0,    "MOV    D,D", 0       }, // 0x52
        { 0,    "MOV    D,E", 0       }, // 0x53
        { 0,    "MOV    D,H", 0       }, // 0x54
        { 0,    "MOV    D,L", 0       }, // 0x55
        { 0,    "MOV    D,M", 0       }, // 0x56
        { 0,    "MOV    D,A", 0       }, // 0x57
        { 0,    "MOV    E,B", 0       }, // 0x58
        { 0,    "MOV    E,C", 0       }, // 0x59
        { 0,    "MOV    E,D", 0       }, // 0x5A
        { 0,    "MOV    E,E", 0       }, // 0x5B
        { 0,    "MOV    E,H", 0       }, // 0x5C
        { 0,    "MOV    E,L", 0       }, // 0x5D
        { 0,    "MOV    E,M", 0       }, // 0x5E
        { 0,    "MOV    E,A", 0       }, // 0x5F

        { 0,    "MOV    H,B", 0       }, // 0x60
        { 0,    "MOV    H,C", 0       }, // 0x61
        { 0,    "MOV    H,D", 0       }, // 0x62
        { 0,    "MOV    H,E", 0       }, // 0x63
        { 0,    "MOV    H,H", 0       }, // 0x64
        { 0,    "MOV    H,L", 0       }, // 0x65
        { 0,    "MOV    H,M", 0       }, // 0x66
        { 0,    "MOV    H,A", 0       }, // 0x67
        { 0,    "MOV    L,B", 0       }, // 0x68
        { 0,    "MOV    L,C", 0       }, // 0x69
        { 0,    "MOV    L,D", 0       }, // 0x6A
        { 0,    "MOV    L,E", 0       }, // 0x6B
        { 0,    "MOV    L,H", 0       }, // 0x6C
        { 0,    "MOV    L,L", 0       }, // 0x6D
        { 0,    "MOV    L,M", 0       }, // 0x6E
        { 0,    "MOV    L,A", 0       }, // 0x6F

        { 0,    "MOV    M,B", 0       }, // 0x70
        { 0,    "MOV    M,C", 0       }, // 0x71
        { 0,    "MOV    M,D", 0       }, // 0x72
        { 0,    "MOV    M,E", 0       }, // 0x73
        { 0,    "MOV    M,H", 0       }, // 0x74
        { 0,    "MOV    M,L", 0       }, // 0x75
        { 0,    "HLT",        0       }, // 0x76
        { 0,    "MOV    M,A", 0       }, // 0x77
        { 0,    "MOV    A,B", 0       }, // 0x78
        { 0,    "MOV    A,C", 0       }, // 0x79
        { 0,    "MOV    A,D", 0       }, // 0x7A
        { 0,    "MOV    A,E", 0       }, // 0x7B
        { 0,    "MOV    A,H", 0       }, // 0x7C
        { 0,    "MOV    A,L", 0       }, // 0x7D
        { 0,    "MOV    A,M", 0       }, // 0x7E
        { 0,    "MOV    A,A", 0       }, // 0x7F

        { 0,    "ADD    B",   0       }, // 0x80
        { 0,    "ADD    C",   0       }, // 0x81
        { 0,    "ADD    D",   0       }, // 0x82
        { 0,    "ADD    E",   0       }, // 0x83
        { 0,    "ADD    H",   0       }, // 0x84
        { 0,    "ADD    L",   0       }, // 0x85
        { 0,    "ADD    M",   0       }, // 0x86
        { 0,    "ADD    A",   0       }, // 0x87
        { 0,    "ADC    B",   0       }, // 0x88
        { 0,    "ADC    C",   0       }, // 0x89
        { 0,    "ADC    D",   0       }, // 0x8A
        { 0,    "ADC    E",   0       }, // 0x8B
        { 0,    "ADC    H",   0       }, // 0x8C
        { 0,    "ADC    L",   0       }, // 0x8D
        { 0,    "ADC    M",   0       }, // 0x8E
        { 0,    "ADC    A",   0       }, // 0x8F

        { 0,    "SUB    B",   0       }, // 0x90
        { 0,    "SUB    C",   0       }, // 0x91
        { 0,    "SUB    D",   0       }, // 0x92
        { 0,    "SUB    E",   0       }, // 0x93
        { 0,    "SUB    H",   0       }, // 0x94
        { 0,    "SUB    L",   0       }, // 0x95
        { 0,    "SUB    M",   0       }, // 0x96
        { 0,    "SUB    A",   0       }, // 0x97
        { 0,    "SBB    B",   0       }, // 0x98
        { 0,    "SBB    C",   0       }, // 0x99
        { 0,    "SBB    D",   0       }, // 0x9A
        { 0,    "SBB    E",   0       }, // 0x9B
        { 0,    "SBB    H",   0       }, // 0x9C
        { 0,    "SBB    L",   0       }, // 0x9D
        { 0,    "SBB    M",   0       }, // 0x9E
        { 0,    "SBB    A",   0       }, // 0x9F

        { 0,    "ANA    B",   0       }, // 0xA0
        { 0,    "ANA    C",   0       }, // 0xA1
        { 0,    "ANA    D",   0       }, // 0xA2
        { 0,    "ANA    E",   0       }, // 0xA3
        { 0,    "ANA    H",   0       }, // 0xA4
        { 0,    "ANA    L",   0       }, // 0xA5
        { 0,    "ANA    M",   0       }, // 0xA6
        { 0,    "ANA    A",   0       }, // 0xA7
        { 0,    "XRA    B",   0       }, // 0xA8
        { 0,    "XRA    C",   0       }, // 0xA9
        { 0,    "XRA    D",   0       }, // 0xAA
        { 0,    "XRA    E",   0       }, // 0xAB
        { 0,    "XRA    H",   0       }, // 0xAC
        { 0,    "XRA    L",   0       }, // 0xAD
        { 0,    "XRA    M",   0       }, // 0xAE
        { 0,    "XRA    A",   0       }, // 0xAF

        { 0,    "ORA    B",   0       }, // 0xB0
        { 0,    "ORA    C",   0       }, // 0xB1
        { 0,    "ORA    D",   0       }, // 0xB2
        { 0,    "ORA    E",   0       }, // 0xB3
        { 0,    "ORA    H",   0       }, // 0xB4
        { 0,    "ORA    L",   0       }, // 0xB5
        { 0,    "ORA    M",   0       }, // 0xB6
        { 0,    "ORA    A",   0       }, // 0xB7
        { 0,    "CMP    B",   0       }, // 0xB8
        { 0,    "CMP    C",   0       }, // 0xB9
        { 0,    "CMP    D",   0       }, // 0xBA
        { 0,    "CMP    E",   0       }, // 0xBB
        { 0,    "CMP    H",   0       }, // 0xBC
        { 0,    "CMP    L",   0       }, // 0xBD
        { 0,    "CMP    M",   0       }, // 0xBE
        { 0,    "CMP    A",   0       }, // 0xBF

        { 0,    "RNZ",        0       }, // 0xC0
        { 0,    "POP    B",   0       }, // 0xC1
        { WORD, "JNZ    ",    ADDRESS }, // 0xC2
        { WORD, "JMP    ",    ADDRESS }, // 0xC3
        { WORD, "CNZ    ",    ADDRESS }, // 0xC4
        { 0,    "PUSH   B",   0       }, // 0xc5
        { BYTE, "ADI    ",    0       }, // 0xC6
        { 0,    "RST    0",   0       }, // 0xC7
        { 0,    "RZ",         0       }, // 0xC8
        { 0,    "RET",        0       }, // 0xC9
        { WORD, "JZ     ",    ADDRESS }, // 0xCA
        { WORD, "JMP    ",    ADDRESS }, // 0xCB
        { WORD, "CZ     ",    ADDRESS }, // 0xCC
        { WORD, "CALL   ",    ADDRESS }, // 0xCD
        { BYTE, "ACI    ",    0       }, // 0xCE
        { 0,    "RST    1",   0       }, // 0xCF

        { 0,    "RNC",        0       }, // 0xD0
        { 0,    "POP    D",   0       }, // 0xD1
        { WORD, "JNC    ",    ADDRESS }, // 0xD2
        { BYTE, "OUT    ",    0       }, // 0xD3
        { WORD, "CNC    ",    ADDRESS }, // 0xD4
        { 0,    "PUSH   D",   0       }, // 0xD5
        { BYTE, "SUI    ",    0       }, // 0xD6
        { 0,    "RST    2",   0       }, // 0xD7
        { 0,    "RC",         0       }, // 0xD8
        { 0,    "RET",        0       }, // 0xD9
        { WORD, "JC     ",    ADDRESS }, // 0xDA
        { BYTE, "IN     ",    0       }, // 0xDB
        { WORD, "CC     ",    ADDRESS }, // 0xDC
        { WORD, "CALL   ",    ADDRESS }, // 0xDD
        { BYTE, "SBI    ",    0       }, // 0xDE
        { 0,    "RST    3",   0       }, // 0xDF

        { 0,    "RPO",        0       }, // 0xE0
        { 0,    "POP    H",   0       }, // 0xE1
        { WORD, "JPO    ",    ADDRESS }, // 0xE2
        { 0,    "XTHL",       0       }, // 0xE3
        { WORD, "CPO    ",    ADDRESS }, // 0xE4
        { 0,    "PUSH   H",   0       }, // 0xE5
        { BYTE, "ANI    ",    0       }, // 0xE6
        { 0,    "RST    4",   0       }, // 0xE7
        { 0,    "RPE",        0       }, // 0xE8
        { 0,    "PCHL",       0       }, // 0xE9
        { WORD, "JPE    ",    ADDRESS }, // 0xEA
        { 0,    "XCHG",       0       }, // 0xEB
        { WORD, "CPE    ",    ADDRESS }, // 0xEC
        { WORD, "CALL   ",    ADDRESS }, // 0xED
        { BYTE, "XRI    ",    0       }, // 0xEE
        { 0,    "RST    5",   0       }, // 0xEF

        { 0,    "RP",         0       }, // 0xF0
        { 0,    "POP    PSW", 0       }, // 0xF1
        { WORD, "JP     ",    ADDRESS }, // 0xF2
        { 0,    "DI",         0       }, // 0xF3
        { WORD, "CP     ",    ADDRESS }, // 0xF4
        { 0,    "PUSH   PSW", 0       }, // 0xF5
        { BYTE, "ORI    ",    0       }, // 0xF6
        { 0,    "RST    6",   0       }, // 0xF7
        { 0,    "RM",         0       }, // 0xF8
        { 0,    "SPHL",       0       }, // 0xF9
        { WORD, "JM     ",    ADDRESS }, // 0xFA
        { 0,    "EI",         0       }, // 0xFB
        { WORD, "CM     ",    ADDRESS }, // 0xFC
        { WORD, "CALL   ",    ADDRESS }, // 0xFD
        { BYTE, "CPI    ",    0       }, // 0xFE
        { 0,    "RST    7",   0       }, // 0xFF
    };

    unsigned char * opcode = &buf[program_counter];
    int offset = 1;

    switch (instructions[*opcode].size)
    {
        case BYTE:
            sprintf(disassembled, "%s#$%02X", instructions[*opcode].mnemonic, opcode[1]);
            break;
        case WORD:
            if (instructions[*opcode].has_address)
            {
                sprintf(disassembled, "%s$%02X%02X", instructions[*opcode].mnemonic, opcode[2], opcode[1]);
            }
            else
            {
                sprintf(disassembled, "%s#$%02X%02X", instructions[*opcode].mnemonic, opcode[2], opcode[1]);
            }
            break;
        default:
            sprintf(disassembled, "%s", instructions[*opcode].mnemonic);
            break;
    }

    offset += instructions[*opcode].size;

    return offset;
}
