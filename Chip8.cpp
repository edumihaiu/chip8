#include "Chip8.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <random>
#include <sys/types.h>

Chip8::Chip8() {
  // rng
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

  randGen = std::default_random_engine(seed);
  randByte = std::uniform_int_distribution<uint8_t>(0, 255);

  // initialize program counter
  pc = MEM_START_ADDRESS;

  // load font to memory
  for (size_t i = 0; i < FONTSET_SIZE; ++i) {
    memory[FONT_START_ADDRESS + i] = fontset[i];
  }
}

void Chip8::loadROMtoMemory(const char *filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (file.is_open()) {
    std::streampos size = file.tellg();
    char *buffer = new char[size];

    // go back to beginning of file
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    // put buffer into memory
    for (size_t i = 0; i < size; ++i) {
      memory[MEM_START_ADDRESS + i] = static_cast<uint8_t>(buffer[i]);
    }

    // free the buffer
    delete[] buffer;
  }
}

void Chip8::OP_00E0() { memset(video, 0, sizeof(video)); }
void Chip8::OP_00EE() {
  --sp;
  pc = stack[sp];
}

void Chip8::OP_1NNN() {
  uint16_t address = opcode & 0x0FFFu;
  pc = address;
}

void Chip8::OP_2NNN() {
  stack[sp++] = pc;
  uint16_t address = opcode & 0x0FFFu;
  pc = address;
}

void Chip8::OP_3XNN() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t address = opcode & 0x00FFu;
  if (registers[Vx] == address)
    pc += 2;
}

void Chip8::OP_4XNN() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t address = opcode & 0x00FFu;
  if (registers[Vx] != address)
    pc += 2;
}

void Chip8::OP_5XY0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
  if (registers[Vx] == registers[Vy])
    pc += 2;
}

void Chip8::OP_6XNN() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t NN = opcode & 0x00FFu;

  registers[Vx] = NN;
}

void Chip8::OP_7XNN() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t NN = opcode & 0x00FFu;

  registers[Vx] += NN;
}

void Chip8::OP_8XY0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
  registers[Vx] = registers[Vy];
}

void Chip8::OP_8XY1() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
  registers[Vx] = (registers[Vx] | registers[Vy]);
}

void Chip8::OP_8XY2() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
  registers[Vx] = (registers[Vx] & registers[Vy]);
}

void Chip8::OP_8XY3() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
  registers[Vx] = (registers[Vx] ^ registers[Vy]);
}

void Chip8::OP_8XY4() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
  uint16_t sum = registers[Vx] + registers[Vy];
  
  if (sum > 255) registers[0xF] = 1;
  else registers[0xF] = 0;

  registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8XY5() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
  uint16_t subtract = registers[Vx] - registers[Vy];
  
  if (registers[Vx] >= registers[Vy]) registers[0xF] = 1;
  else registers[0xF] = 0;

  registers[Vx] -= registers[Vy];
}

void Chip8::OP_8XY6() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  
  registers[0xF] = (registers[Vx] & 0x01u);
  registers[Vx] >>= 1;
}

void Chip8::OP_8XY7() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
  if (registers[Vx] >= registers[Vy]) registers[0xF] = 1;
  else registers[0xF] = 0;

  registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8XYE() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  

  registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

  registers[Vx] <<= 1;
}

void Chip8::OP_9XY0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  if (registers[Vx] != registers[Vy])
    pc += 2;
}

void Chip8::OP_ANNN() {
  uint16_t NNN = opcode & 0x0FFFu;
  index_register = NNN;
}

void Chip8::OP_BNNN() {
  uint16_t NNN = opcode & 0x0FFFu;
  pc = registers[0x0u] + NNN;
}

void Chip8::OP_CXNN() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t NN = opcode & 0x00FFu;

  registers[Vx] = randByte(randGen) & NN;
}

void Chip8::OP_DXYN() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  uint8_t N = opcode & 0x000Fu;
  
  uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
  uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

  registers[0xF] = 0;
  for (unsigned int row = 0; row < N; ++row) {
    uint8_t spriteByte = memory[index_register + row];

    for (unsigned int col = 0; col < 8; ++col) {
      uint8_t spritePixel = spriteByte & (0x80u >> col);
      uint32_t *screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];
      
      // screenpixel on
      if (spritePixel) {
        if (*screenPixel == 0xFFFFFFFF) {
          registers[0xF] = 1;
        }
      // XOR with sprite pixel
      *screenPixel ^= 0xFFFFFFFF;
      }
    }
  }
}

