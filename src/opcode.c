#include "chip8.h"

void opcode_00E0(CHIP8 *chip8);
void opcode_00EE(CHIP8 *chip8);
void opcode_1NNN(CHIP8 *chip8);
void opcode_2NNN(CHIP8 *chip8);
void opcode_3XNN(CHIP8 *chip8);
void opcode_4XNN(CHIP8 *chip8);
void opcode_5XY0(CHIP8 *chip8);
void opcode_6XNN(CHIP8 *chip8);
void opcode_7XNN(CHIP8 *chip8);
void opcode_8XY0(CHIP8 *chip8);
void opcode_8XY1(CHIP8 *chip8);
void opcode_8XY2(CHIP8 *chip8);
void opcode_8XY3(CHIP8 *chip8);
void opcode_8XY4(CHIP8 *chip8);
void opcode_8XY5(CHIP8 *chip8);
void opcode_8XY6(CHIP8 *chip8);
void opcode_8XY7(CHIP8 *chip8);
void opcode_8XYE(CHIP8 *chip8);
void opcode_9XY0(CHIP8 *chip8);
void opcode_ANNN(CHIP8 *chip8);
void opcode_BNNN(CHIP8 *chip8);
void opcode_CXNN(CHIP8 *chip8);
void opcode_DXYN(CHIP8 *chip8);
void opcode_EX9E(CHIP8 *chip8);
void opcode_EXA1(CHIP8 *chip8);
void opcode_FX07(CHIP8 *chip8);
void opcode_FX0A(CHIP8 *chip8);
void opcode_FX15(CHIP8 *chip8);
void opcode_FX18(CHIP8 *chip8);
void opcode_FX1E(CHIP8 *chip8);
void opcode_FX29(CHIP8 *chip8);
void opcode_FX33(CHIP8 *chip8);
void opcode_FX55(CHIP8 *chip8);
void opcode_FX65(CHIP8 *chip8);

// 定义操作码处理函数类型
typedef void (*OpcodeHandler)(CHIP8 *, uint16_t);

// 操作码处理函数声明
void handle_00E0(CHIP8 *chip8, uint16_t opcode); // 清屏
void handle_00EE(CHIP8 *chip8, uint16_t opcode); // 返回
void handle_1NNN(CHIP8 *chip8, uint16_t opcode); // 跳转到地址 NNN
void handle_2NNN(CHIP8 *chip8, uint16_t opcode); // 调用子程序
void handle_3XNN(CHIP8 *chip8, uint16_t opcode); // 如果 VX == NN，跳过下一条指令
void handle_4XNN(CHIP8 *chip8, uint16_t opcode); // 如果 VX != NN，跳过下一条指令
void handle_5XY0(CHIP8 *chip8, uint16_t opcode); // 如果 VX == VY，跳过下一条指令
void handle_6XNN(CHIP8 *chip8, uint16_t opcode); // 设置 VX = NN
void handle_7XNN(CHIP8 *chip8, uint16_t opcode); // 设置 VX += NN
void handle_8XY0(CHIP8 *chip8, uint16_t opcode); // 设置 VX = VY
void handle_8XY1(CHIP8 *chip8, uint16_t opcode); // 设置 VX |= VY
void handle_8XY2(CHIP8 *chip8, uint16_t opcode); // 设置 VX &= VY
void handle_8XY3(CHIP8 *chip8, uint16_t opcode); // 设置 VX ^= VY
void handle_8XY4(CHIP8 *chip8, uint16_t opcode); // 设置 VX += VY，设置 VF 为进位
void handle_8XY5(CHIP8 *chip8, uint16_t opcode); // 设置 VX -= VY，设置 VF 为借位
void handle_8XY6(CHIP8 *chip8, uint16_t opcode); // 设置 VX >>= 1，设置 VF 为最低位
void handle_8XY7(CHIP8 *chip8, uint16_t opcode); // 设置 VX = VY - VX，设置 VF 为借位
void handle_8XYE(CHIP8 *chip8, uint16_t opcode); // 设置 VX <<= 1，设置 VF 为最高位
void handle_9XY0(CHIP8 *chip8, uint16_t opcode); // 如果 VX != VY，跳过下一条指令
void handle_ANNN(CHIP8 *chip8, uint16_t opcode); // 设置 I = NNN
void handle_BNNN(CHIP8 *chip8, uint16_t opcode); // 跳转到地址 NNN + V0
void handle_CXNN(CHIP8 *chip8, uint16_t opcode); // 设置 VX = 随机数 & NN
void handle_DXYN(CHIP8 *chip8, uint16_t opcode); // 绘制精灵
void handle_EX9E(CHIP8 *chip8, uint16_t opcode); // 如果按键 VX 按下，跳过下一条指令
void handle_EXA1(CHIP8 *chip8, uint16_t opcode); // 如果按键 VX 未按下，跳过下一条指令
void handle_FX07(CHIP8 *chip8, uint16_t opcode); // 设置 VX = 延迟定时器
void handle_FX0A(CHIP8 *chip8, uint16_t opcode); // 等待按键，存储到 VX
void handle_FX15(CHIP8 *chip8, uint16_t opcode); // 设置延迟定时器 = VX
void handle_FX18(CHIP8 *chip8, uint16_t opcode); // 设置声音定时器 = VX
void handle_FX1E(CHIP8 *chip8, uint16_t opcode); // 设置 I += VX
void handle_FX29(CHIP8 *chip8, uint16_t opcode); // 设置 I = 字体地址
void handle_FX33(CHIP8 *chip8, uint16_t opcode); // 存储 BCD 表示
void handle_FX55(CHIP8 *chip8, uint16_t opcode); // 存储寄存器到内存
void handle_FX65(CHIP8 *chip8, uint16_t opcode); // 加载内存到寄存器

