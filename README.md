# c8e
Virtual Machine CHIP-8 Emulator

Introduction
------------
This is a work in progress interpreter emulator. The CHIP-8 was not an actual machine but a virtual machine which allowed programmers to debug and test their designs. This is a self research project inspired by my compilers and computer architecture classes.

Notes
------------
The core interpreter is functional but the graphics display is not yet working. You won't see much by using this program just the OPCODE Instructions it reads and the status of the memory and registers. 

TO DO
------------
Implement graphics, disassembler and input.

How To Use
------------
In root directory execute 'make'
Then execute with: c8vm tetris.c8 