void Chip8::OP_EX9E() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t key = registers[Vx];

  if (keypad[key]) {
    pc += 2;
  }
}

void Chip8::OP_EXA1() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t key = registers[Vx];

  if (!keypad[key]) {
    pc += 2;
  }
}

void Chip8::OP_FX07() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  registers[Vx] = delayTimer;
}

void Chip8::OP_FX0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  for (uint8_t i = 0; i < 16; ++i) {
    if (keypad[i]) {
        registers[Vx] = i;
        return;
      }
  }
  pc -= 2;
}

void Chip8::OP_FX15()
{
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  delayTimer = registers[Vx];
}

void Chip8::OP_FX18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	soundTimer = registers[Vx];
}

void Chip8::OP_FX1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	index_register += registers[Vx];
}

void Chip8::OP_FX29()
{
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t digit = registers[Vx];

  index_register = FONT_START_ADDRESS + (5 * digit);
}

void Chip8::OP_FX33()
{
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t value = registers[Vx];

  uint8_t hundreds = value / 100;
  uint8_t tens = (value / 10) % 10;
  uint8_t units = value % 10;

  memory[index_register] = hundreds;
  memory[index_register + 1] = tens;
  memory[index_register + 2] = units;
}

void Chip8::OP_FX55() 
{
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  for (uint8_t i = 0; i <= Vx; ++i)
  {
    memory[index_register + i] = registers[i];
  }
}

void Chip8::OP_FX65() 
{
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  for (uint8_t i = 0; i <= Vx; ++i)
  {
    registers[i] = memory[index_register + i];
  }
}

void Chip8::Cycle() {
  //fetch
  opcode = (memory[pc] << 8u) | memory[pc + 1];
  pc += 2;

  //decode
  uint16_t category = opcode & 0xF000;

  switch(category) {
    case 0x0000:
      switch (opcode) {
        case 0x00E0:
          OP_00E0();
          break;
        case 0x00EE:
          OP_00EE();
          break;
        default:
          break;
      }
    break;
    case 0x1000:
      OP_1NNN();
      break;
    case 0x2000:
      OP_2NNN();
      break;
    case 0x3000:
      OP_3XNN();
      break;
    case 0x4000:
      OP_4XNN();
      break;
    case 0x5000:
      OP_5XY0();
      break;
    case 0x6000:
      OP_6XNN();
      break;
    case 0x7000:
      OP_7XNN();
      break;
    case 0x8000:
      switch (opcode & 0x000F) {
        case 0x0000:
          OP_8XY0();
          break;
        case 0x0001:
          OP_8XY1();
          break;
        case 0x0002:
          OP_8XY2();
          break;
        case 0x0003:
          OP_8XY3();
          break;
        case 0x0004:
          OP_8XY4();
          break;
        case 0x0005:
          OP_8XY5();
          break;
        case 0x0006:
          OP_8XY6();
          break;
        case 0x0007:
          OP_8XY7();
          break;
        case 0x000E:
          OP_8XYE();
          break;
        default:
          break;
      }
    case 0x9000:
      OP_9XY0();
      break;
    case 0xA000:
      OP_ANNN();
      break;
    case 0xB000:
      OP_BNNN();
      break;
    case 0xC000:
      OP_CXNN();
      break;
    case 0xD000:
      OP_DXYN();
      break;  
    case 0xE000:
      switch (opcode & 0x00FF) {
        case 0x009E:
          OP_EX9E();
          break;
        case 0x00A1:
          OP_EXA1();
          break;
        default:
          break;
      }
    case 0xF000:
      switch (opcode & 0x00FF) {
        case 0x0007:
          OP_FX07();
          break;
        case 0x000A:
          OP_FX0A();
          break;
        case 0x0015:
          OP_FX15();
          break;
        case 0x0018:
          OP_FX18();
          break;
        case 0x001E:
          OP_FX1E();
          break;
        case 0x0029:
          OP_FX29();
          break;
        case 0x0033:
          OP_FX33();
          break;
        case 0x0055:
          OP_FX55();
          break;
        case 0x0065:
          OP_FX65();
          break;
        default:
          break;
      }
    default:
      break;
  }

}