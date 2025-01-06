#ifndef __CHIP8_H__
#define __CHIP8_H__

/// ************************** Chip8-c运行频率和定时器 ****************************** ///
// chip8 运行频率540Hz
#define CHIP8_CYCLES_FREQ_HZ 540
// 每个周期延迟时间
#define CHIP8_CYCLES_DELAY_MS(X) (1000000 / (X))
// 定时器的更新频率
#define CHIP8_TIMER_FREQ_HZ (1000000 / 60)
/// ****************************************************************************** ///


/// *****************************Chip8-c内存分配*********************************** ///
// Memory Map:
// +---------------+= 0xFFF (4095) End of Chip-8 RAM
// |               |
// |               |
// |               |
// |               |
// |               |
// | 0x200 to 0xFFF|
// |     Chip-8    |
// | Program / Data|
// |     Space     |
// |               |
// |               |
// |               |
// +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
// |               |
// |               |
// |               |
// +---------------+= 0x200 (512) Start of most Chip-8 programs
// | 0x000 to 0x1FF|
// | Reserved for  |
// |  interpreter  |
// +---------------+= 0x000 (0) Start of Chip-8 RAM

// chip-8 内存大小(4 KB)
#define CHIP8_MEMORY_SIZE 4096
// chip-8 程序加载的起始地址（0x200）
#define CHIP8_MEMORY_START_ADDR 0x200


/// *****************************Chip8-c外设配置********************************** ///
///                               Kegyboard 
// The computers which originally used the Chip-8 Language had a 16-key 
// hexadecimal keypad with the following layout:
// 1	2	3	C
// 4	5	6	D
// 7	8	9	E
// A	0	B	F
// chip-8的键盘大小
#define CHIP8_KEY_SIZE 16

///                                 Display
// The original implementation of the Chip-8 language used 
// a 64x32-pixel monochrome display with this format:
// -----------------
// |(0,0)	(63,0) |
// |(0,31)	(63,31)|
// -----------------
// chip-8 显示分辨率（64x32）
#define CHIP8_DISPLAY_HEIGHT 64
#define CHIP8_DISPLAY_WIDTH 32

// Chip-8 draws graphics on screen through the use of 
// sprites. A sprite is a group of bytes which are a
// binary representation of the desired picture. Chip-8 
// sprites may be up to 15 bytes, for a possible sprite 
// size of 8x15.
// chip8-c 显示像素的颜色值（32 位 ARGB 格式）
#define CHIP8_DISPLAY_WHITE 0xFFFFFFFF
#define CHIP8_DISPLAY_BLACK 0x00000000

// Programs may also refer to a group of sprites 
// representing the hexadecimal digits 0 through F. 
// These sprites are 5 bytes long, or 8x5 pixels. 
// The data should be stored in the interpreter area of 
// Chip-8 memory (0x000 to 0x1FF). Below is a listing of 
// each character's bytes, in binary and hexadecimal
// Chip-8字符集的大小
#define CHIP8_FONTSET_SIZE 80
// Chip-8字符集起始地址
#define CHIP8_FONTSET_MEM_START_ADDR 0x50
// Chip-8字符集数据定义
extern uint8_t chip8_fontset[];
/// ****************************************************************************** ///


/// **************************Chip-8数据类型定义************************************ ///
typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;
/// ****************************************************************************** ///


/// ***************************Chip-8系统状态************************************** ///
enum system_state
{
    CHIP8_SYS_STATE_EXIT,     // 退出
    CHIP8_SYS_STATE_RUNNING,  // 运行
    CHIP8_SYS_STATE_PAUSE     // 暂停
};
/// ****************************************************************************** ///


/// ***************************Chip-8系统结构体************************************* ///
typedef struct chip8_system
{
    byte memory[CHIP8_MEMORY_SIZE];  // 4 KB 内存 存储程序和数据
    byte register[16];               // 16 个 8 位通用寄存器（V0-VF）
    word index_register;             // 6 位索引寄存器（I），用于存储内存地址。
    word pc;                         // 程序计数器，指向当前指令的地址。
    word opcode;                     // 当前操作码
    word stack[16];                  // 调用栈，用于存储返回地址。
    byte sp;                         // 调用栈指针
    byte delay_timer;                // 延迟定时器
    byte sound_timer;                // 音频定时器
    dword display[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH]; // 显示缓冲区，存储像素状态。
    byte keys[CHIP8_KEY_SIZE];       // 键盘状态，记录按键是否按下。
    byte display_refresh_flags;      // 标志是否需要刷新显示。
    enum system_state state;         // 系统状态（退出、运行、暂停）

}CHIP8;
/// ****************************************************************************** ///


/// *********************************chip8函数声明********************************* ///
CHIP8 *chip8_init();  // 初始化chip8系统
byte chip8_load_program(CHIP8 *chip8, const char *rom); // 加载程序
void chip8_emulate_cycle(CHIP8 *chip8); // 模拟一个周期
void chip8_timer(CHIP8 *chip8);         // 执行一个CPU周期
/// ****************************************************************************** ///


