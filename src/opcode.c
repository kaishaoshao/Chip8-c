#include "chip8.h"

// 清屏
void opcode_00E0(CHIP8 *chip8)
{
    memset(chip8->display, CHIP8_DISPLAY_BLACK, sizeof(chip8->display));
}

// 子程序返回  调用栈中弹出返回地址
void opcode_00EE(CHIP8 *chip8)
{
    if (chip8->sp > 0)
        chip8->pc = chip8->stack[--(chip8->sp)];
    else 
    {
        printf("stack underflow\n");
        exit(1);
    }
}

// 无条件跳转
void opcode_1NNN(CHIP8 *chip8)
{
    chip8->pc = NNN(_OPCODE);
}

// 调用子程序 压入调用栈中
void opcode_2NNN(CHIP8 *chip8)
{
    chip8->stack[chip8->sp++] = chip8->pc;
    chip8->pc = NNN(_OPCODE);
}

// 条件跳转(VX == NN)
void opcode_3XNN(CHIP8 *chip8)
{
    if(VX(_OPCODE) == NN(_OPCODE))
        chip8->pc += 2;
}

// 条件跳转(VX != NN)
void opcode_4XNN(CHIP8 *chip8)
{
    if(VX(_OPCODE) != NN(_OPCODE))
        chip8->pc += 2;
}

// 条件跳转(VX == VY)
void opcode_5XY0(CHIP8 *chip8)
{
    if(VX(_OPCODE) == VY(_OPCODE))
        chip8->pc += 2;
}

// 将立即数加载到寄存器中
void opcode_6XNN(CHIP8 *chip8)
{
    VX(_OPCODE) = NN(_OPCODE);
}

// 寄存器值增加
void opcode_7XNN(CHIP8 *chip8)
{
    VX(_OPCODE) += NN(_OPCODE);
}

// 将一个寄存器的值复制到另外一个寄存器中
void opcode_8XY0(CHIP8 *chip8)
{
    VX(_OPCODE) = VY(_OPCODE);
}

// 用于按位或操作
void opcode_8XY1(CHIP8 *chip8)
{
    VX(_OPCODE) |= VY(_OPCODE);
}

// 用于按位与操作
void opcode_8XY2(CHIP8 *chip8)
{
    VX(_OPCODE) &= VY(_OPCODE);
}

// 用于按位异或操作
void opcode_8XY3(CHIP8 *chip8)
{
    VX(_OPCODE) ^= VY(_OPCODE);
}

// 用于加法操作，并检测溢出
void opcode_8XY4(CHIP8 *chip8)
{
    word add = (word)VX(_OPCODE) + (word)VY(_OPCODE);
    VX(_OPCODE) = add & 0xFF;
    // 检测溢出
    _VF = (add & 0x0100) >> 8;
}

// 用于减法操作，并检测借位。
void opcode_8XY5(CHIP8 *chip8)
{
   if(VX(_OPCODE) > VY(_OPCODE))
       _VF = 1;
   else
       _VF = 0;
    VX(_OPCODE) -= VY(_OPCODE);
}

// 用于右移操作，并保存最低位。
void opcode_8XY6(CHIP8 *chip8)
{
    _VF = (VX(_OPCODE) & 0x01);
    VX(_OPCODE) = VX(_OPCODE) >> 1;
}

// 用于反向减法操作，并检测借位
void opcode_8XY7(CHIP8 *chip8)
{
    if(VX(_OPCODE) < VY(_OPCODE))
        _VF = 1;
    else
        _VF = 0;
    VX(_OPCODE) = VY(_OPCODE) - VX(_OPCODE);
}

// 用于左移操作，并保存最高位
void opcode_8XYE(CHIP8 *chip8)
{
    _VF = (VX(_OPCODE) & 0x80) >> 7;
    VX(_OPCODE) = VX(_OPCODE) << 1;
}

// 用于条件跳转，比较两个寄存器的值
void opcode_9XY0(CHIP8 *chip8)
{
    if(VX(_OPCODE) != VY(_OPCODE))
        chip8->pc += 2;
}

// 用于设置内存地址
void opcode_ANNN(CHIP8 *chip8)
{
    chip8->index_register = NNN(_OPCODE);
}

// 用于跳转 
void opcode_BNNN(CHIP8 *chip8)
{
    chip8->pc = NNN(_OPCODE) + chip8->registers[0];
}