// 查找表：操作码类型到处理函数的映射
static OpcodeHandler opcode_handlers[16] = {
    [0x0] = handle_0XXX, // 0x0XXX 操作码
    [0x1] = handle_1NNN, // 0x1NNN 操作码
    [0x2] = handle_2NNN, // 0x2NNN 操作码
    [0x3] = handle_3XNN, // 0x3XNN 操作码
    [0x4] = handle_4XNN, // 0x4XNN 操作码
    [0x5] = handle_5XY0, // 0x5XY0 操作码
    [0x6] = handle_6XNN, // 0x6XNN 操作码
    [0x7] = handle_7XNN, // 0x7XNN 操作码
    [0x8] = handle_8XXX, // 0x8XXX 操作码
    [0x9] = handle_9XY0, // 0x9XY0 操作码
    [0xA] = handle_ANNN, // 0xANNN 操作码
    [0xB] = handle_BNNN, // 0xBNNN 操作码
    [0xC] = handle_CXNN, // 0xCXNN 操作码
    [0xD] = handle_DXYN, // 0xDXYN 操作码
    [0xE] = handle_EXXX, // 0xEXXX 操作码
    [0xF] = handle_FXXX, // 0xFXXX 操作码
};

// 主循环：执行一个 CPU 周期
void chip8_cycle(CHIP8 *chip8)
{
    // 提取操作码
    uint16_t opcode = (chip8->mem[chip8->pc] << 8) | chip8->mem[chip8->pc + 1];
    chip8->pc += 2;

    // 提取操作码类型
    uint8_t type = (opcode >> 12) & 0xF;

    // 调用对应的处理函数
    if (opcode_handlers[type])
    {
        opcode_handlers[type](chip8, opcode);
    }
}

// 示例操作码处理函数
void handle_00E0(CHIP8 *chip8, uint16_t opcode)
{
    // 清屏
    memset(chip8->display, 0, sizeof(chip8->display));
}

void handle_00EE(CHIP8 *chip8, uint16_t opcode)
{
    // 返回
    chip8->pc = chip8->stack[--chip8->sp];
}

void handle_1NNN(CHIP8 *chip8, uint16_t opcode)
{
    // 跳转到地址 NNN
    chip8->pc = opcode & 0xFFF;
}

void handle_2NNN(CHIP8 *chip8, uint16_t opcode)
{
    // 调用子程序
    chip8->stack[chip8->sp++] = chip8->pc;
    chip8->pc = opcode & 0xFFF;
}

void handle_3XNN(CHIP8 *chip8, uint16_t opcode)
{
    // 如果 VX == NN，跳过下一条指令
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t nn = opcode & 0xFF;
    if (chip8->V[x] == nn)
    {
        chip8->pc += 2;
    }
}

void handle_4XNN(CHIP8 *chip8, uint16_t opcode)
{
    // 如果 VX != NN，跳过下一条指令
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t nn = opcode & 0xFF;
    if (chip8->V[x] != nn)
    {
        chip8->pc += 2;
    }
}

void handle_5XY0(CHIP8 *chip8, uint16_t opcode)
{
    // 如果 VX == VY，跳过下一条指令
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    if (chip8->V[x] == chip8->V[y])
    {
        chip8->pc += 2;
    }
}

void handle_6XNN(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX = NN
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t nn = opcode & 0xFF;
    chip8->V[x] = nn;
}

void handle_7XNN(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX += NN
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t nn = opcode & 0xFF;
    chip8->V[x] += nn;
}

void handle_8XY0(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX = VY
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    chip8->V[x] = chip8->V[y];
}

void handle_8XY1(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX |= VY
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    chip8->V[x] |= chip8->V[y];
}

void handle_8XY2(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX &= VY
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    chip8->V[x] &= chip8->V[y];
}

void handle_8XY3(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX ^= VY
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    chip8->V[x] ^= chip8->V[y];
}

void handle_8XY4(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX += VY，设置 VF 为进位
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    uint16_t sum = chip8->V[x] + chip8->V[y];
    chip8->V[0xF] = (sum > 0xFF) ? 1 : 0;
    chip8->V[x] = sum & 0xFF;
}

void handle_8XY5(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX -= VY，设置 VF 为借位
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    chip8->V[0xF] = (chip8->V[x] >= chip8->V[y]) ? 1 : 0;
    chip8->V[x] -= chip8->V[y];
}

