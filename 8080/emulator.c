#include <stdio.h>
#include <stdlib.h>
#include "emulator.h"

void
die(state_8080_t * state)
{
    // The program counter advances ahead of any errors.
    state->program_counter -= 1;

    printf("Instructon 0x%02x\n", state->memory[state->program_counter]);
    printf("PC         0x%04x\n", state->program_counter);
    printf("SP         0x%04x\n", state->stack_pointer);
    printf("IE         0x%02x\n", state->interrupt_enabled);
    printf("A          0x%02x\n", state->a);
    printf("B          0x%02x\n", state->b);
    printf("C          0x%02x\n", state->c);
    printf("D          0x%02x\n", state->d);
    printf("E          0x%02x\n", state->e);
    printf("H          0x%02x\n", state->h);
    printf("L          0x%02x\n", state->l);
    printf("sign       0x%02x\n", state->condition_codes.s);
    printf("zero       0x%02x\n", state->condition_codes.z);
    printf("aux carry  0x%02x\n", state->condition_codes.a);
    printf("parity     0x%02x\n", state->condition_codes.p);
    printf("carry      0x%02x\n", state->condition_codes.c);
    exit(1);
}

/*
 * The parity bit is set when the number of bits set in a byte are even and
 * reset when they are odd.
 */
int
parity(unsigned char byte)
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
 * Immediate instructions.
 * Instructions that perform operations on byte(s) which are part of the
 * instruction itself.
 */

// Load a register pair immediately.
int
lxi(state_8080_t * state, unsigned char rp, unsigned char * opcode)
{
    switch(rp)
    {
        case BC:
            state->c = opcode[1];
            state->b = opcode[2];
            state->program_counter += 2;
            printf("c: 0x%2X, b: 0x%2X\n", state->c, state->b);
            break;
        default:
            die(state);
            break;
    }

    return 10;
}

