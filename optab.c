#include <stdio.h>
#include <string.h>
#include "assembler.h"


const int OPTAB_SIZE = 26;

const OptabEntry OPTAB[] = {
    { "ADD",  0x18 },
    { "SUB",  0x1C },
    { "MUL",  0x20 },
    { "DIV",  0x24 },

    { "LDA",  0x00 }, // Load Register A
    { "LDX",  0x04 }, // Load Register X
    { "LDL",  0x08 }, // Load Register L
    { "STA",  0x0C }, // Store Register A
    { "STX",  0x10 }, // Store Register X
    { "STL",  0x14 }, // Store Register L
    { "LDCH", 0x50 }, // Load Character to A (Rightmost byte)
    { "STCH", 0x54 }, // Store Character from A (Rightmost byte)

    { "COMP", 0x28 }, // Compare
    { "J",    0x3C }, // Unconditional Jump
    { "JEQ",  0x30 }, // Jump if Equal
    { "JGT",  0x34 }, // Jump if Greater than
    { "JLT",  0x38 }, // Jump if Less than
    { "JSUB", 0x48 }, // Jump to Subroutine
    { "RSUB", 0x4C }, // Return from Subroutine

    { "AND",  0x40 },
    { "OR",   0x44 },
    { "TIX",  0x2C }, // Test Index Register (Compare X and increment)

    { "RD",   0xD8 }, // Read Device
    { "TD",   0xE0 }, // Test Device
    { "WD",   0xDC }  // Write Device
};

int findOpcode(char* mnemonic) {
	for(int i=0;i<OPTAB_SIZE;i++) {
		if(strcmp(OPTAB[i].mnemonic,mnemonic)==0) {
			return OPTAB[i].opcode;
		}
	}
	perror("opcode not found\n");
	return -1;
}

