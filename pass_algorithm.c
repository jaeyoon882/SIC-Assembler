#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "assembler.h"
#define MAX_LEN 256
//parsing
regex_t regex;
const char *pattern = "^([A-Za-z0-9&]*)[ \t]+([A-Za-z0-9&]+)([ \t]+([^\n\r]*))?";

char currentLine[MAX_LEN];

int LOCCTR = 0;

void parseLine(char *line) {
    regmatch_t pmatch[5];
    label[0] = opcode[0] = operand[0] = '\0';

    if (regexec(&regex, line, 5, pmatch, 0) == 0) {
        //label
        if (pmatch[1].rm_so != -1) {
            int len = pmatch[1].rm_eo - pmatch[1].rm_so;
            strncpy(label, line + pmatch[1].rm_so, len);
            label[len] = '\0';
        }
        //opcode
        if (pmatch[2].rm_so != -1) {
            int len = pmatch[2].rm_eo - pmatch[2].rm_so;
            strncpy(opcode, line + pmatch[2].rm_so, len);
            opcode[len] = '\0';
        }
        //operand
        if (pmatch[4].rm_so != -1) {
            int len = pmatch[4].rm_eo - pmatch[4].rm_so;
            strncpy(operand, line + pmatch[4].rm_so, len);
            operand[len] = '\0';
        }
    }
}

void pass1() {
	FILE *source = fopen("SRCFILE.asm","r");
	
	if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        	perror("regex error\n");
        	return;
    	}	

	if(source==NULL) {
		perror("failed to open source file\n");
		return;
	}
	while(feof(source)==0) {
		fgets(currentLine,MAX_LEN,source);
		currentLine[strlen(currentLine)-1]='\0';
		parseLine(currentLine);
		
		if(strcmp(opcode,"START")==0) {
			startingAddr = atoi(operand);
			LOCCTR = atoi(operand);
			continue; //read next line
		}		

		if(strcmp(opcode,"END")==0) {
			programLength = LOCCTR - startingAddr;
			break;
		}

		if(currentLine[0]!='.') {
			if(label[0]!='\0') {
				if(findSymbol(label) != -1) {
					perror("duplicate symbol\n");
					return;
				}
				else {
					insertSymbol(label,LOCCTR);
				}
			}
			
			if(strcmp(opcode,"WORD")==0) LOCCTR += 3;
			else if(strcmp(opcode,"RESW")==0) LOCCTR += 3*atoi(operand);
			else if(strcmp(opcode,"BYTE")==0) {
				int len = strlen(operand); 
    				if (operand[0] == 'C') {
        				LOCCTR += (len - 3); 
				}
    				else if (operand[0] == 'X') {
        				LOCCTR += (len - 3) / 2;
				}
			}
			else if(strcmp(opcode,"RESB")==0) LOCCTR += atoi(operand);
			else if(findOpcode(opcode)!=-1) LOCCTR += 3;
			else {
				perror("pass1 error - opcode error");
				return;
			}	

		} 
	}
	
	fclose(source);
}

char t_record_buffer[80];
int t_record_length;
int t_starting_address;

void write_t_record(FILE* fp) {
	if(t_record_length==0) return;
	fprintf(fp,"T%06X%02X%s\n",t_starting_address,t_record_length,t_record_buffer);
	t_record_buffer[0]='\0';
	t_record_length = 0;
	t_starting_address = -1;
}


void generate_instruction_objcode(int opcode,int address,char *obj_str,int x_indexed) {
	int obj_code;
	obj_code = opcode<<16;
	if(x_indexed) obj_code |= 1 << 15;
	obj_code |= address;
	sprintf(obj_str,"%06X",obj_code);
}

