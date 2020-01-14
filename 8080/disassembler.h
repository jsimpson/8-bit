#ifndef DISASSEMBLER_8080_H_
#define DISASSEMBLER_8080_H_

typedef struct instruction
{
    char size;
    char mnemonic[255];
} instruction_t;

int disassemble(unsigned char * buf, int program_counter);

#endif /* !DISASSEMBLER_8080_H_ */
