#include "Chip8.hpp"
#include "platform.hpp"
#include <chrono>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("you forgot to pass the rom!\n");
        return 1;
    }

    Platform platform("chip8 emulator", 640, 320, 64, 32);
    Chip8 chip8;
    chip8.loadROMtoMemory(argv[1]);

    int pitch = (sizeof(chip8.video[0])) * 64;
    
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while(!quit)
    {
        quit = platform.ProcessInput(chip8.keypad);
        chip8.Cycle();
        platform.Update(chip8.video, pitch);

    }
    return 0;
}
