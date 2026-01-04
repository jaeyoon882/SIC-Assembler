#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <stdio.h>
#include <regex.h>

extern const int OPTAB_SIZE;
extern int programLength;

typedef struct {
	char mnemonic[7];
	int opcode;
}OptabEntry;

typedef struct {
	char symbol[10];
	int address;
}SymtabEntry;

extern const OptabEntry OPTAB[];


int findOpcode(char *mnemonic);

int findSymbol(char *label);

void insertSymbol(char *label, int address);

void pass1();

void parseLine(char *line);

void pass2();

extern regex_t regex;
extern const char *pattern;
extern int startingAddr;
extern int programLength;
extern char label[30];
extern char opcode[30];
extern char operand[100];
#endif
