#include "opcodes.h"
#include "display.h"
#include "input.h"
#include "timer.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
// Type alias for an opcode handler function
typedef void (*OpcodeHandler)(Chip8 *chip8, uint16_t opcode);


/**
 * 00E0 - CLS
 * Clear the display.
 */
void op_00E0(Chip8 *chip8, uint16_t opcode) {
    // CLS - Clear the display

    memset(chip8->display, 0, sizeof(chip8->display));
    chip8->draw_flag = true;
}

/**
 * 00EE - RET
 * Return from a subroutine.
 * 
 * The interpreter sets the program counter to the address at the top of
 * the stack, then subtracts 1 from the stack pointer.
 */
void op_00EE(Chip8 *chip8, uint16_t opcode) {

    if (chip8->sp == 0) {
        fprintf(stderr, "Stack underflow on RET\n");
        return;
    }

    chip8->sp--;
    chip8->pc = chip8->stack[chip8->sp];

}

/**
 * 1nnn - JP addr
 * Jump to location nnn.
 * 
 * The interpreter sets the program counter to nnn.
 */
void op_1nnn(Chip8 *chip8, uint16_t opcode) {

    uint16_t address = opcode & 0x0FFF;
    chip8->pc = address;

}

/**
 * 2nnn - CALL addr
 * Call subroutine at nnn.
 * 
 * The interpreter increments the stack pointer, then puts the current PC
 * on the top of the stack. The PC is then set to nnn.
 */
void op_2nnn(Chip8 *chip8, uint16_t opcode) {
    
    uint16_t address = opcode & 0x0FFF;

    chip8->stack[chip8->sp] = chip8->pc;
    chip8->sp++;
    if (chip8->sp >= STACK_SIZE) {
        fprintf(stderr, "Stack overflow on CALL\n");
        return;
    }    
    chip8->pc = address;

}

/**
 * 3xkk - SE Vx, byte
 * Skip next instruction if Vx = kk.
 * 
 * The interpreter compares register Vx to kk, and if they are equal,
 * increments the program counter by 2.
 */
void op_3xkk(Chip8 *chip8, uint16_t opcode) {
    uint8_t byte = opcode & 0x00FF;
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    if (chip8->V[Vx] == byte) {
        chip8->pc += 2;
    }
}

/**
 * 4xkk - SNE Vx, byte
 * Skip next instruction if Vx != kk.
 * 
 * The interpreter compares register Vx to kk, and if they are not equal,
 * increments the program counter by 2.
 */
void op_4xkk(Chip8 *chip8, uint16_t opcode) {

    uint8_t byte = opcode & 0x00FF;
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    if (chip8->V[Vx] != byte) {
        chip8->pc += 2;
    }

}

/**
 * 5xy0 - SE Vx, Vy
 * Skip next instruction if Vx = Vy.
 * 
 * The interpreter compares register Vx to register Vy, and if they are equal,
 * increments the program counter by 2.
 */
void op_5xy0(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    if (chip8->V[Vx] == chip8->V[Vy]) {
        chip8->pc += 2;
    }

}

/**
 * 6xkk - LD Vx, byte
 * Set Vx = kk.
 * 
 * The interpreter puts the value kk into register Vx.
 */
void op_6xkk(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = (opcode & 0x00FF);

    chip8->V[Vx] = byte;
}

/**
 * 7xkk - ADD Vx, byte
 * Set Vx = Vx + kk.
 * 
 * Adds the value kk to the value of register Vx, then stores the result
 * in Vx.
 */
void op_7xkk(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = (opcode & 0x00FF);

    uint8_t result = chip8->V[Vx] + byte;

    // Check for overflow (for debug/logging purposes)
    if (result < chip8->V[Vx]) {
        fprintf(stderr, "Warning: ADD Vx, byte overflowed at V[%X]: %02X + %02X = %02X\n", Vx, chip8->V[Vx], byte, result);
    }

    chip8->V[Vx] = result;

}




/**
 * 8xy0 - LD Vx, Vy
 * Set Vx = Vy.
 * 
 * Stores the value of register Vy in register Vx.
 */
void op_8xy0(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    chip8->V[Vx] = chip8->V[Vy];

}

/**
 * 8xy1 - OR Vx, Vy
 * Set Vx = Vx OR Vy.
 * 
 * Performs a bitwise OR on the values of Vx and Vy, then stores the result
 * in Vx. A bitwise OR compares the corrseponding bits from two values, and
 * if either bit is 1, then the same bit in the result is also 1. Otherwise,
 * it is 0.
 */
void op_8xy1(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    chip8->V[Vx] |= chip8->V[Vy];

}

