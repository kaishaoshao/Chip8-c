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


/// **************************Chip-8数据类型定义***************************************** ///
typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;
/// ****************************************************************************** ///


/// ***************************Chip-8系统状态************************************* ///
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





/// ****************************************************************************** ///

/// ****************************************************************************** ///

#endif