#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <cstdint>
#include <random>

#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

class Chip8 {
public:

  uint8_t registers[16]{};
  uint8_t memory[4096]{};
  uint16_t stack[16]{};
  uint8_t sp{};
  uint8_t delayTimer{};
  uint8_t soundTimer{};
  uint8_t keypad[16]{};
  uint16_t pc{};
  uint16_t index_register{};
  uint32_t video[64 * 32]{};
  uint16_t opcode;

  // rng
  std::default_random_engine randGen;
  std::uniform_int_distribution<uint8_t> randByte{0, 255};

  static inline constexpr unsigned int MEM_START_ADDRESS = 0x200;
  static inline constexpr unsigned int FONT_START_ADDRESS = 0x50;
  static inline constexpr unsigned int FONTSET_SIZE = 80;
  static inline constexpr uint8_t fontset[FONTSET_SIZE] = {
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

  Chip8();
  void loadROMtoMemory(char const *filename);
  void Cycle(void);
private:
  void OP_00E0();
  void OP_00EE();
  void OP_2NNN();
  void OP_3XNN();
  void OP_4XNN();
  void OP_5XY0();
  void OP_6XNN();
  void OP_7XNN();
  void OP_8XY0();
  void OP_8XY1();
  void OP_8XY2();
  void OP_8XY3();
  void OP_8XY4();
  void OP_8XY5();
  void OP_8XY6();
  void OP_8XY7();
  void OP_8XYE();
  void OP_9XY0();
  void OP_ANNN();
  void OP_BNNN();
  void OP_CXNN();
  void OP_DXYN();
  void OP_EX9E();
  void OP_EXA1();
  void OP_FX07();
  void OP_FX0A();
  void OP_FX15();
  void OP_FX18();
  void OP_FX1E();
  void OP_FX29();
  void OP_FX33();
  void OP_FX55();
  void OP_FX65();
  void OP_1NNN();
};

#endif