// 用于生成随机数
void opcode_CXNN(CHIP8 *chip8)
{
    VX(_OPCODE) = NN(_OPCODE) & (byte)(rand() % 256);
}

// 绘制
void opcode_DXYN(CHIP8 *chip8)
{  
    // 绘制
    chip8->registers[0xF] = 0;

    // 获取坐标
    int x = X(_OPCODE);
    int y = Y(_OPCODE);
    int n = NN(_OPCODE); 
    byte start_x = chip8->registers[x] & (CHIP8_DISPLAY_WIDTH - 1);
    byte start_y = chip8->registers[y] & (CHIP8_DISPLAY_HEIGHT - 1);
    word index = chip8->index_register;

    // 循环绘制
    for (byte height = 0; height < n; height++)
    {
        for (byte width = 0; width < 8; width++)
        {
            // 获取当前像素值
            byte pixel = chip8->memory[index] & (0x80 >> width);
            // 获取当前位置的像素值
            byte current_x = start_x + width;
            byte current_y = start_y + height;

            if(current_x >= CHIP8_DISPLAY_WIDTH || current_y >= CHIP8_DISPLAY_HEIGHT)
                continue;
            if(pixel)
            {
                if (chip8->display[current_y][current_x] == CHIP8_DISPLAY_WHITE)
                {
                   chip8->registers[0xF] = 1;
                   chip8->display[current_y][current_x] = CHIP8_DISPLAY_BLACK;
                }
                else
                {
                    chip8->display[current_y][current_x] = CHIP8_DISPLAY_WHITE;
                }
                chip8->display_refresh_flags = 1;
                
            }
            
        }
        index++;
    }
}

// 用于按键检测--如果键被按下，则跳转
void opcode_EX9E(CHIP8 *chip8)
{
    if (VX(_OPCODE) >= 16)
        return;
    if (chip8->keys[VX(_OPCODE)])
        chip8->pc += 2;
}

// 用于按键检测--如果键没有被按下，则跳转
void opcode_EXA1(CHIP8 *chip8)
{
    if (VX(_OPCODE) >= 16)
        return;
    if (!(chip8->keys[VX(_OPCODE)]))
        chip8->pc += 2;
}

// 获取当前时间
void opcode_FX07(CHIP8 *chip8)
{
    VX(_OPCODE) = chip8->delay_timer;
}

// 等待按键按下
void opcode_FX0A(CHIP8 *chip8)
{
    byte key_pressed = 0;
    for(byte i = 0; i < CHIP8_KEY_SIZE - 1; i++)
    {
        if (chip8->keys[i])
        {
            VX(_OPCODE) = i;
            key_pressed = 1;
        }
    }
    if (!key_pressed)
        chip8->pc -= 2;
}

// 设置延时定时器
void opcode_FX15(CHIP8 *chip8)
{
    chip8->delay_timer = VX(_OPCODE);
}

// 设置声音定时器
void opcode_FX18(CHIP8 *chip8)
{
    chip8->sound_timer = VX(_OPCODE);
}

// 延时定时器加1
void opcode_FX1E(CHIP8 *chip8)
{
    _I += VX(_OPCODE);
}

// 设置 I 为字符的字符地址
void opcode_FX29(CHIP8 *chip8)
{
    _I = chip8->memory[CHIP8_FONTSET_MEM_START_ADDR + 5 * VX(_OPCODE)];
}

// 将VX的值存储为BCD码
void opcode_FX33(CHIP8 *chip8)
{
    int x = VX(_OPCODE);
    byte one = x % 10;
    byte ten = x / 10u % 10u;
    byte hundred = x / 100u % 10u;
    chip8->memory[_I] = one;
    chip8->memory[_I + 1] = ten;
    chip8->memory[_I + 2] = hundred;
}

// 将V0到VX的值存储到内存I中
void opcode_FX55(CHIP8 *chip8)
{
    for (int i = 0; i <= X(_OPCODE); i++) {
        chip8->memory[_I + i] = chip8->registers[i];
    }
}

// 从内存I中加载值到寄存器V0到VX中
void opcode_FX65(CHIP8 *chip8)
{
    for(int i = 0; i <= X(_OPCODE); i++)
        chip8->registers[i] = chip8->memory[_I + i];
}
