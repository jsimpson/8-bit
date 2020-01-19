#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "emulator.h"

void
die(cpu_8080_t * cpu)
{
    // The program counter advances ahead of any errors.
    cpu->program_counter -= 1;

    printf("Instructon 0x%02x\n", cpu->memory[cpu->program_counter]);
    printf("PC         0x%04x\n", cpu->program_counter);
    printf("SP         0x%04x\n", cpu->stack_pointer);
    printf("IE         0x%02x\n", cpu->interrupt_enabled);
    printf("A          0x%02x\n", cpu->a);
    printf("B          0x%02x\n", cpu->b);
    printf("C          0x%02x\n", cpu->c);
    printf("D          0x%02x\n", cpu->d);
    printf("E          0x%02x\n", cpu->e);
    printf("H          0x%02x\n", cpu->h);
    printf("L          0x%02x\n", cpu->l);
    printf("sign       0x%02x\n", cpu->condition_codes.s);
    printf("zero       0x%02x\n", cpu->condition_codes.z);
    printf("aux carry  0x%02x\n", cpu->condition_codes.ac);
    printf("parity     0x%02x\n", cpu->condition_codes.p);
    printf("carry      0x%02x\n", cpu->condition_codes.cy);

    free(cpu->memory);
    free(cpu);

    exit(1);
}

/*
 * The parity bit is set when the number of bits set in a byte are even and
 * reset when they are odd.
 */
static int
parity(int byte)
{
    int parity = 0;
    byte = (byte & (1 << 7));

    for (int i = 0; i < 8; i++)
    {
        if (byte & 0x1)
        {
            parity++;
        }

        byte >>= 1;
    }

    return (0 == (parity & 0x1));
}

/*
 * Single register instructions.
 */

void
process_condition_bits(cpu_8080_t * cpu, uint16_t value, uint8_t bits)
{
    if (bits & ZERO_BIT) {
        cpu->condition_codes.z = value == 0;
    }

    if (bits & SIGN_BIT) {
        cpu->condition_codes.s = (value & 0x80) == 0x80;
    }

    if (bits & AUX_CARRY_BIT) {
        cpu->condition_codes.ac = (value & 0xF) == 0xF;
    }

    if (bits & PARITY_BIT) {
        cpu->condition_codes.p = parity(value);
    }

    if (bits & CARRY_BIT) {
        cpu->condition_codes.cy = (value & 0x80) == 0x80;
    }
}

// Increment register or memory.
static int
inr(cpu_8080_t * cpu, uint8_t reg)
{
    int cycles = 5;
    uint8_t condition_bits = ZERO_BIT | SIGN_BIT | PARITY_BIT | AUX_CARRY_BIT;

    switch(reg)
    {
        case B:
            cpu->b = cpu->b + 1;
            process_condition_bits(cpu, cpu->b, condition_bits);
            break;
        case C:
            cpu->c = cpu->c + 1;
            process_condition_bits(cpu, cpu->c, condition_bits);
            break;
        case D:
            cpu->d = cpu->d + 1;
            process_condition_bits(cpu, cpu->d, condition_bits);
            break;
        case E:
            cpu->e = cpu->e + 1;
            process_condition_bits(cpu, cpu->e, condition_bits);
            break;
        case H:
            cpu->h = cpu->h + 1;
            process_condition_bits(cpu, cpu->h, condition_bits);
            break;
        case L:
            cpu->l = cpu->l + 1;
            process_condition_bits(cpu, cpu->l, condition_bits);
            break;
        case M:
            {
                uint16_t address = (cpu->h << 8) | cpu->l;
                cpu->memory[address] = cpu->memory[address] + 1;
                process_condition_bits(cpu, cpu->memory[address], condition_bits);
            }

            cycles = 10;
            break;
        case A:
            cpu->a = cpu->l + 1;
            process_condition_bits(cpu, cpu->a, condition_bits);
            break;
        default:
            die(cpu);
            break;
    }

    return cycles;
}

/*
 * Data transfer instructions.
 * Instructions that transfer data between registers or between memory and
 * registers.
 */

// Store Accumulator.
static int
stax(cpu_8080_t * cpu, uint8_t rp)
{
    switch(rp)
    {
        case B:
            {
                uint16_t addr = (cpu->b << 8) | cpu->c;
                cpu->memory[addr] = cpu->a;
            }
            break;
        case D:
            {
                uint16_t addr = (cpu->d << 8) | cpu->e;
                cpu->memory[addr] = cpu->a;
            }
            break;
        default:
            die(cpu);
    }

    return 7;
}

