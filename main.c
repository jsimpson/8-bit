#include <stdio.h>
#include <stdlib.h>
#include "8080/disassembler.h"

int
main(int argc, char const * argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Please supply an executable.\n");
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

    size_t size = fread((unsigned char *)buf, fsize, 1, fp);
    printf("%lu\n", size);
    fclose(fp);

    int offset = 0;
    while (offset < fsize)
    {
        offset += disassemble((unsigned char *)buf, offset);
    }

    free(buf);
    return 0;
}