void handle_8XY6(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX >>= 1，设置 VF 为最低位
    uint8_t x = (opcode >> 8) & 0xF;
    chip8->V[0xF] = chip8->V[x] & 0x1;
    chip8->V[x] >>= 1;
}

void handle_8XY7(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX = VY - VX，设置 VF 为借位
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    chip8->V[0xF] = (chip8->V[y] >= chip8->V[x]) ? 1 : 0;
    chip8->V[x] = chip8->V[y] - chip8->V[x];
}

void handle_8XYE(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX <<= 1，设置 VF 为最高位
    uint8_t x = (opcode >> 8) & 0xF;
    chip8->V[0xF] = (chip8->V[x] >> 7) & 0x1;
    chip8->V[x] <<= 1;
}

void handle_9XY0(CHIP8 *chip8, uint16_t opcode)
{
    // 如果 VX != VY，跳过下一条指令
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    if (chip8->V[x] != chip8->V[y])
    {
        chip8->pc += 2;
    }
}

void handle_ANNN(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 I = NNN
    chip8->I = opcode & 0xFFF;
}

void handle_BNNN(CHIP8 *chip8, uint16_t opcode)
{
    // 跳转到地址 NNN + V0
    chip8->pc = (opcode & 0xFFF) + chip8->V[0];
}

void handle_CXNN(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX = 随机数 & NN
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t nn = opcode & 0xFF;
    chip8->V[x] = (rand() % 256) & nn;
}

void handle_DXYN(CHIP8 *chip8, uint16_t opcode)
{
    // 绘制精灵
    uint8_t x = chip8->V[(opcode >> 8) & 0xF];
    uint8_t y = chip8->V[(opcode >> 4) & 0xF];
    uint8_t height = opcode & 0xF;
    chip8->V[0xF] = 0;

    for (uint8_t row = 0; row < height; row++)
    {
        uint8_t sprite = chip8->mem[chip8->I + row];
        for (uint8_t col = 0; col < 8; col++)
        {
            if (sprite & (0x80 >> col))
            {
                uint16_t index = (x + col) + ((y + row) * 64);
                if (chip8->display[index] == 1)
                {
                    chip8->V[0xF] = 1;
                }
                chip8->display[index] ^= 1;
            }
        }
    }
}

void handle_EX9E(CHIP8 *chip8, uint16_t opcode)
{
    // 如果按键 VX 按下，跳过下一条指令
    uint8_t x = (opcode >> 8) & 0xF;
    if (chip8->key[chip8->V[x]])
    {
        chip8->pc += 2;
    }
}

void handle_EXA1(CHIP8 *chip8, uint16_t opcode)
{
    // 如果按键 VX 未按下，跳过下一条指令
    uint8_t x = (opcode >> 8) & 0xF;
    if (!chip8->key[chip8->V[x]])
    {
        chip8->pc += 2;
    }
}

void handle_FX07(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 VX = 延迟定时器
    uint8_t x = (opcode >> 8) & 0xF;
    chip8->V[x] = chip8->delay_timer;
}

void handle_FX0A(CHIP8 *chip8, uint16_t opcode)
{
    // 等待按键，存储到 VX
    uint8_t x = (opcode >> 8) & 0xF;
    for (uint8_t i = 0; i < 16; i++)
    {
        if (chip8->key[i])
        {
            chip8->V[x] = i;
            return;
        }
    }
    chip8->pc -= 2; // 如果没有按键，重新执行当前指令
}

void handle_FX15(CHIP8 *chip8, uint16_t opcode)
{
    // 设置延迟定时器 = VX
    uint8_t x = (opcode >> 8) & 0xF;
    chip8->delay_timer = chip8->V[x];
}

void handle_FX18(CHIP8 *chip8, uint16_t opcode)
{
    // 设置声音定时器 = VX
    uint8_t x = (opcode >> 8) & 0xF;
    chip8->sound_timer = chip8->V[x];
}

void handle_FX1E(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 I += VX
    uint8_t x = (opcode >> 8) & 0xF;
    chip8->I += chip8->V[x];
}

void handle_FX29(CHIP8 *chip8, uint16_t opcode)
{
    // 设置 I = 字体地址
    uint8_t x = (opcode >> 8) & 0xF;
    chip8->I = chip8->V[x] * 5;
}

void handle_FX33(CHIP8 *chip8, uint16_t opcode)
{
    // 存储 BCD 表示
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t value = chip8->V[x];
    chip8->mem[chip8->I] = value / 100;
    chip8->mem[chip8->I + 1] = (value / 10) % 10;
    chip8->mem[chip8->I + 2] = value % 10;
}

void handle_FX55(CHIP8 *chip8, uint16_t opcode)
{
    // 存储寄存器到内存
    uint8_t x = (opcode >> 8) & 0xF;
    for (uint8_t i = 0; i <= x; i++)
    {
        chip8->mem[chip8->I + i] = chip8->V[i];
    }
}

void handle_FX65(CHIP8 *chip8, uint16_t opcode)
{
    // 加载内存到寄存器
    uint8_t x = (opcode >> 8) & 0xF;
    for (uint8_t i = 0; i <= x; i++)
    {
        chip8->V[i] = chip8->mem[chip8->I + i];
    }
}