/**
 * 8xy2 - AND Vx, Vy
 * Set Vx = Vx AND Vy.
 * 
 * Performs a bitwise AND on the values of Vx and Vy, then stores the result
 * in Vx. A bitwise AND compares the corrseponding bits from two values, and
 * if both bits are 1, then the same bit in the result is also 1. Otherwise,
 * it is 0.
 */
void op_8xy2(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    chip8->V[Vx] &= chip8->V[Vy];

}

/**
 * 8xy3 - XOR Vx, Vy
 * Set Vx = Vx XOR Vy.
 * 
 * Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the
 * result in Vx. An exclusive OR compares the corrseponding bits from two values,
 * and if the bits are not both the same, then the corresponding bit in the result
 * is set to 1. Otherwise, it is 0.
 */
void op_8xy3(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    chip8->V[Vx] ^= chip8->V[Vy];

}

/**
 * 8xy4 - ADD Vx, Vy
 * Set Vx = Vx + Vy, set VF = carry.
 * 
 * The values of Vx and Vy are added together. If the result is greater than 8 bits
 * (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result
 * are kept, and stored in Vx.
 */
void op_8xy4(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    uint16_t result = chip8->V[Vx] + chip8->V[Vy];

    chip8->V[0xF] = (result > 0xFF);

    chip8->V[Vx] = result;

}

/**
 * 8xy5 - SUB Vx, Vy
 * Set Vx = Vx - Vy, set VF = NOT borrow.
 * 
 * If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and
 * the results stored in Vx.
 */
void op_8xy5(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    chip8->V[0xF] = (chip8->V[Vx] > chip8->V[Vy]);
    chip8->V[Vx] -= chip8->V[Vy];

}

/**
 * 8xy6 - SHR Vx {, Vy}
 * Set Vx = Vx SHR 1.
 * 
 * If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx
 * is divided by 2.
 */
void op_8xy6(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    chip8->V[0xF] = chip8->V[Vx] & 0x1;
    chip8->V[Vx] >>= 1;
}

/**
 * 8xy7 - SUBN Vx, Vy
 * Set Vx = Vy - Vx, set VF = NOT borrow.
 * 
 * If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and
 * the results stored in Vx.
 */
void op_8xy7(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    chip8->V[0xF] = chip8->V[Vy] > chip8->V[Vx];
    chip8->V[Vx] = chip8->V[Vy] - chip8->V[Vx];
}


/**
 * 8xyE - SHL Vx {, Vy}
 * Set Vx = Vx SHL 1.
 * 
 * If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then
 * Vx is multiplied by 2.
 */
void op_8xyE(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    chip8->V[0xF] = (chip8->V[Vx] & 0x80) >> 7;
    chip8->V[Vx] <<= 1;
}


/**
 * 9xy0 - SNE Vx, Vy
 * Skip next instruction if Vx != Vy.
 * 
 * The values of Vx and Vy are compared, and if they are not equal, the program
 * counter is increased by 2.
 */
void op_9xy0(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode &0x00F0) >> 4;

    if (chip8->V[Vx] != chip8->V[Vy])
        chip8->pc += 2;

}

/**
 * Annn - LD I, addr
 * Set I = nnn.
 * 
 * The value of register I is set to nnn.
 */
void op_Annn(Chip8 *chip8, uint16_t opcode) {

    uint16_t address = opcode & 0x0FFF;
    chip8->I = address;
}

/**
 * Bnnn - JP V0, addr
 * Jump to location nnn + V0.
 * 
 * The program counter is set to nnn plus the value of V0.
 */
void op_Bnnn(Chip8 *chip8, uint16_t opcode) {

    uint16_t address = opcode & 0x0FFF;
    chip8->pc = address + chip8->V[0x0];
}

/**
 * Cxkk - RND Vx, byte
 * Set Vx = random byte AND kk.
 * 
 * The interpreter generates a random number from 0 to 255, which is then ANDed
 * with the value kk. The results are stored in Vx. See instruction 8xy2 for
 * more information on AND.
 */
void op_Cxkk(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t kk = opcode & 0x00FF;

    uint8_t random = rand() % 256;

    chip8->V[Vx] = random & kk;
}

/**
 * Dxyn - DRW Vx, Vy, nibble
 * Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
 * 
 * The interpreter reads n bytes from memory, starting at the address stored in
 * I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
 * Sprites are XORed onto the existing screen. If this causes any pixels to be erased,
 * VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it
 * is outside the coordinates of the display, it wraps around to the opposite side of
 * the screen. See instruction 8xy3 for more information on XOR, and section 2.4,
 * Display, for more information on the Chip-8 screen and sprites.
 */
