#include <stdio.h>
#include "disassembler.h"

int
disassemble(unsigned char * buf, char * disassembled, int program_counter)
{
    enum { BYTE = 1, WORD = 2, HAS_ADDRESS = 1 };

    // https://pastraiser.com/cpu/i8080/i8080_opcodes.html
    static instruction_t instructions[] = {
        { 0,    "NOP",       0           }, // 0x00
        { WORD, "LXI\tB",    0           }, // 0x01
        { 0,    "STAX\tB",   0           }, // 0x02
        { 0,    "INX\tB",    0           }, // 0x03
        { 0,    "INR\tB",    0           }, // 0x04
        { 0,    "DCR\tB",    0           }, // 0x05
        { BYTE, "MVI\tB",    0           }, // 0x06
        { 0,    "RLC",       0           }, // 0x07
        { 0,    "NOP",       0           }, // 0x08
        { 0,    "DAD\tB",    0           }, // 0x09
        { 0,    "LDAX\tB",   0           }, // 0x0A
        { 0,    "DCX\tB",    0           }, // 0x0B
        { 0,    "INR\tC",    0           }, // 0x0C
        { 0,    "DCR\tC",    0           }, // 0x0D
        { BYTE, "MVI\tC",    0           }, // 0x0E
        { 0,    "RRC",       0           }, // 0x0F

        { 0,    "NOP",       0           }, // 0x10
        { WORD, "LXI\tD",    0           }, // 0x11
        { 0,    "STAX\tD",   0           }, // 0x12
        { 0,    "INX\tD",    0           }, // 0x13
        { 0,    "INR\tD",    0           }, // 0x14
        { 0,    "DCR\tD",    0           }, // 0x15
        { BYTE, "MVI\tD",    0           }, // 0x16
        { 0,    "RAL",       0           }, // 0x17
        { 0,    "NOP",       0           }, // 0x18
        { 0,    "DAD\tD",    0           }, // 0x19
        { 0,    "LDAX\tD",   0           }, // 0x1A
        { 0,    "DCX\tD",    0           }, // 0x1B
        { 0,    "INR\tE",    0           }, // 0x1C
        { 0,    "DCR\tE",    0           }, // 0x1D
        { BYTE, "MVI\tE",    0           }, // 0x1E
        { 0,    "RAR",       0           }, // 0x1F

        { 0,    "NOP",       0           }, // 0x20
        { WORD, "LXI\tH",    0           }, // 0x21
        { WORD, "SHLD\t",    HAS_ADDRESS }, // 0x22
        { 0,    "INX\tH",    0           }, // 0x23
        { 0,    "INR\tH",    0           }, // 0x24
        { 0,    "DCR\tH",    0           }, // 0x25
        { BYTE, "MVI\tH",    0           }, // 0x26
        { 0,    "DAA",       0           }, // 0x27
        { 0,    "NOP",       0           }, // 0x28
        { 0,    "DAD\tH",    0           }, // 0x29
        { WORD, "LHLD\t",    HAS_ADDRESS }, // 0x2A
        { 0,    "DCX\tH",    0           }, // 0x2B
        { 0,    "INR\tL",    0           }, // 0x2C
        { 0,    "DCR\tL",    0           }, // 0x2D
        { BYTE, "MVI\tL",    0           }, // 0x2E
        { 0,    "CMA",       0           }, // 0x2F

        { 0,    "NOP",       0           }, // 0x30
        { WORD, "LXI\tSP",   0           }, // 0x31
        { WORD, "STA\t",     HAS_ADDRESS }, // 0x32
        { 0,    "INX\tSP",   0           }, // 0x33
        { 0,    "INR\tM",    0           }, // 0x34
        { 0,    "DCR\tM",    0           }, // 0x35
        { BYTE, "MVI\tM",    0           }, // 0x36
        { 0,    "STC",       0           }, // 0x37
        { 0,    "NOP",       0           }, // 0x38
        { 0,    "DAD\tSP",   0           }, // 0x39
        { WORD, "LDA\t",     HAS_ADDRESS }, // 0x3A
        { 0,    "DCX\tSP",   0           }, // 0x3B
        { 0,    "INR\tA",    0           }, // 0x3C
        { 0,    "DCR\tA",    0           }, // 0x3D
        { BYTE, "MVI\tA",    0           }, // 0x3E
        { 0,    "CMC",       0           }, // 0x3F

        { 0,    "MOV\tB,B",  0           }, // 0x40
        { 0,    "MOV\tB,C",  0           }, // 0x41
        { 0,    "MOV\tB,D",  0           }, // 0x42
        { 0,    "MOV\tB,E",  0           }, // 0x43
        { 0,    "MOV\tB,H",  0           }, // 0x44
        { 0,    "MOV\tB,L",  0           }, // 0x45
        { 0,    "MOV\tB,M",  0           }, // 0x46
        { 0,    "MOV\tB,A",  0           }, // 0x47
        { 0,    "MOV\tC,B",  0           }, // 0x48
        { 0,    "MOV\tC,C",  0           }, // 0x49
        { 0,    "MOV\tC,D",  0           }, // 0x4A
        { 0,    "MOV\tC,E",  0           }, // 0x4B
        { 0,    "MOV\tC,H",  0           }, // 0x4C
        { 0,    "MOV\tC,L",  0           }, // 0x4D
        { 0,    "MOV\tC,M",  0           }, // 0x4E
        { 0,    "MOV\tC,A",  0           }, // 0x4F

        { 0,    "MOV\tD,B",  0           }, // 0x50
        { 0,    "MOV\tD,C",  0           }, // 0x51
        { 0,    "MOV\tD,D",  0           }, // 0x52
        { 0,    "MOV\tD,E",  0           }, // 0x53
        { 0,    "MOV\tD,H",  0           }, // 0x54
        { 0,    "MOV\tD,L",  0           }, // 0x55
        { 0,    "MOV\tD,M",  0           }, // 0x56
        { 0,    "MOV\tD,A",  0           }, // 0x57
        { 0,    "MOV\tE,B",  0           }, // 0x58
        { 0,    "MOV\tE,C",  0           }, // 0x59
        { 0,    "MOV\tE,D",  0           }, // 0x5A
        { 0,    "MOV\tE,E",  0           }, // 0x5B
        { 0,    "MOV\tE,H",  0           }, // 0x5C
        { 0,    "MOV\tE,L",  0           }, // 0x5D
        { 0,    "MOV\tE,M",  0           }, // 0x5E
        { 0,    "MOV\tE,A",  0           }, // 0x5F

        { 0,    "MOV\tH,B",  0           }, // 0x60
        { 0,    "MOV\tH,C",  0           }, // 0x61
        { 0,    "MOV\tH,D",  0           }, // 0x62
        { 0,    "MOV\tH,E",  0           }, // 0x63
        { 0,    "MOV\tH,H",  0           }, // 0x64
        { 0,    "MOV\tH,L",  0           }, // 0x65
        { 0,    "MOV\tH,M",  0           }, // 0x66
        { 0,    "MOV\tH,A",  0           }, // 0x67
        { 0,    "MOV\tL,B",  0           }, // 0x68
        { 0,    "MOV\tL,C",  0           }, // 0x69
        { 0,    "MOV\tL,D",  0           }, // 0x6A
        { 0,    "MOV\tL,E",  0           }, // 0x6B
        { 0,    "MOV\tL,H",  0           }, // 0x6C
        { 0,    "MOV\tL,L",  0           }, // 0x6D
        { 0,    "MOV\tL,M",  0           }, // 0x6E
        { 0,    "MOV\tL,A",  0           }, // 0x6F

        { 0,    "MOV\tM,B",  0           }, // 0x70
        { 0,    "MOV\tM,C",  0           }, // 0x71
        { 0,    "MOV\tM,D",  0           }, // 0x72
        { 0,    "MOV\tM,E",  0           }, // 0x73
        { 0,    "MOV\tM,H",  0           }, // 0x74
        { 0,    "MOV\tM,L",  0           }, // 0x75
        { 0,    "HLT",       0           }, // 0x76
        { 0,    "MOV\tM,A",  0           }, // 0x77
        { 0,    "MOV\tA,B",  0           }, // 0x78
        { 0,    "MOV\tA,C",  0           }, // 0x79
        { 0,    "MOV\tA,D",  0           }, // 0x7A
        { 0,    "MOV\tA,E",  0           }, // 0x7B
        { 0,    "MOV\tA,H",  0           }, // 0x7C
        { 0,    "MOV\tA,L",  0           }, // 0x7D
        { 0,    "MOV\tA,M",  0           }, // 0x7E
        { 0,    "MOV\tA,A",  0           }, // 0x7F

        { 0,    "ADD\tB",    0           }, // 0x80
        { 0,    "ADD\tC",    0           }, // 0x81
        { 0,    "ADD\tD",    0           }, // 0x82
        { 0,    "ADD\tE",    0           }, // 0x83
        { 0,    "ADD\tH",    0           }, // 0x84
        { 0,    "ADD\tL",    0           }, // 0x85
        { 0,    "ADD\tM",    0           }, // 0x86
        { 0,    "ADD\tA",    0           }, // 0x87
        { 0,    "ADC\tB",    0           }, // 0x88
        { 0,    "ADC\tC",    0           }, // 0x89
        { 0,    "ADC\tD",    0           }, // 0x8A
        { 0,    "ADC\tE",    0           }, // 0x8B
        { 0,    "ADC\tH",    0           }, // 0x8C
        { 0,    "ADC\tL",    0           }, // 0x8D
        { 0,    "ADC\tM",    0           }, // 0x8E
        { 0,    "ADC\tA",    0           }, // 0x8F

        { 0,    "SUB\tB",    0           }, // 0x90
        { 0,    "SUB\tC",    0           }, // 0x91
        { 0,    "SUB\tD",    0           }, // 0x92
        { 0,    "SUB\tE",    0           }, // 0x93
        { 0,    "SUB\tH",    0           }, // 0x94
        { 0,    "SUB\tL",    0           }, // 0x95
        { 0,    "SUB\tM",    0           }, // 0x96
        { 0,    "SUB\tA",    0           }, // 0x97
        { 0,    "SBB\tB",    0           }, // 0x98
        { 0,    "SBB\tC",    0           }, // 0x99
        { 0,    "SBB\tD",    0           }, // 0x9A
        { 0,    "SBB\tE",    0           }, // 0x9B
        { 0,    "SBB\tH",    0           }, // 0x9C
        { 0,    "SBB\tL",    0           }, // 0x9D
        { 0,    "SBB\tM",    0           }, // 0x9E
        { 0,    "SBB\tA",    0           }, // 0x9F

        { 0,    "ANA\tB",    0           }, // 0xA0
        { 0,    "ANA\tC",    0           }, // 0xA1
        { 0,    "ANA\tD",    0           }, // 0xA2
        { 0,    "ANA\tE",    0           }, // 0xA3
        { 0,    "ANA\tH",    0           }, // 0xA4
        { 0,    "ANA\tL",    0           }, // 0xA5
        { 0,    "ANA\tM",    0           }, // 0xA6
        { 0,    "ANA\tA",    0           }, // 0xA7
        { 0,    "XRA\tB",    0           }, // 0xA8
        { 0,    "XRA\tC",    0           }, // 0xA9
        { 0,    "XRA\tD",    0           }, // 0xAA
        { 0,    "XRA\tE",    0           }, // 0xAB
        { 0,    "XRA\tH",    0           }, // 0xAC
        { 0,    "XRA\tL",    0           }, // 0xAD
        { 0,    "XRA\tM",    0           }, // 0xAE
        { 0,    "XRA\tA",    0           }, // 0xAF

        { 0,    "ORA\tB",    0           }, // 0xB0
        { 0,    "ORA\tC",    0           }, // 0xB1
        { 0,    "ORA\tD",    0           }, // 0xB2
        { 0,    "ORA\tE",    0           }, // 0xB3
        { 0,    "ORA\tH",    0           }, // 0xB4
        { 0,    "ORA\tL",    0           }, // 0xB5
        { 0,    "ORA\tM",    0           }, // 0xB6
        { 0,    "ORA\tA",    0           }, // 0xB7
        { 0,    "CMP\tB",    0           }, // 0xB8
        { 0,    "CMP\tC",    0           }, // 0xB9
        { 0,    "CMP\tD",    0           }, // 0xBA
        { 0,    "CMP\tE",    0           }, // 0xBB
        { 0,    "CMP\tH",    0           }, // 0xBC
        { 0,    "CMP\tL",    0           }, // 0xBD
        { 0,    "CMP\tM",    0           }, // 0xBE
        { 0,    "CMP\tA",    0           }, // 0xBF

        { 0,    "RNZ",       0           }, // 0xC0
        { 0,    "POP\tB",    0           }, // 0xC1
        { WORD, "JNZ\t",     HAS_ADDRESS }, // 0xC2
        { WORD, "JMP\t",     HAS_ADDRESS }, // 0xC3
        { WORD, "CNZ\t",     HAS_ADDRESS }, // 0xC4
        { 0,    "PUSH\tB",   0           }, // 0xc5
        { BYTE, "ADI\t",     0           }, // 0xC6
        { 0,    "RST\t0",    0           }, // 0xC7
        { 0,    "RZ",        0           }, // 0xC8
        { 0,    "RET",       0           }, // 0xC9
        { WORD, "JZ \t",     HAS_ADDRESS }, // 0xCA
        { WORD, "JMP\t",     HAS_ADDRESS }, // 0xCB
        { WORD, "CZ \t",     HAS_ADDRESS }, // 0xCC
        { WORD, "CALL\t",    HAS_ADDRESS }, // 0xCD
        { BYTE, "ACI\t",     0           }, // 0xCE
        { 0,    "RST\t1",    0           }, // 0xCF

        { 0,    "RNC",       0           }, // 0xD0
        { 0,    "POP\tD",    0           }, // 0xD1
        { WORD, "JNC\t",     HAS_ADDRESS }, // 0xD2
        { BYTE, "OUT\t",     0           }, // 0xD3
        { WORD, "CNC\t",     HAS_ADDRESS }, // 0xD4
        { 0,    "PUSH\tD",   0           }, // 0xD5
        { BYTE, "SUI\t",     0           }, // 0xD6
        { 0,    "RST\t2",    0           }, // 0xD7
        { 0,    "RC",        0           }, // 0xD8
        { 0,    "RET",       0           }, // 0xD9
        { WORD, "JC \t",     HAS_ADDRESS }, // 0xDA
        { BYTE, "IN \t",     0           }, // 0xDB
        { WORD, "CC \t",     HAS_ADDRESS }, // 0xDC
        { WORD, "CALL\t",    HAS_ADDRESS }, // 0xDD
        { BYTE, "SBI\t",     0           }, // 0xDE
        { 0,    "RST\t3",    0           }, // 0xDF

        { 0,    "RPO",       0           }, // 0xE0
        { 0,    "POP\tH",    0           }, // 0xE1
        { WORD, "JPO\t",     HAS_ADDRESS }, // 0xE2
        { 0,    "XTHL",      0           }, // 0xE3
        { WORD, "CPO\t",     HAS_ADDRESS }, // 0xE4
        { 0,    "PUSH\tH",   0           }, // 0xE5
        { BYTE, "ANI\t",     0           }, // 0xE6
        { 0,    "RST\t4",    0           }, // 0xE7
        { 0,    "RPE",       0           }, // 0xE8
        { 0,    "PCHL",      0           }, // 0xE9
        { WORD, "JPE\t",     HAS_ADDRESS }, // 0xEA
        { 0,    "XCHG",      0           }, // 0xEB
        { WORD, "CPE\t",     HAS_ADDRESS }, // 0xEC
        { WORD, "CALL\t",    HAS_ADDRESS }, // 0xED
        { BYTE, "XRI\t",     0           }, // 0xEE
        { 0,    "RST\t5",    0           }, // 0xEF

        { 0,    "RP",        0           }, // 0xF0
        { 0,    "POP\tPSW",  0           }, // 0xF1
        { WORD, "JP \t",     HAS_ADDRESS }, // 0xF2
        { 0,    "DI",        0           }, // 0xF3
        { WORD, "CP \t",     HAS_ADDRESS }, // 0xF4
        { 0,    "PUSH\tPSW", 0           }, // 0xF5
        { BYTE, "ORI\t",     0           }, // 0xF6
        { 0,    "RST\t6",    0           }, // 0xF7
        { 0,    "RM",        0           }, // 0xF8
        { 0,    "SPHL",      0           }, // 0xF9
        { WORD, "JM \t",     HAS_ADDRESS }, // 0xFA
        { 0,    "EI",        0           }, // 0xFB
        { WORD, "CM \t",     HAS_ADDRESS }, // 0xFC
        { WORD, "CALL\t",    HAS_ADDRESS }, // 0xFD
        { BYTE, "CPI\t",     0           }, // 0xFE
        { 0,    "RST\t7",    0           }, // 0xFF
    };

    unsigned char * opcode = &buf[program_counter];
    int offset = 1;

    switch (instructions[*opcode].size)
    {
        case BYTE:
            sprintf(disassembled, "%s,#$%02X", instructions[*opcode].mnemonic, opcode[1]);
            break;
        case WORD:
            sprintf(disassembled, "%s%s%02X%02X", instructions[*opcode].mnemonic, (instructions[*opcode].has_address ? "$" : ",#$"), opcode[2], opcode[1]);
            break;
        default:
            sprintf(disassembled, "%s", instructions[*opcode].mnemonic);
            break;
    }

    offset += instructions[*opcode].size;

    return offset;
}