/// *********************************操作码宏************************************** ///
#define _OPCODE     (chip8->opcode)
#define X(opcode)   (byte)((0x0F00 & opcode) >> 8)
#define Y(opcode)   (byte)((0x00F0 & opcode) >> 4)
#define N(opcode)   (byte)(0x000F & opcode)
#define NN(opcode)  (byte)(0x00FF & opcode)
#define NNN(opcode) (word)(0x0FFF & opcode)

#define VX          (chip8->register[X(_OPCODE)])
#define VY          (chip8->register[Y(_OPCODE)])
#define _VF          (chip8->register[0xF])
#define _I           (chip8->index_register)
/// ****************************************************************************** ///


/// ********************************操作码函数声明********************************** ///
typedef void (*opcode_func)(CHIP8 *chip8);
#define OPCODE(N) opcode_##N(chip8)

void opcode_00E0(CHIP8 *chip8); // 00E0 - CLS 清空屏幕                        
void opcode_00EE(CHIP8 *chip8); // 00EE - RET 从子程序返回                 
void opcode_1NNN(CHIP8 *chip8); // 1NNN - JP addr 跳转到地址                   
void opcode_2NNN(CHIP8 *chip8); // 2NNN - CALL addr 调用子程序
void opcode_3XNN(CHIP8 *chip8); // 3XNN - SE Vx, byte 跳过下一个指令            VX == NN 
void opcode_4XNN(CHIP8 *chip8); // 4XNN - SNE Vx, byte VX != NN 跳过下一个指令  VX != NN
void opcode_5XY0(CHIP8 *chip8); // 5XY0 - SE Vx, Vy 跳过下一个指令              VX == VY
void opcode_6XNN(CHIP8 *chip8); // 6XNN - LD Vx, byte 设置寄存器                VX = NN
void opcode_7XNN(CHIP8 *chip8); // 7XNN - ADD Vx, byte 加法                    VX += NN
void opcode_8XY0(CHIP8 *chip8); // 8XY0 - LD Vx, Vy 设置寄存器                  VX = VY
void opcode_8XY1(CHIP8 *chip8); // 8XY1 - OR Vx, Vy 位运算                      VX |= VY
void opcode_8XY2(CHIP8 *chip8); // 8XY2 - AND Vx, Vy 位运算                     VX &= VY
void opcode_8XY3(CHIP8 *chip8); // 8XY3 - XOR Vx, Vy 位运算                     VX ^= VY
void opcode_8XY4(CHIP8 *chip8); // 8XY4 - ADD Vx, Vy 加法                       VX += VY
void opcode_8XY5(CHIP8 *chip8); // 8XY5 - SUB Vx, Vy 减法                       VX -= VY
void opcode_8XY6(CHIP8 *chip8); // 8XY6 - SHR Vx 右移                           VX >>= 1
void opcode_8XY7(CHIP8 *chip8); // 8XY7 - SUBN Vx, Vy 减法                      VX = VY - VX
void opcode_8XYE(CHIP8 *chip8); // 8XYE - SHL Vx 左移                           VX <<= 1
void opcode_9XY0(CHIP8 *chip8); // 9XY0 - SNE Vx, Vy 跳过下一个指令               VX != VY
void opcode_ANNN(CHIP8 *chip8); // ANNN - LD I, addr 设置索引寄存器               I = NNN
void opcode_BNNN(CHIP8 *chip8); // BNNN - JP V0, addr 跳转到地址                 PC = V0 + NNN
void opcode_CXNN(CHIP8 *chip8); // CXNN - RND Vx, byte 随机数                   VX = rand() & NN
void opcode_DXYN(CHIP8 *chip8); // DXYN - DRW Vx, Vy, nibble 显示              （VX，VY）显示像素
void opcode_EX9E(CHIP8 *chip8); // EX9E - SKP Vx 跳过下一个指令                 键盘按下
void opcode_EXA1(CHIP8 *chip8); // EXA1 - SKNP Vx 跳过下一个指令                键盘未按下
void opcode_FX07(CHIP8 *chip8); // FX07 - LD Vx, DT 设置寄存器                  VX = 延迟定时器
void opcode_FX0A(CHIP8 *chip8); // FX0A - LD Vx, K 键盘输入                     存储到VX
void opcode_FX15(CHIP8 *chip8); // FX15 - LD DT, Vx 设置定时器                  延迟定时器 = VX
void opcode_FX18(CHIP8 *chip8); // FX18 - LD ST, Vx 设置定时器                  音频定时器 = VX
void opcode_FX1E(CHIP8 *chip8); // FX1E - ADD I, Vx 设置索引寄存器               I += VX
void opcode_FX29(CHIP8 *chip8); // FX29 - LD F, Vx 设置字符集                   I = Vx 的像素地址
void opcode_FX33(CHIP8 *chip8); // FX33 - LD B, Vx 存储BCD                     Vx的BCD表示存储到I、I+1、I+2
void opcode_FX55(CHIP8 *chip8); // FX55 - LD [I], Vx 存储寄存器                 V0-Vx存储到I开始地址  
void opcode_FX65(CHIP8 *chip8); // FX65 - LD Vx, [I] 读取寄存器                 I开始地址读取到V0-Vx
/// ****************************************************************************** ///

#endif