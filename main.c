#include <stdio.h>
#include <stdlib.h>
#include "8080/disassembler.h"

int
main(int argc, char const * argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Please supply an 8080 ROM.\n");
        return 1;
    }

    FILE * fp = fopen(argv[1], "rb");
    if (fp == NULL)
    {
        printf("Couldn't open %s\n", argv[1]);
        return(1);
    }

    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    void * buf = malloc(fsize);

    if (fread((unsigned char *)buf, fsize, 1, fp) != 1)
    {
        printf("Failed to read data.\n");
        return 1;
    }

    fclose(fp);

    char disassembled[255];

    int offset = 0;
    while (offset < fsize)
    {
        printf("%04X ", offset);
        offset += disassemble((unsigned char *)buf, disassembled, offset);
        printf("%s\n", disassembled);
    }

    free(buf);
    return 0;
}