/*
 * Register pair instructions.
 * Instructions which operate on a pair of registers.
 */

// Increment register pair.
static int
inx(cpu_8080_t * cpu, uint8_t rp)
{
    switch(rp)
    {
        case BC:
            {
                uint16_t value = (cpu->b << 8) | cpu->c;
                value += 1;
                cpu->b = (value & 0xFF00) >> 8;
                cpu->c = value & 0xFF;
            }
            break;
        case DE:
            {
                uint16_t value = (cpu->d << 8) | cpu->e;
                value += 1;
                cpu->d = (value & 0xFF00) >> 8;
                cpu->e = value & 0xFF;
            }
            break;
        case HL:
            {
                uint16_t value = (cpu->h << 8) | cpu->l;
                value += 1;
                cpu->h = (value & 0xFF00) >> 8;
                cpu->l = value & 0xFF;
            }
            break;
        case SP:
            {
                uint16_t value = cpu->stack_pointer;
                value += 1;
                cpu->stack_pointer = value & 0xFFFF;
            }
            break;
        default:
            die(cpu);
            break;
    }

    return 5;
}

/*
 * Immediate instructions.
 * Instructions that perform operations on byte(s) which are part of the
 * instruction itself.
 */

// Load a register pair immediately.
static int
lxi(cpu_8080_t * cpu, uint8_t reg, uint8_t * opcode)
{
    switch(reg)
    {
        case B:
            cpu->c = opcode[1];
            cpu->b = opcode[2];
            break;
        case D:
            cpu->e = opcode[1];
            cpu->d = opcode[2];
            break;
        case H:
            cpu->l = opcode[1];
            cpu->h = opcode[2];
            break;
        case SP:
            cpu->stack_pointer = ((opcode[2] << 8) | opcode[1]) & 0xFFFF;
            break;
        default:
            die(cpu);
            break;
    }

    cpu->program_counter += 2;

    return 10;
}

