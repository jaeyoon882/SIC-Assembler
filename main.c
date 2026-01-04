#include <stdio.h>
#include "assembler.h"
int startingAddr = 0;
int programLength = 0;

char label[30];
char opcode[30];
char operand[100];

int main() {
	pass1();
	pass2();
	printf("build successful\n");
	return 1;
}
