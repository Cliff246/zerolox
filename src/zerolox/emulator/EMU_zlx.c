#include "EMU_Context.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if(argc > 3)
    {
        fprintf(stderr, "Usage: %s [image.bin [instruction_limit]]\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint64_t limit = UINT64_MAX;
    if(argc == 3)
    {
        char *end;
        errno = 0;
        limit = strtoull(argv[2], &end, 10);
        if(errno || argv[2][0] == '-' || end == argv[2] || *end != '\0')
        {
            fprintf(stderr, "Invalid instruction limit\n");
            return EXIT_FAILURE;
        }
    }

    ZLX_EMU_Context_t *context = ZLX_EMU_init_Context();
    ZLX_EMU_load_file_Context(context, argc > 1 ? argv[1] : "image.bin");

    for(uint64_t step = 0; step < limit; ++step)
    {
        ZLX_EMU_step_Context(context);
    }

    ZLX_EMU__CPU_t *cpu = context->p__machine->p__cpu;
    printf("pc=%" PRIu32 "\n", ZLX_EMU_get_pc__CPU(cpu));

    for(unsigned i = 0; i < 32; ++i)
    {
        printf("r%u=%" PRIu32 "\n", i, ZLX_EMU_get_register__CPU(cpu, i));
    }

    ZLX_EMU_free_Context(context);
    return EXIT_SUCCESS;
}
