#include <stdio.h>
#include <string.h>
#include "assembler.h"
#define MAX_SYMTAB_SIZE 1000

int SymtabSize = 0;


SymtabEntry SYMTAB[MAX_SYMTAB_SIZE];

int findSymbol(char* label) {
	for(int i=0;i<SymtabSize;i++) {
		if(strcmp(SYMTAB[i].symbol,label)==0) {
			return SYMTAB[i].address;
		}
	}
	return -1;
}

void insertSymbol(char *label,int address) {
	SYMTAB[SymtabSize].address = address;
	strcpy(SYMTAB[SymtabSize].symbol,label);
	SymtabSize++;
}