void op_Dxyn(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);

    uint8_t x = chip8->V[Vx];
    uint8_t y = chip8->V[Vy];
    const uint8_t *sprite = &chip8->memory[chip8->I];

    chip8->V[0xF] = draw_sprite(chip8, x, y, n, sprite);
    chip8->draw_flag = true;
}



/**
 * Ex9E - SKP Vx
 * Skip next instruction if key with the value of Vx is pressed.
 * 
 * Checks the keyboard, and if the key corresponding to the value of Vx is currently in
 * the down position, PC is increased by 2.
 */
void op_Ex9E(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t key = chip8->V[Vx];

    if (is_key_pressed(chip8, key))
        chip8->pc += 2;
}

/**
 * ExA1 - SKNP Vx
 * Skip next instruction if key with the value of Vx is not pressed.
 * 
 * Checks the keyboard, and if the key corresponding to the value of Vx is currently in
 * the up position, PC is increased by 2.
 */
void op_ExA1(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t key = chip8->V[Vx];

    if (!is_key_pressed(chip8, key)) 
        chip8->pc += 2;
    
}




/**
 * Fx07 - LD Vx, DT
 * Set Vx = delay timer value.
 * 
 * The value of DT is placed into Vx.
 */
void op_Fx07(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;

    chip8->V[Vx] = chip8->delay_timer;
}

/**
 * Fx0A - LD Vx, K
 * Wait for a key press, store the value of the key in Vx.
 * 
 * All execution stops until a key is pressed, then the value of that key is stored in Vx.
 */
void op_Fx0A(Chip8 *chip8, uint16_t opcode) {


    uint8_t Vx = (opcode & 0x0F00) >> 8;

    for (uint8_t key = 0; key < KEYPAD_SIZE; ++key) {
        if (chip8->keypad[key]) {
            chip8->V[Vx] = key;
            return;  // Continue to next instruction normally
        }
    }

    // No key pressed: repeat this instruction by not advancing pc
    chip8->pc -= 2;
}

/**
 * Fx15 - LD DT, Vx
 * Set delay timer = Vx.
 * 
 * DT is set equal to the value of Vx.
 */
void op_Fx15(Chip8 *chip8, uint16_t opcode) {
    
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    chip8->delay_timer = chip8->V[Vx];
}


/**
 * Fx18 - LD ST, Vx
 * Set sound timer = Vx.
 * 
 * ST is set equal to the value of Vx.
 */
void op_Fx18(Chip8 *chip8, uint16_t opcode) {

    uint8_t Vx = (opcode & 0x0F00) >> 8;

    chip8->sound_timer = chip8->V[Vx];
}
/**
 * Fx1E - ADD I, Vx
 * Set I = I + Vx.
 * 
 * The values of I and Vx are added, and the results are stored in I.
 */
void op_Fx1E(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    chip8->I = (chip8->I + chip8->V[Vx]) & 0xFFFF; // though I is 16-bit anyway

}

/**
 * Fx29 - LD F, Vx
 * Set I = location of sprite for digit Vx.
 * 
 * The value of I is set to the location for the hexadecimal sprite corresponding to the
 * value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal
 * font.
 */
void op_Fx29(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    uint8_t digit = chip8->V[Vx];

    // Each digit sprite is 5 bytes starting from 0x000
    chip8->I = digit * 5;

}

/**
 * Fx33 - LD B, Vx
 * Store BCD representation of Vx in memory locations I, I+1, and I+2.
 * 
 * The interpreter takes the decimal value of Vx, and places the hundreds digit in
 * memory at location in I, the tens digit at location I+1, and the ones digit at
 * location I+2.
 */
void op_Fx33(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t value = chip8->V[Vx];

    chip8->memory[chip8->I] = value / 100;
    chip8->memory[chip8->I + 1] = (value / 10) % 10;
    chip8->memory[chip8->I + 2] = value % 10;
}

/**
 * Fx55 - LD [I], Vx
 * Store registers V0 through Vx in memory starting at location I.
 * 
 * The interpreter copies the values of registers V0 through Vx into memory, starting at
 * the address in I.
 */
void op_Fx55(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= Vx; i++)
        chip8->memory[chip8->I + i] = chip8->V[i]; 

}

/**
 * Fx65 - LD Vx, [I]
 * Read registers V0 through Vx from memory starting at location I.
 * 
 * The interpreter reads values from memory starting at location I into registers V0
 * through Vx.
 */
void op_Fx65(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= Vx; i++)
        chip8->V[i] = chip8->memory[chip8->I + i];
}
