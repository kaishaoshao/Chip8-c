#include "chip8.h"


// CHIP-8 字符集
// 原理：   以0 为例
// 0xF0,  // 11110000
// 0x90,  // 10010000
// 0x90,  // 10010000
// 0x90,  // 10010000
// 0xF0   // 11110000
byte chip8_fontset[CHIP8_FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/**
 * chip8_init 函数用于初始化 CHIP8 结构体。
 *
 * @return 返回一个指向初始化后的 CHIP8 结构体的指针。
 *
 * 此函数的主要职责是初始化 CHIP8 结构体，包括生成随机数种子、分配内存资源并清零、
 * 设置初始 PC 值、加载字体以及初始化运行状态。
 * 生产随机数种子是为了实现游戏随机性。
 */
CHIP8 *chip8_init()
{
    // 生成随机数种子
    srand((unsigned)time(NULL));
    // 分配内存资源并清零
    CHIP8 *chip8 = (CHIP8 *)malloc(sizeof(CHIP8));
    memset(chip8, 0, sizeof(CHIP8));
    // 设置初始 PC 值
    chip8->pc = CHIP8_MEMORY_START_ADDR;
    // 加载字体
    for (int i = 0; i < CHIP8_FONTSET_SIZE; i++)
    {
        chip8->memory[CHIP8_FONTSET_MEM_START_ADDR + i] = chip8_fontset[i];
    }
    // 初始化运行状态
    chip8->state = CHIP8_SYS_STATE_RUNNING;
    return chip8;
}

/**
 * 获取CHIP-8游戏文件的大小
 *
 * 此函数的目的是确定CHIP-8游戏文件的大小，以确保在加载游戏代码到内存时不会超出文件的边界
 * 它通过将文件指针移动到文件末尾，然后读取当前位置来确定文件大小，最后将文件指针重置到文件开头
 *
 * @param chip8_file 指向CHIP-8游戏文件的文件指针
 * @return 文件大小，以字节为单位如果文件指针为空，则返回-1
 */
static byte get_chip8_file_size(FILE *chip8_file)
{
    if(chip8_file == NULL){
        printf("chip8_file is NULL\n");
        return -1;
    }
    // 获取文件大小   
    fseek(chip8_file, 0, SEEK_END);
    long file_size = ftell(chip8_file);
    // 重置文件指针
    rewind(chip8_file);
    return file_size;
}

/**
 * 将CHIP-8文件数据以十六进制格式打印出来。
 * 此函数的目的是用于调试或验证CHIP-8文件是否正确加载或处理。
 * 它以每行16个字节的格式打印数据，确保输出具有良好的可读性。
 *
 * @param chip8_file_data 指向CHIP-8文件数据的指针。
 * @param chip8_file_size CHIP-8文件的大小，以字节为单位。
 */
static void get_ch8_hex(byte *chip8_file_data, size_t chip8_file_size)
{
    for (int  i = 0; i < chip8_file_size; i++)
    {
        printf("%02x ", chip8_file_data[i]);
        if ((i+1) % 16 == 0)
            printf("\n");
    }
}

/**
 * chip8_load_program 函数用于加载 CHIP-8 程序到 CHIP8 结构体中。
 * 
 * @param chip8 指向 CHIP8 结构体的指针，该结构体包含 CHIP-8 解释器的状态和内存。
 * @param rom 指向程序 ROM 的指针，该 ROM 是要加载到 CHIP-8 内存中的程序数据。
 * 
 * @return 返回一个 byte 类型的值，表示程序加载的状态。通常用于指示加载是否成功。
 * 
 * 此函数的主要职责是将 CHIP-8 程序（ROM）加载到 CHIP8 结构体的内存中，
 * 以便后续执行。它会根据提供的 rom 数据填充 CHIP8 结构体的相应部分。
 */
byte chip8_load_program(CHIP8 *chip8, const char *chip8_file)
{
    FILE *fp = fopen(chip8_file, "rb");
    long size =  get_chip8_file_size(fp);
    if (size > CHIP8_MEMORY_SIZE - CHIP8_MEMORY_START_ADDR) {
        fprintf(stderr, "ROM file size is too large\n");
        return -1;
    }
    
    byte *data = chip8->memory + CHIP8_MEMORY_START_ADDR; // 起始地址512
    size_t read_size = fread((void*)chip8_file, 1, size, fp);
    if (read_size != size) {
        fprintf(stderr, "ROM file read error\n");
        return -1;
    }
    get_ch8_hex(data, size);
    fclose(fp);
    return 0;
}

// TODO:
void chip8_emulate_cycle(CHIP8 *chip8)
{
    // 取指
    word opcode = (0xFF00 & (chip8->memory[chip8->pc] << 8)) 
        | chip8->memory[chip8->pc + 1];
    // 保存
    chip8->opcode = opcode;
    // 跳转 更新程序计数器
    chip8->pc += 2;
    // 译码
    byte opcode_type = (0xF000 & opcode) >> 12;

    switch (opcode_type) {
        case 0x0:
            if(opcode == 0x00E0)
                OPCODE(00E0);
            else if (opcode == 0x00EE)
                OPCODE(00EE);
            else 
                break;
            break;
        case 0x1:
            OPCODE(1NNN);
            break;
        case 0x2:
            OPCODE(2NNN);
            break;
        case 0x3:
            OPCODE(3XNN);
            break;
        case 0x4:
            OPCODE(4XNN);
            break;
        case 0x5:
            OPCODE(5XY0);
            break;
        case 0x6:
            OPCODE(6XNN);
            break;
        case 0x7:
            OPCODE(7XNN);
            break;
        case 0x8:
            switch (N(opcode))
            {
            case 0x0:
                OPCODE(8XY0);
                break;
            case 0x1:
                OPCODE(8XY1);
                break;
            case 0x2:
                OPCODE(8XY2);
                break;
            case 0x3:
                OPCODE(8XY3);
                break;
            case 0x4:
                OPCODE(8XY4);
                break;
            case 0x5:
                OPCODE(8XY5);
                break;
            case 0x6:
                OPCODE(8XY6);
                break;
            case 0x7:
                OPCODE(8XY7);
                break;
            case 0xE:
                OPCODE(8XYE);
                break;
            default:
                break;
            }
            break;
        case 0x9:
            OPCODE(9XY0);
            break;
        case 0xA:
            OPCODE(ANNN);
            break;
        case 0xB:
            OPCODE(BNNN);
            break;
        case 0xC:
            OPCODE(CXNN);
            break;
        case 0xD:
            OPCODE(DXYN);
            break;
        case 0xE:
            if (NN(_OPCODE) == 0x9E)
                OPCODE(EX9E);
            else if (NN(_OPCODE) == 0xA1)
                OPCODE(EXA1);
            else
                break;
            break;
        case 0xF:
            switch (NN(_OPCODE))
            {
            case 0x07:
                OPCODE(FX07);
                break;
            case 0x0A:
                OPCODE(FX0A);
                break;
            case 0x15:
                OPCODE(FX15);
                break;
            case 0x1E:
                OPCODE(FX1E);
                break;
            case 0x29:
                OPCODE(FX29);
                break;
            case 0x33:
                OPCODE(FX33);
                break;
            case 0x55:
                OPCODE(FX55);
                break;
            case 0x65:
                OPCODE(FX65);
                break;
            default:
                fprintf(stderr, "Unknown opcode: 0x%04X\n", opcode);
                break;
            }
            break;
        default:
            fprintf(stderr, "Unknown opcode: 0x%04X\n", opcode);
            break;
    }
}

/**
 * chip8_timer 减少chip8的计时器值
 * 延迟计时器通常用于控制游戏逻辑的延迟。例如，某些操作（如角色移动、动画更新）
 * 需要在一定时间间隔内执行。
 * 它是一个倒计时器，当计时器的值大于 0 时，会逐帧递减，直到为 0。
 * 声音计时器用于控制声音的播放。当声音计时器的值大于 0 时，模拟器通常会播放蜂鸣声
 *
 * @param chip8 指向chip8结构体的指针，该结构体包含计时器成员
 */
void chip8_timer(CHIP8 *chip8)
{
    if (chip8->delay_timer > 0)
        chip8->delay_timer--;

    if (chip8->sound_timer > 0)
        chip8->sound_timer--;
}