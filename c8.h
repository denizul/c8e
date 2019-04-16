#if !defined(C8_H)
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#define C8_H
#define NUMREGS 16
#define MEMLOCS 4096

typedef struct 
{
    unsigned short opcode;              //Current OPcode of program
    unsigned short stack[16];           //Stack for storing PC register for jump based routines
    unsigned short SP;                  //Stack Pointer
    unsigned short soundTimer;          //DSP Register, counts down to 0 at 60Hz 
    unsigned short delayTimer;          //DSP Register, counts down to 0 at 60Hz
    unsigned short PC;                  //Program Counter
    unsigned short IREG;                //Special Addressing Register
    unsigned char V[NUMREGS];           //General Purpose Registers
    unsigned char memory[MEMLOCS];      //4096 memory locations, each 8 bits wide
    unsigned char gfx[64 * 32];         //Display uses 2048 pixels which can be emulated using binary
    unsigned char key[16];              //Array to hold key state for I/O

} chip8;

const unsigned char fontSet[80] =
{ 
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

void systemStart(chip8 *c8);
void fetch(chip8 *c8);
void decode(chip8 *c8);
void execute(chip8 *c8);
void debugDisplay(chip8 *c8);
void loadData(chip8 *c8, char *ch);
#endif