int generate_data_objcode(char *opcode,char *operand,char *obj_str) {
	int obj_code = -1;
	int obj_len = 0;
	if(strcmp(opcode,"WORD")==0) {
		obj_code = atoi(operand);
		sprintf(obj_str,"%06X",obj_code&0xFFFFFF);
		obj_len = 3;
	}

	else if(strcmp(opcode,"BYTE")==0) {
		char pure_constant[20];
		strncpy(pure_constant,operand+2,strlen(operand+2)-1);
		if(operand[0]=='C') {
			char temp[3];
			for(int i=0;i<strlen(pure_constant);i++) {
				sprintf(temp,"%02X",pure_constant[i]);
				strcat(obj_str,temp);
			}
			obj_len = strlen(pure_constant);

		}
		else if(operand[0]=='X'){
			strcpy(obj_str,pure_constant);
			obj_str[strlen(pure_constant)]='\0';
			obj_len = (strlen(pure_constant)+1)/2;
		}
	}
	return obj_len;
}




void pass2() {
	FILE *source = fopen("SRCFILE.asm","r");
	FILE *object = fopen("OBJFILE","w");
	LOCCTR=0;

	if(source==NULL || object==NULL) {
		perror("failed to open file\n");
		return;
	}

	while(fgets(currentLine,MAX_LEN,source)!=NULL) {
		currentLine[strlen(currentLine)-1] = '\0';
		parseLine(currentLine);

		int obj_len = 0;
		char obj_str[30];
		obj_str[0] = '\0';

		if(strcmp(opcode,"START")==0) {
			//write header line
			fprintf(object,"H%-6s%06X%06X\n",label,startingAddr,programLength);
			t_starting_address = startingAddr;
			LOCCTR = startingAddr;
			continue;
		}

		//write text line
		if(findOpcode(opcode)!=-1) {
			int opcode_hex = findOpcode(opcode);
			if(operand[0]!='\0') {
				//search symtab for operand
				//check if ",X" exists
				//if found then store symbol values as operand address
				char *comma = strchr(operand,',');
				if(comma) {
					char pureSymbol[20];
					int len = comma - operand;
					strncpy(pureSymbol,operand,len);
					pureSymbol[len]='\0';
					int address = findSymbol(pureSymbol);
					if(address==-1) address=0;
					generate_instruction_objcode(opcode_hex,address,obj_str,1);

				}
				else {
					int address = findSymbol(operand);
					if (address == -1) address = 0;
					generate_instruction_objcode(opcode_hex,address,obj_str,0);

				}
			}
			else {
				generate_instruction_objcode(opcode_hex,0,obj_str,0);
			}
			obj_len=3;
		}
		else if(strcmp(opcode,"BYTE")==0 || strcmp(opcode,"WORD")==0) {
			//convert constant to object code
			obj_len=generate_data_objcode(opcode,operand,obj_str);
		}

		else if(strcmp(opcode,"RESB")==0 || strcmp(opcode,"RESW")==0) {
			write_t_record(object);

		}

		if(strcmp(opcode,"END")==0) {
			//write final text line
			write_t_record(object);
			fprintf(object,"E%06X\n",startingAddr);
			break;
		}

		if(obj_len>0) {
			if(t_record_length + obj_len > 30) {
				write_t_record(object);
				strcpy(t_record_buffer,obj_str);
				t_record_length = obj_len;
				t_starting_address = LOCCTR;
			}
			else {
				if(t_record_length==0) t_starting_address = LOCCTR;

				strcat(t_record_buffer,obj_str);
				t_record_length += obj_len;
			}
		}

		if(strcmp(opcode, "WORD") == 0) LOCCTR += 3;
        	else if(strcmp(opcode, "RESW") == 0) LOCCTR += 3 * atoi(operand);
        	else if(strcmp(opcode, "RESB") == 0) LOCCTR += atoi(operand);
        	else if(strcmp(opcode, "BYTE") == 0) {
                	if(operand[0] == 'C') LOCCTR += strlen(operand) - 3;
             		else if(operand[0] == 'X') LOCCTR += (strlen(operand) - 3) / 2;
        	}
        	else LOCCTR += 3;

	}
	fclose(source);
	fclose(object);

}

