#include "c8.h"

void systemStart(chip8 *c8)
{
    //Setup Program Counter
    c8->PC = 0x200;

    //Clear Reg File, Stack, and Key input
    for (int i = 0; i < 16; i++)
        c8->key[i] = c8->V[i] = c8->stack[i] = 0;

    //Clear Memory
    for (int i = 0; i < MEMLOCS; i++)
        c8->memory[i] = 0;

    //Clear Graphics
    for (int i = 0; i < MEMLOCS / 2; i++)
        c8->gfx[i] = 0;

    //Clear remaining Registers and parametres
    c8->SP = 0;
    c8->soundTimer = 0;
    c8->delayTimer = 0;
    c8->IREG = 0;
    c8->opcode = 0;

    return;
}

//Obtain next instruction and increment PC Register
void fetch(chip8 *c8)
{
    c8->opcode = (c8->memory[c8->PC] << 8) | c8->memory[(c8->PC) + 1];
    c8->PC += 2;
    return;
}

void decode(chip8 *c8)
{
    //Decode block
    printf("Opcode Out: %x", c8->opcode);
    switch ((c8->opcode & 0xF000) >> 12)
    {
    case 0:

        switch (c8->opcode & 0x000F)
        {
        case 0: //00E0 Clears the screen
            //puts("Clear the screen!");
            for(int i = 0; i < MEMLOCS/2; i++) c8->gfx[i] = 0;
            //set draw flag
            
            break;

        case 0xE: //00EE Returns from a subroutine -- return;
            c8->PC = c8->stack[--(c8->SP)];
            break;

        default:
            puts("Unknown OpCode 0x0000!");
            exit(-1);
            break;
        }
        break;

    case 1:
        c8->PC = c8->opcode & 0x0FFF;
        break;

    case 2: //2NNN The interpreter increments the stack pointer, 
            //then puts the current PC on the top of the stack. The PC is then set to NNN
        c8->stack[(c8->SP)++] = c8->PC;
        c8->PC = c8->opcode & 0x0FFF;
        break;

    case 3:

        if ((c8->V[(c8->opcode & 0x0F00) >> 8]) == (c8->opcode & 0x00FF))
        {
            c8->PC += 2;
        }

        break;

    case 4: //4XNN Skips the next instruction if VX doesn't equal NN
        if ((c8->V[(c8->opcode & 0x0F00) >> 8]) != (c8->opcode & 0x00FF))
        {
            c8->PC += 2;
        }
        break;

    case 5:
        if ((c8->V[(c8->opcode & 0x0F00) >> 8]) == (c8->V[(c8->opcode & 0x00F0) >> 4]))
        {
            c8->PC += 2;
        }
        break;

    case 6: //6XNN Set Vx = NN
        c8->V[(c8->opcode & 0x0F00) >> 8] = c8->opcode & 0x00FF;
        break;

    case 7: //7XNN Adds NN to VX. (Carry flag is not changed) 
        c8->V[(c8->opcode & 0x0F00) >> 8] += c8->opcode & 0x00FF;
        break;

    case 8:

        switch (c8->opcode & 0x000F)
        {
        case 0: //8XY0 Sets VX to the value of VY.
            (c8->V[(c8->opcode & 0x0F00) >> 8]) = (c8->V[(c8->opcode & 0x00F0) >> 4]);
            break;

        case 1: //8XY1 Sets VX to VX or VY. (Bitwise OR operation)
            (c8->V[(c8->opcode & 0x0F00) >> 8]) |= (c8->V[(c8->opcode & 0x00F0) >> 4]);
            break;

        case 2: //8XY2 Sets VX to VX and VY. (Bitwise AND operation)
            (c8->V[(c8->opcode & 0x0F00) >> 8]) &= (c8->V[(c8->opcode & 0x00F0) >> 4]);
            break;

        case 3: //8XY3 Sets VX to VX xor VY.
            (c8->V[(c8->opcode & 0x0F00) >> 8]) ^= (c8->V[(c8->opcode & 0x00F0) >> 4]);
            break;

        case 4: //8XY4 Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
            c8->V[0xF] = c8->V[(c8->opcode & 0x0F00) >> 8] > (255 - c8->V[(c8->opcode & 0x00F0) >> 4]) ? 1 : 0;
            c8->V[(c8->opcode & 0x0F00) >> 8] += c8->V[(c8->opcode & 0x00F0) >> 4];
            break;

        case 5: //8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
            c8->V[0xF] = c8->V[(c8->opcode & 0x0F00) >> 8] > c8->V[(c8->opcode & 0x00F0) >> 4] ? 1 : 0;
            c8->V[(c8->opcode & 0x0F00) >> 8] -= c8->V[(c8->opcode & 0x00F0) >> 4];
            break;

        case 6: //8XY6 Stores the least significant bit of VX in VF and then shifts VX to the right by 1
            c8->V[0xF] = c8->opcode >> 0xF;
            c8->V[(c8->opcode & 0x0F00 >> 8)] >>= 1;
            break;

        case 7: //8XY7 Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
            c8->V[0xF] = c8->V[(c8->opcode & 0x0F00) >> 8] > c8->V[(c8->opcode & 0x00F0) >> 4] ? 1 : 0;
            c8->V[(c8->opcode & 0x0F00) >> 8] = c8->V[(c8->opcode & 0x00F0) >> 4] - c8->V[(c8->opcode & 0x0F00) >> 8];
            break;

        case 0xE: //8XYE Stores the most significant bit of VX in VF and then shifts VX to the left by 1
            c8->V[0xF] = (c8->opcode >> 0xF);
            (c8->V[(c8->opcode & 0x0F00) >> 8]) <<= 1;
            break;

        default:
            puts("Unknown OpCode 0x8000!");
            exit(-1);
            break;
        }
        break;

    case 9: //9XY0 Skips the next instruction if VX doesn't equal VY
        c8->PC += (c8->V[(c8->opcode & 0x0F00) >> 8]) != c8->V[(c8->opcode & 0x00F0) >> 4] ? 2 : 0;
        break;

    case 0xA: //ANNN Sets I to the address NNN
        c8->IREG = c8->opcode & 0x0FFF;
        break;

    case 0xB: //BNNN Jumps to the address NNN plus V0
        c8->PC = c8->V[0] + (c8->opcode & 0x0FFF);
        break;

    case 0xC: // CXNN Sets VX to result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
        c8->V[(c8->opcode & 0x0F00) >> 8] = (rand() % 256) & (c8->opcode & 0x00FF);
        break;

    case 0xD: //DXYN Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels
              //Each row of 8 pixels is read as bit-coded starting from memory location I; 
              //I value doesn’t change after the execution of this instruction. 
              //As described above, VF is set to 1 if any screen pixels are flipped from set to unset 
              //when the sprite is drawn, and to 0 if that doesn’t happen
        puts("Display required");
        break;

    case 0xE:

        switch (c8->opcode & 0x000F)
        {
        case 0xE: // EX9E Skips the next instruction if the key stored in VX is pressed
            c8->PC += c8->key[c8->V[(c8->opcode & 0x0F00) >> 8]] != 0 ? 2 : 0;
            break;

        case 0x1: //EXA1 Skips the next instruction if the key stored in VX isn't pressed
            c8->PC += c8->key[c8->V[(c8->opcode & 0x0F00) >> 8]] == 0 ? 2 : 0;
            break;

        default:
            puts("Unknown opcode 0xE000");
            exit(-1);
            break;
        }
        break;

    case 0xF:
        /*
         	Timer 	Vx = get_delay() 	.

        */
        switch (c8->opcode & 0x00FF)
        {
        case 0x07: //FX07 Sets VX to the value of the delay timer
            c8->V[(c8->opcode & 0x0F00) >> 8] = c8->delayTimer;
            break;

        case 0x0A: //FX0A Wait for a key press, store the value of the key in Vx
        {
            bool pressed = false;
            for (int i = 0; i < 16; i++)
            {
                if (c8->key[i])
                {
                    pressed = true;
                    c8->V[(c8->opcode & 0x0F00) >> 8] = i;
                }
            }
            //Onto next cycle if not pressed and return
            if (pressed == false)
            {
                return;
            }
        }
        break;

        case 0x15: //FX15 Sets the delay timer to VX.
            c8->delayTimer = c8->V[(c8->opcode & 0x0F00) >> 8];
            break;

        case 0x18: //FX18 Sets the sound timer to VX.
            c8->soundTimer = c8->V[(c8->opcode & 0x0F00) >> 8];
            break;

        case 0x1E: //FX1E Set I = I + Vx
            c8->IREG += c8->V[(c8->opcode & 0x0F00) >> 8];
            break;

        case 0x29: //FX29 Set I = location of sprite for digit Vx
            //c8->IREG +=
            puts("Display FX29!");
            break;

        case 0x33: //FX33 Store BCD representation of Vx in memory locations I, I+1, and I+2
            c8->memory[c8->IREG] = c8->V[(c8->opcode & 0x0F00) >> 8] / 100;
            c8->memory[c8->IREG + 1] = (c8->V[(c8->opcode & 0x0F00) >> 8] / 10) % 10;
            c8->memory[c8->IREG + 2] = (c8->V[(c8->opcode & 0x0F00) >> 8] % 10) % 10;
            break;

        case 0x55: //FX55 Store registers V0 through Vx in memory starting at location I
        {
            int vx = (c8->opcode & 0x0F00) >> 8;
            for (int i = 0; i < vx; i++)
                c8->memory[c8->IREG + i] = c8->V[i];
        }

        break;

        case 0x65: //FX65 Read registers V0 through Vx from memory starting at location I
        {
            int vx = (c8->opcode & 0x0F00) >> 8;
            for (int i = 0; i < vx; i++)
                c8->V[i] = c8->memory[c8->IREG + i];
        }

        break;
        default:
            puts("Unknown OpCode! 0xF000!");
            break;
        }
        break;

    default:
    puts("Unidentified Opcode!");
        break;
    }
    return;
}

