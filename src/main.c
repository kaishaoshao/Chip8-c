#include "chip8.h"

int main(int argc, char *argv[])
{
    CHIP8 *chip8 = chip8_init();
    if (!chip8)
        return -1;
    printf("Chip-8 Emulator\n");
    free(chip8);
    return 0;
}