int
process_instruction(cpu_8080_t * cpu)
{
    int cycles = 0;
    uint8_t * opcode = &cpu->memory[cpu->program_counter];

    cpu->program_counter++;

    switch(*opcode)
    {
        case 0x00:
        case 0x08:
        case 0x10:
        case 0x18:
        case 0x20:
        case 0x28:
        case 0x30:
        case 0x38:
            return 4;
            break;
        case 0x01: // LXI B, D16
            cycles = lxi(cpu, B, opcode);
            break;
        case 0x02:
            cycles = stax(cpu, B);
            break;
        case 0x03:
            cycles = inx(cpu, B);
            break;
        case 0x04:
            cycles = inr(cpu, B);
            break;
        case 0x05:
            cpu->b = cpu->b - 1;
            cpu->condition_codes.z = ((cpu->b) == 0);
            cpu->condition_codes.s = ((cpu->b & 0x80) == 0x80);
            cpu->condition_codes.p = parity(cpu->b);
            cpu->condition_codes.ac = ((cpu->b & 0xf) == 0x0);
            break;
        case 0x06:
            cpu->b = opcode[1];
            cpu->program_counter++;
            break;
        case 0x07:
            {
                unsigned char x = cpu->a;
                cpu-> a = ((x << 1) | (x & 0x80) >> 7);
                cpu->condition_codes.cy = ((x & 0x80) == 0x80);
            }
            break;
        case 0x09:
            {
                unsigned int register_pair_HL = (cpu->h << 8) | (cpu->l);
                unsigned int register_pair_BC = (cpu->b << 8) | (cpu->c);
                register_pair_HL += register_pair_BC;
                cpu->condition_codes.cy = (register_pair_HL > 0xff);
                cpu->h = (register_pair_HL & 0xff00) >> 8;
                cpu->l = register_pair_HL & 0xff;
            }
            break;
        case 0x0A:
            {
                unsigned short int register_pair = (cpu->b << 8) | (cpu->c);
                cpu->a = cpu->memory[register_pair];
            }
            break;
        case 0x0B:
            {
                unsigned short int register_pair = (cpu->b << 8) | (cpu->c);
                register_pair -= - 1;
                cpu->b = (register_pair & 0xff00) >> 8;
                cpu->c = register_pair & 0xff;
            }
            break;
        case 0x0C:
            cycles = inr(cpu, C);
            break;
        case 0x0D:
            cpu->c = cpu->c - 1;
            cpu->condition_codes.z = ((cpu->c) == 0);
            cpu->condition_codes.s = ((cpu->c & 0x80) == 0x80);
            cpu->condition_codes.p = parity(cpu->c);
            cpu->condition_codes.ac = ((cpu->c & 0xf) == 0x0);
            break;
        case 0x0E:
            cpu->c = opcode[1];
            cpu->program_counter++;
            break;
        case 0x0F:
            {
                unsigned char x = cpu->a;
                cpu-> a = ((x & 1) << 7 | (x >> 1));
                cpu->condition_codes.cy = ((x & 1) == 1);
            }
            break;

        case 0x11:
            cycles = lxi(cpu, D, opcode);
            break;
        case 0x12:
            cycles = stax(cpu, D);
            break;
        case 0x13:
            {
                unsigned short int register_pair = (cpu->d << 8) | (cpu->e);
                register_pair += 1;
                cpu->d = (register_pair & 0xff00) >> 8;
                cpu->e = register_pair & 0xff;
            }
            break;
        case 0x14:
            cycles = inr(cpu, D);
            break;
        case 0x15:
            cpu->d = cpu->d - 1;
            cpu->condition_codes.z = ((cpu->d) == 0);
            cpu->condition_codes.s = ((cpu->d & 0x80) == 0x80);
            cpu->condition_codes.p = parity(cpu->d);
            cpu->condition_codes.ac = ((cpu->d & 0xf) == 0x0);
            break;
        case 0x16:
            cpu->d = opcode[1];
            cpu->program_counter++;
            break;
        case 0x17:
            {
                unsigned char x = cpu->a;
                cpu-> a = ((x << 1) | cpu->condition_codes.cy);
                cpu->condition_codes.cy = ((x & 0x80) == 0x80);
            }
            break;
        case 0x19:
            {
                unsigned int register_pair_HL = (cpu->h << 8) | (cpu->l);
                unsigned int register_pair_DE = (cpu->d << 8) | (cpu->e);
                register_pair_HL += register_pair_DE;
                cpu->condition_codes.cy = (register_pair_HL > 0xff);
                cpu->h = (register_pair_HL & 0xff00) >> 8;
                cpu->l = register_pair_HL & 0xff;
            }
            break;
        case 0x1A:
            {
                unsigned short int register_pair = (cpu->d << 8) | (cpu->e);
                cpu->a = cpu->memory[register_pair];
            }
            break;
        case 0x1B:
            {
                unsigned short int register_pair = (cpu->d << 8) | (cpu->e);
                register_pair -= 1;
                cpu->b = (register_pair & 0xff00) >> 8;
                cpu->e = register_pair & 0xff;
            }
            break;
        case 0x1C:
            cycles = inr(cpu, E);
            break;
        case 0x1D:
            cpu->e = cpu->e - 1;
            cpu->condition_codes.z = ((cpu->e) == 0);
            cpu->condition_codes.s = ((cpu->e & 0x80) == 0x80);
            cpu->condition_codes.p = parity(cpu->e);
            cpu->condition_codes.ac = ((cpu->e & 0xf) == 0x0);
            break;
        case 0x1E:
            cpu->e = opcode[1];
            cpu->program_counter++;
            break;
        case 0x1F:
            {
                unsigned char x = cpu->a;
                cpu-> a = ((cpu->condition_codes.cy << 7) | (x >> 1));
                cpu->condition_codes.cy = ((x & 1) == 1);
            }
            break;

        case 0x21:
            cycles = lxi(cpu, H, opcode);
            break;
        case 0x22: die(cpu); break;
        case 0x23: die(cpu); break;
        case 0x24:
            cycles = inr(cpu, H);
            break;
        case 0x25: die(cpu); break;
        case 0x26: die(cpu); break;
        case 0x27: die(cpu); break;
        case 0x29: die(cpu); break;
        case 0x2A: die(cpu); break;
        case 0x2B: die(cpu); break;
        case 0x2C:
            cycles = inr(cpu, L);
            break;
        case 0x2D: die(cpu); break;
        case 0x2E: die(cpu); break;
        case 0x2F: die(cpu); break;

        case 0x31:
            cycles = lxi(cpu, SP, opcode);
            break;
        case 0x32: die(cpu); break;
        case 0x33: die(cpu); break;
        case 0x34:
            cycles = inr(cpu, M);
            break;
        case 0x35: die(cpu); break;
        case 0x36: die(cpu); break;
        case 0x37: die(cpu); break;
        case 0x39: die(cpu); break;
        case 0x3A: die(cpu); break;
        case 0x3B: die(cpu); break;
        case 0x3C:
            cycles = inr(cpu, A);
            break;
        case 0x3D: die(cpu); break;
        case 0x3E: die(cpu); break;
        case 0x3F: die(cpu); break;

        case 0x40:
            cpu->b = cpu->c;
            break;
        case 0x41:
            cpu->b = cpu->d;
            break;
        case 0x42:
            cpu->b = cpu->e;
            break;
        case 0x43: die(cpu); break;
        case 0x44: die(cpu); break;
        case 0x45: die(cpu); break;
        case 0x46: die(cpu); break;
        case 0x47: die(cpu); break;
        case 0x48: die(cpu); break;
        case 0x49: die(cpu); break;
        case 0x4A: die(cpu); break;
        case 0x4B: die(cpu); break;
        case 0x4C: die(cpu); break;
        case 0x4D: die(cpu); break;
        case 0x4E: die(cpu); break;
        case 0x4F: die(cpu); break;

        case 0x50: die(cpu); break;
        case 0x51: die(cpu); break;
        case 0x52: die(cpu); break;
        case 0x53: die(cpu); break;
        case 0x54: die(cpu); break;
        case 0x55: die(cpu); break;
        case 0x56: die(cpu); break;
        case 0x57: die(cpu); break;
        case 0x58: die(cpu); break;
        case 0x59: die(cpu); break;
        case 0x5A: die(cpu); break;
        case 0x5B: die(cpu); break;
        case 0x5C: die(cpu); break;
        case 0x5D: die(cpu); break;
        case 0x5E: die(cpu); break;
        case 0x5F: die(cpu); break;

        case 0x60: die(cpu); break;
        case 0x61: die(cpu); break;
        case 0x62: die(cpu); break;
        case 0x63: die(cpu); break;
        case 0x64: die(cpu); break;
        case 0x65: die(cpu); break;
        case 0x66: die(cpu); break;
        case 0x67: die(cpu); break;
        case 0x68: die(cpu); break;
        case 0x69: die(cpu); break;
        case 0x6A: die(cpu); break;
        case 0x6B: die(cpu); break;
        case 0x6C: die(cpu); break;
        case 0x6D: die(cpu); break;
        case 0x6E: die(cpu); break;
        case 0x6F: die(cpu); break;

        case 0x70: die(cpu); break;
        case 0x71: die(cpu); break;
        case 0x72: die(cpu); break;
        case 0x73: die(cpu); break;
        case 0x74: die(cpu); break;
        case 0x75: die(cpu); break;
        case 0x76: die(cpu); break;
        case 0x77: die(cpu); break;
        case 0x78: die(cpu); break;
        case 0x79: die(cpu); break;
        case 0x7A: die(cpu); break;
        case 0x7B: die(cpu); break;
        case 0x7C: die(cpu); break;
        case 0x7D: die(cpu); break;
        case 0x7E: die(cpu); break;
        case 0x7F: die(cpu); break;

        case 0x80: die(cpu); break;
        case 0x81: die(cpu); break;
        case 0x82: die(cpu); break;
        case 0x83: die(cpu); break;
        case 0x84: die(cpu); break;
        case 0x85: die(cpu); break;
        case 0x86: die(cpu); break;
        case 0x87: die(cpu); break;
        case 0x88: die(cpu); break;
        case 0x89: die(cpu); break;
        case 0x8A: die(cpu); break;
        case 0x8B: die(cpu); break;
        case 0x8C: die(cpu); break;
        case 0x8D: die(cpu); break;
        case 0x8E: die(cpu); break;
        case 0x8F: die(cpu); break;

        case 0x90: die(cpu); break;
        case 0x91: die(cpu); break;
        case 0x92: die(cpu); break;
        case 0x93: die(cpu); break;
        case 0x94: die(cpu); break;
        case 0x95: die(cpu); break;
        case 0x96: die(cpu); break;
        case 0x97: die(cpu); break;
        case 0x98: die(cpu); break;
        case 0x99: die(cpu); break;
        case 0x9A: die(cpu); break;
        case 0x9B: die(cpu); break;
        case 0x9C: die(cpu); break;
        case 0x9D: die(cpu); break;
        case 0x9E: die(cpu); break;
        case 0x9F: die(cpu); break;

        case 0xA0: die(cpu); break;
        case 0xA1: die(cpu); break;
        case 0xA2: die(cpu); break;
        case 0xA3: die(cpu); break;
        case 0xA4: die(cpu); break;
        case 0xA5: die(cpu); break;
        case 0xA6: die(cpu); break;
        case 0xA7: die(cpu); break;
        case 0xA8: die(cpu); break;
        case 0xA9: die(cpu); break;
        case 0xAA: die(cpu); break;
        case 0xAB: die(cpu); break;
        case 0xAC: die(cpu); break;
        case 0xAD: die(cpu); break;
        case 0xAE: die(cpu); break;
        case 0xAF: die(cpu); break;

        case 0xB0: die(cpu); break;
        case 0xB1: die(cpu); break;
        case 0xB2: die(cpu); break;
        case 0xB3: die(cpu); break;
        case 0xB4: die(cpu); break;
        case 0xB5: die(cpu); break;
        case 0xB6: die(cpu); break;
        case 0xB7: die(cpu); break;
        case 0xB8: die(cpu); break;
        case 0xB9: die(cpu); break;
        case 0xBA: die(cpu); break;
        case 0xBB: die(cpu); break;
        case 0xBC: die(cpu); break;
        case 0xBD: die(cpu); break;
        case 0xBE: die(cpu); break;
        case 0xBF: die(cpu); break;

        case 0xC0: die(cpu); break;
        case 0xC1: die(cpu); break;
        case 0xC2: die(cpu); break;
        case 0xC3: die(cpu); break;
        case 0xC4: die(cpu); break;
        case 0xC5: die(cpu); break;
        case 0xC6: die(cpu); break;
        case 0xC7: die(cpu); break;
        case 0xC8: die(cpu); break;
        case 0xC9: die(cpu); break;
        case 0xCA: die(cpu); break;
        case 0xCB: die(cpu); break;
        case 0xCC: die(cpu); break;
        case 0xCD: die(cpu); break;
        case 0xCE: die(cpu); break;
        case 0xCF: die(cpu); break;

        case 0xD0: die(cpu); break;
        case 0xD1: die(cpu); break;
        case 0xD2: die(cpu); break;
        case 0xD3: die(cpu); break;
        case 0xD4: die(cpu); break;
        case 0xD5: die(cpu); break;
        case 0xD6: die(cpu); break;
        case 0xD7: die(cpu); break;
        case 0xD8: die(cpu); break;
        case 0xD9: die(cpu); break;
        case 0xDA: die(cpu); break;
        case 0xDB: die(cpu); break;
        case 0xDC: die(cpu); break;
        case 0xDD: die(cpu); break;
        case 0xDE: die(cpu); break;
        case 0xDF: die(cpu); break;

        case 0xE0: die(cpu); break;
        case 0xE1: die(cpu); break;
        case 0xE2: die(cpu); break;
        case 0xE3: die(cpu); break;
        case 0xE4: die(cpu); break;
        case 0xE5: die(cpu); break;
        case 0xE6: die(cpu); break;
        case 0xE7: die(cpu); break;
        case 0xE8: die(cpu); break;
        case 0xE9: die(cpu); break;
        case 0xEA: die(cpu); break;
        case 0xEB: die(cpu); break;
        case 0xEC: die(cpu); break;
        case 0xED: die(cpu); break;
        case 0xEE: die(cpu); break;
        case 0xEF: die(cpu); break;

        case 0xF0: die(cpu); break;
        case 0xF1: die(cpu); break;
        case 0xF2: die(cpu); break;
        case 0xF3: die(cpu); break;
        case 0xF4: die(cpu); break;
        case 0xF5: die(cpu); break;
        case 0xF6: die(cpu); break;
        case 0xF7: die(cpu); break;
        case 0xF8: die(cpu); break;
        case 0xF9: die(cpu); break;
        case 0xFA: die(cpu); break;
        case 0xFB: die(cpu); break;
        case 0xFC: die(cpu); break;
        case 0xFD: die(cpu); break;
        case 0xFE: die(cpu); break;
        case 0xFF: die(cpu); break;
    }

    return cycles;
}

#define MAX_RAM_SIZE 0x10000 // 16 kB

void
load_rom_to_memory(cpu_8080_t * cpu, const char * filename)
{
    FILE * fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Could not open the 8080 ROM %s.\n", filename);
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    unsigned char * buffer = &cpu->memory[0];
    fread(buffer, fsize, 1, fp);
    fclose(fp);
}

int
main(int argc, const char * argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Please supply an 8080 ROM.\n");
        return 1;
    }

    cpu_8080_t * cpu = calloc(1, sizeof(cpu_8080_t));
    cpu->memory = malloc(MAX_RAM_SIZE);

    load_rom_to_memory(cpu, argv[1]);

    for(;;)
    {
        process_instruction(cpu);
    }

    free(cpu->memory);
    free(cpu);

    return 0;
}