void execute(chip8 *c8)
{
    return;
}

void loadData(chip8 *c8, char *ch)
{
    FILE *fptr;
    char *buffer;
    long filelen;

    fptr = fopen(ch, "rb");   // Open in Binary Mode
    fseek(fptr, 0, SEEK_END); // Seek to end of the file
    filelen = ftell(fptr);    // Get byte offset in the file
    rewind(fptr);             // Return back to beginning of the file

    buffer = (char *)malloc((filelen + 1) * sizeof(char)); // Enough memory for file + \0
    fread(buffer, filelen, 1, fptr);                       // Read in the entire file
    fclose(fptr);                                          // Close the file
    printf("\nSize of File: %u Bytes\n", filelen);         //Print size of ROM

    if (filelen < (4096 - 512)) memcpy((void *)(&(c8->memory[512])), buffer, filelen);
    
    free(buffer);
    return;
}

void debugDisplay(chip8 *c8)
{
    puts("Registers\n");
    for (int i = 0; i < NUMREGS; i += 4)
    {
        printf("\tV%02d=0x%.2X\tV%02d=0x%.2X\tV%02d=0x%.2X\tV%02d=0x%.2X\n",
               i, c8->V[i],
               i + 1, c8->V[i + 1],
               i + 2, c8->V[i + 2],
               i + 3, c8->V[i + 3]);
    }

    puts("\nSpecial Registers\n");
    printf("\tSound Timer=0x%.2X\tDelay Timer=0x%.2X\tSP=0x%.2X\n\tPC=0x%.4X\t\tI=0x%.4X\n",
           c8->soundTimer, c8->delayTimer, c8->SP, c8->PC, c8->IREG);

    puts("\nSoftware Stack Addresses\n");
    for (int i = 0; i < NUMREGS; i += 2)
    {
        printf("\tLoc%02d=0x%.4X\tLoc%02d=0x%.4X\n",
               i, c8->stack[i], i + 1, c8->stack[i + 1]);
    }

    printf("\nCurrent OpCode: 0x%.4X\n", c8->opcode);

    /* puts("Memory Space\n");
    for (int i = 0; i < MEMLOCS; i += 4)
    {

        printf("\tLoc%02d=0x%.2X\tLoc%02d=0x%.2X\tLoc%02d=0x%.2X\tLoc%02d=0x%.2X\n",
               i, c8->memory[i], i + 1, c8->memory[i + 1],
               i + 2, c8->memory[i + 2], i + 3, c8->memory[i + 3]);
    } */
    return;
}