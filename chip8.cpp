#include "chip8.hpp"
#include <chrono>
#include <cstdint>
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