int
emulate(state_8080_t * state)
{
    printf("%02X\n", state->program_counter);
    unsigned char * opcode = &state->memory[state->program_counter];

    state->program_counter++;

    switch(*opcode)
    {
        case 0x00: // NOP
            break;
        case 0x01: // LXI B, D16
            lxi(state, BC, opcode);
            exit(0);
            break;
        case 0x02:
            {
                unsigned short int register_pair = (state->b << 8) | (state->c);
                state->memory[register_pair] = state->a;
            }
            break;
        case 0x03:
            {
                unsigned short int register_pair = (state->b << 8) | (state->c);
                register_pair += 1;
                state->b = (register_pair & 0xFF00) >> 8;
                state->c = register_pair & 0xFF;
            }
            break;
        case 0x04:
            state->b = state->b + 1;
            state->condition_codes.z = ((state->b) == 0);
            state->condition_codes.s = ((state->b & 0x80) == 0x80);
            state->condition_codes.p = parity(state->b);
            state->condition_codes.a = ((state->b & 0xf) == 0xf);
            break;
        case 0x05:
            state->b = state->b - 1;
            state->condition_codes.z = ((state->b) == 0);
            state->condition_codes.s = ((state->b & 0x80) == 0x80);
            state->condition_codes.p = parity(state->b);
            state->condition_codes.a = ((state->b & 0xf) == 0x0);
            break;
        case 0x06:
            state->b = opcode[1];
            state->program_counter++;
            break;
        case 0x07:
            {
                unsigned char x = state->a;
                state-> a = ((x << 1) | (x & 0x80) >> 7);
                state->condition_codes.c = ((x & 0x80) == 0x80);
            }
            break;
        case 0x09:
            {
                unsigned int register_pair_HL = (state->h << 8) | (state->l);
                unsigned int register_pair_BC = (state->b << 8) | (state->c);
                register_pair_HL += register_pair_BC;
                state->condition_codes.c = (register_pair_HL > 0xff);
                state->h = (register_pair_HL & 0xff00) >> 8;
                state->l = register_pair_HL & 0xff;
            }
            break;
        case 0x0A:
            {
                unsigned short int register_pair = (state->b << 8) | (state->c);
                state->a = state->memory[register_pair];
            }
            break;
        case 0x0B:
            {
                unsigned short int register_pair = (state->b << 8) | (state->c);
                register_pair -= - 1;
                state->b = (register_pair & 0xff00) >> 8;
                state->c = register_pair & 0xff;
            }
            break;
        case 0x0C:
            state->c = state->c + 1;
            state->condition_codes.z = ((state->c) == 0);
            state->condition_codes.s = ((state->c & 0x80) == 0x80);
            state->condition_codes.p = parity(state->c);
            state->condition_codes.a = ((state->c & 0xf) == 0xf);
            break;
        case 0x0D:
            state->c = state->c - 1;
            state->condition_codes.z = ((state->c) == 0);
            state->condition_codes.s = ((state->c & 0x80) == 0x80);
            state->condition_codes.p = parity(state->c);
            state->condition_codes.a = ((state->c & 0xf) == 0x0);
            break;
        case 0x0E:
            state->c = opcode[1];
            state->program_counter++;
            break;
        case 0x0F:
            {
                unsigned char x = state->a;
                state-> a = ((x & 1) << 7 | (x >> 1));
                state->condition_codes.c = ((x & 1) == 1);
            }
            break;

        case 0x11:
            state->e = opcode[1];
            state->d = opcode[2];
            state->program_counter += 2;
            break;
        case 0x12:
            {
                unsigned short int register_pair = (state->d << 8) | (state->e);
                state->memory[register_pair] = state->a;
            }
            break;
        case 0x13:
            {
                unsigned short int register_pair = (state->d << 8) | (state->e);
                register_pair += 1;
                state->d = (register_pair & 0xff00) >> 8;
                state->e = register_pair & 0xff;
            }
            break;
        case 0x14:
            state->d = state->d + 1;
            state->condition_codes.z = ((state->d) == 0);
            state->condition_codes.s = ((state->d & 0x80) == 0x80);
            state->condition_codes.p = parity(state->d);
            state->condition_codes.a = ((state->d & 0xf) == 0xf);
            break;
        case 0x15:
            state->d = state->d - 1;
            state->condition_codes.z = ((state->d) == 0);
            state->condition_codes.s = ((state->d & 0x80) == 0x80);
            state->condition_codes.p = parity(state->d);
            state->condition_codes.a = ((state->d & 0xf) == 0x0);
            break;
        case 0x16:
            state->d = opcode[1];
            state->program_counter++;
            break;
        case 0x17:
            {
                unsigned char x = state->a;
                state-> a = ((x << 1) | state->condition_codes.c);
                state->condition_codes.c = ((x & 0x80) == 0x80);
            }
            break;
        case 0x18: break;
        case 0x19:
            {
                unsigned int register_pair_HL = (state->h << 8) | (state->l);
                unsigned int register_pair_DE = (state->d << 8) | (state->e);
                register_pair_HL += register_pair_DE;
                state->condition_codes.c = (register_pair_HL > 0xff);
                state->h = (register_pair_HL & 0xff00) >> 8;
                state->l = register_pair_HL & 0xff;
            }
            break;
        case 0x1A:
            {
                unsigned short int register_pair = (state->d << 8) | (state->e);
                state->a = state->memory[register_pair];
            }
            break;
        case 0x1B:
            {
                unsigned short int register_pair = (state->d << 8) | (state->e);
                register_pair -= 1;
                state->b = (register_pair & 0xff00) >> 8;
                state->e = register_pair & 0xff;
            }
            break;
        case 0x1C:
            state->e = state->e + 1;
            state->condition_codes.z = ((state->e) == 0);
            state->condition_codes.s = ((state->e & 0x80) == 0x80);
            state->condition_codes.p = parity(state->e);
            state->condition_codes.a = ((state->e & 0xf) == 0xf);
            break;
        case 0x1D:
            state->e = state->e - 1;
            state->condition_codes.z = ((state->e) == 0);
            state->condition_codes.s = ((state->e & 0x80) == 0x80);
            state->condition_codes.p = parity(state->e);
            state->condition_codes.a = ((state->e & 0xf) == 0x0);
            break;
        case 0x1E:
            state->e = opcode[1];
            state->program_counter++;
            break;
        case 0x1F:
            {
                unsigned char x = state->a;
                state-> a = ((state->condition_codes.c << 7) | (x >> 1));
                state->condition_codes.c = ((x & 1) == 1);
            }
            break;

        case 0x20: die(state); break;
        case 0x21: die(state); break;
        case 0x22: die(state); break;
        case 0x23: die(state); break;
        case 0x24: die(state); break;
        case 0x25: die(state); break;
        case 0x26: die(state); break;
        case 0x27: die(state); break;
        case 0x28: die(state); break;
        case 0x29: die(state); break;
        case 0x2A: die(state); break;
        case 0x2B: die(state); break;
        case 0x2C: die(state); break;
        case 0x2D: die(state); break;
        case 0x2E: die(state); break;
        case 0x2F: die(state); break;

        case 0x30: die(state); break;
        case 0x31: die(state); break;
        case 0x32: die(state); break;
        case 0x33: die(state); break;
        case 0x34: die(state); break;
        case 0x35: die(state); break;
        case 0x36: die(state); break;
        case 0x37: die(state); break;
        case 0x38: die(state); break;
        case 0x39: die(state); break;
        case 0x3A: die(state); break;
        case 0x3B: die(state); break;
        case 0x3C: die(state); break;
        case 0x3D: die(state); break;
        case 0x3E: die(state); break;
        case 0x3F: die(state); break;

        case 0x40:
            state->b = state->c;
            break;
        case 0x41:
            state->b = state->d;
            break;
        case 0x42:
            state->b = state->e;
            break;
        case 0x43: die(state); break;
        case 0x44: die(state); break;
        case 0x45: die(state); break;
        case 0x46: die(state); break;
        case 0x47: die(state); break;
        case 0x48: die(state); break;
        case 0x49: die(state); break;
        case 0x4A: die(state); break;
        case 0x4B: die(state); break;
        case 0x4C: die(state); break;
        case 0x4D: die(state); break;
        case 0x4E: die(state); break;
        case 0x4F: die(state); break;

        case 0x50: die(state); break;
        case 0x51: die(state); break;
        case 0x52: die(state); break;
        case 0x53: die(state); break;
        case 0x54: die(state); break;
        case 0x55: die(state); break;
        case 0x56: die(state); break;
        case 0x57: die(state); break;
        case 0x58: die(state); break;
        case 0x59: die(state); break;
        case 0x5A: die(state); break;
        case 0x5B: die(state); break;
        case 0x5C: die(state); break;
        case 0x5D: die(state); break;
        case 0x5E: die(state); break;
        case 0x5F: die(state); break;

        case 0x60: die(state); break;
        case 0x61: die(state); break;
        case 0x62: die(state); break;
        case 0x63: die(state); break;
        case 0x64: die(state); break;
        case 0x65: die(state); break;
        case 0x66: die(state); break;
        case 0x67: die(state); break;
        case 0x68: die(state); break;
        case 0x69: die(state); break;
        case 0x6A: die(state); break;
        case 0x6B: die(state); break;
        case 0x6C: die(state); break;
        case 0x6D: die(state); break;
        case 0x6E: die(state); break;
        case 0x6F: die(state); break;

        case 0x70: die(state); break;
        case 0x71: die(state); break;
        case 0x72: die(state); break;
        case 0x73: die(state); break;
        case 0x74: die(state); break;
        case 0x75: die(state); break;
        case 0x76: die(state); break;
        case 0x77: die(state); break;
        case 0x78: die(state); break;
        case 0x79: die(state); break;
        case 0x7A: die(state); break;
        case 0x7B: die(state); break;
        case 0x7C: die(state); break;
        case 0x7D: die(state); break;
        case 0x7E: die(state); break;
        case 0x7F: die(state); break;

        case 0x80: die(state); break;
        case 0x81: die(state); break;
        case 0x82: die(state); break;
        case 0x83: die(state); break;
        case 0x84: die(state); break;
        case 0x85: die(state); break;
        case 0x86: die(state); break;
        case 0x87: die(state); break;
        case 0x88: die(state); break;
        case 0x89: die(state); break;
        case 0x8A: die(state); break;
        case 0x8B: die(state); break;
        case 0x8C: die(state); break;
        case 0x8D: die(state); break;
        case 0x8E: die(state); break;
        case 0x8F: die(state); break;

        case 0x90: die(state); break;
        case 0x91: die(state); break;
        case 0x92: die(state); break;
        case 0x93: die(state); break;
        case 0x94: die(state); break;
        case 0x95: die(state); break;
        case 0x96: die(state); break;
        case 0x97: die(state); break;
        case 0x98: die(state); break;
        case 0x99: die(state); break;
        case 0x9A: die(state); break;
        case 0x9B: die(state); break;
        case 0x9C: die(state); break;
        case 0x9D: die(state); break;
        case 0x9E: die(state); break;
        case 0x9F: die(state); break;

        case 0xA0: die(state); break;
        case 0xA1: die(state); break;
        case 0xA2: die(state); break;
        case 0xA3: die(state); break;
        case 0xA4: die(state); break;
        case 0xA5: die(state); break;
        case 0xA6: die(state); break;
        case 0xA7: die(state); break;
        case 0xA8: die(state); break;
        case 0xA9: die(state); break;
        case 0xAA: die(state); break;
        case 0xAB: die(state); break;
        case 0xAC: die(state); break;
        case 0xAD: die(state); break;
        case 0xAE: die(state); break;
        case 0xAF: die(state); break;

        case 0xB0: die(state); break;
        case 0xB1: die(state); break;
        case 0xB2: die(state); break;
        case 0xB3: die(state); break;
        case 0xB4: die(state); break;
        case 0xB5: die(state); break;
        case 0xB6: die(state); break;
        case 0xB7: die(state); break;
        case 0xB8: die(state); break;
        case 0xB9: die(state); break;
        case 0xBA: die(state); break;
        case 0xBB: die(state); break;
        case 0xBC: die(state); break;
        case 0xBD: die(state); break;
        case 0xBE: die(state); break;
        case 0xBF: die(state); break;

        case 0xC0: die(state); break;
        case 0xC1: die(state); break;
        case 0xC2: die(state); break;
        case 0xC3: die(state); break;
        case 0xC4: die(state); break;
        case 0xC5: die(state); break;
        case 0xC6: die(state); break;
        case 0xC7: die(state); break;
        case 0xC8: die(state); break;
        case 0xC9: die(state); break;
        case 0xCA: die(state); break;
        case 0xCB: die(state); break;
        case 0xCC: die(state); break;
        case 0xCD: die(state); break;
        case 0xCE: die(state); break;
        case 0xCF: die(state); break;

        case 0xD0: die(state); break;
        case 0xD1: die(state); break;
        case 0xD2: die(state); break;
        case 0xD3: die(state); break;
        case 0xD4: die(state); break;
        case 0xD5: die(state); break;
        case 0xD6: die(state); break;
        case 0xD7: die(state); break;
        case 0xD8: die(state); break;
        case 0xD9: die(state); break;
        case 0xDA: die(state); break;
        case 0xDB: die(state); break;
        case 0xDC: die(state); break;
        case 0xDD: die(state); break;
        case 0xDE: die(state); break;
        case 0xDF: die(state); break;

        case 0xE0: die(state); break;
        case 0xE1: die(state); break;
        case 0xE2: die(state); break;
        case 0xE3: die(state); break;
        case 0xE4: die(state); break;
        case 0xE5: die(state); break;
        case 0xE6: die(state); break;
        case 0xE7: die(state); break;
        case 0xE8: die(state); break;
        case 0xE9: die(state); break;
        case 0xEA: die(state); break;
        case 0xEB: die(state); break;
        case 0xEC: die(state); break;
        case 0xED: die(state); break;
        case 0xEE: die(state); break;
        case 0xEF: die(state); break;

        case 0xF0: die(state); break;
        case 0xF1: die(state); break;
        case 0xF2: die(state); break;
        case 0xF3: die(state); break;
        case 0xF4: die(state); break;
        case 0xF5: die(state); break;
        case 0xF6: die(state); break;
        case 0xF7: die(state); break;
        case 0xF8: die(state); break;
        case 0xF9: die(state); break;
        case 0xFA: die(state); break;
        case 0xFB: die(state); break;
        case 0xFC: die(state); break;
        case 0xFD: die(state); break;
        case 0xFE: die(state); break;
        case 0xFF: die(state); break;
    }

    return 1;
}

#define MAX_RAM_SIZE 0x10000 // 16 kB

int
main(int argc, const char * argv[])
{
    state_8080_t * state = calloc(1,sizeof(state_8080_t));
    state->memory = malloc(MAX_RAM_SIZE);

    FILE *f = fopen("data/invaders.rom", "rb");
    if (f==NULL)
    {
        printf("error: Could not open\n");
        exit(1);
    }

    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char * buffer = &state->memory[0];
    fread(buffer, fsize, 1, f);
    fclose(f);

    for(;;)
    {
        emulate(state);
    }

    return 0;
}
