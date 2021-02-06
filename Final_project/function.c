#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "function.h"

void add_default_symbol(Symbol_node *head)
{
	add_to_symbol_list(head, "R0", 0);
	add_to_symbol_list(head, "R1", 1);
	add_to_symbol_list(head, "R2", 2);
	add_to_symbol_list(head, "R3", 3);
	add_to_symbol_list(head, "R4", 4);
	add_to_symbol_list(head, "R5", 5);
	add_to_symbol_list(head, "R6", 6);
	add_to_symbol_list(head, "R7", 7);
	add_to_symbol_list(head, "R8", 8);
	add_to_symbol_list(head, "R9", 9);
	add_to_symbol_list(head, "R10", 10);
	add_to_symbol_list(head, "R11", 11);
	add_to_symbol_list(head, "R12", 12);
	add_to_symbol_list(head, "R13", 13);
	add_to_symbol_list(head, "R14", 14);
	add_to_symbol_list(head, "R15", 15);
	add_to_symbol_list(head, "SP", 0);
	add_to_symbol_list(head, "LCL", 1);
	add_to_symbol_list(head, "ARG", 2);
	add_to_symbol_list(head, "THIS", 3);
	add_to_symbol_list(head, "THAT", 4);
	add_to_symbol_list(head, "SCREEN", 0x4000);
	add_to_symbol_list(head, "KBD", 0x6000);
}

void add_to_symbol_list(Symbol_node *head, char *string, int value)
{
	Symbol_node *current = head;
	// find if the symbol has already existed
	while(current->next != NULL && strcmp(current->symbol, string) != 0)
	{
		current = current->next;
	}
	// the same value
	if(current->symbol != NULL)
	{
		// calloc new memory
		current->next = calloc(1, sizeof(Symbol_node));
		current = current->next;
	}
	// add symbol
	current->symbol = string;
	current->memory_location = value;
}

void buildlist(Asm_node *head, Symbol_node *symbol_head, FILE *fp)
{
	// beginning
	Asm_node *current = head;
	char *string;
	string = readline(fp);
	unsigned int line = 0;
	// read until the end of the file
	while(string != NULL)
	{
		// delete comments and spaces
		string = deletecomments(string);
		string = deletespaces(string);
		// jump-point or not
		if(string[0] == '(')
		{
			char *symbol;
			symbol = malloc(strlen(string));
			strcpy(symbol, string + 1);
			if(symbol[strlen(symbol) - 1] == ')')
			{
				symbol[strlen(symbol) - 1] = '\0';
			}
			add_to_symbol_list(symbol_head, symbol, line);
		}
		else if(strlen(string) > 1)
		{
			if(current->assembly == NULL)
			{
				current->assembly = string;
			}
			else
			{
				current->next = malloc(sizeof(Asm_node));
				current = current->next;
				current->assembly = string;
			}
			line++;
		}
		// contiue reading
		string = readline(fp);
	}
	current->next = NULL;
}

char *readline(FILE *fp)
{
	int size = 80;
	int offset = 0;
	char *string;
	string = malloc(size);
	// get the string
	while(fgets(string + offset, size - offset, fp))
	{
		// read until the end of the line
		if(strchr(string + offset, '\n'))
		{
			string[strlen(string)] = '\0';
			return string;
		}
	}
	// end
	free(string);
	return ('\0');
}

char *deletecomments(char *string)
{
	const int length = strlen(string);
	int i;
	for(i = 0;i < length - 1;i++)
	{
		// ignore the string after //
		if(string[i] == '/' && string[i + 1] == '/' )
		{
			string[i] = '\0';
			return string;
		}
	}
	return string;
}

char *deletespaces(char *string)
{
	// deal with start
	while(*string && isspace((unsigned char) *string))
	{
		string++;
	}
	int size = strlen(string);
	char *end;
	end = string + size - 1;
	// deal with end
	while(end >= string && isspace((unsigned char) *end))
	{
		end--;
	}
	*(end + 1) = '\0';
	return string;
}

void outputlist(Asm_node *head, Symbol_node *symbol_head, FILE *fp)
{
	Asm_node *current = head;
	char string[17];
	char *symbol;
	unsigned int symbol_count = 0;
	// start
	while(current != NULL)
	{
		// A instruction or C instruction
		if(current->assembly[0] == '@')
		{
			// change symbol to memory-location
			if(!isdigit((int)current->assembly[1]))
			{
				int memory_location = findsymbol(symbol_head, current->assembly + 1);
				// the symbol is lost
				if(memory_location < 0)
				{
					symbol = malloc(strlen(current->assembly));
					strcpy(symbol, current->assembly + 1);
					add_to_symbol_list(symbol_head, symbol, 16 + symbol_count);
					memory_location = 16 + symbol_count;
					symbol_count++;
				}
				sprintf(current->assembly, "@%d", memory_location);
			}
			current->machine_code = A_instruction(current->assembly);
		}
		else
		{
			current->machine_code = C_instruction(current->assembly);
		}
		// change int to binary
		make_binary_string(current->machine_code, string, 16);
		fprintf(fp, "%s\n", string);
		current = current->next;
	}
}

int findsymbol(Symbol_node *head, char *string)
{
	Symbol_node *current = head;
	while(current != NULL)
	{
		if(strcmp(current->symbol, string) == 0)
		{
			return (current->memory_location);
		}
		current = current->next;
	}
	return -1;
}

unsigned int A_instruction(char *assembly)
{
	// A instruction : 0vvv vvvv vvvv vvvv
	unsigned int ans = 0b0;
	if(assembly[0] == '@')
	{
		assembly++;
	}
	unsigned int assembly_num = (unsigned int)strtol(assembly, NULL, 10);
	ans |= (0b0 << 15);
	ans |= assembly_num;
	return ans;
}

unsigned int C_instruction(char *assembly)
{
	// C instruction : 111a cccc ccdd djjj
	unsigned int ans = 0b0;
	char *comp = NULL;
	char *dest = NULL;
	char *jump = NULL;
	unsigned int a = 0b0;
	unsigned int compnum = 0b0;
	unsigned int destnum = 0b0;
	unsigned int jumpnum = 0b0;
	if(strchr(assembly, ';') != NULL)
	{
		comp = strtok(assembly, ";");
		jump = strtok(NULL, ";");
	}
	if(strchr(assembly, '=') != NULL)
	{
		dest = strtok(assembly, "=");
		comp = strtok(NULL, "=");
	}
	// a
	char *M_position;
	M_position = strchr(comp, 'M');
	if(M_position != NULL)
	{
		a = 0b1;
		*(M_position) = 'A';
	}
	// comp
	if(strcmp(comp, "0") == 0)
	{
		compnum = 0b101010;
	}
	else if(strcmp(comp, "1") == 0)
	{
		compnum = 0b111111;
	}
	else if(strcmp(comp, "-1") == 0)
	{
		compnum = 0b111010;
	}
	else if(strcmp(comp, "D") == 0)
	{
		compnum = 0b001100;
	}
	else if(strcmp(comp, "A") == 0)
	{
		compnum = 0b110000;
	}
	else if(strcmp(comp, "!D") == 0)
	{
		compnum = 0b001101;
	}
	else if(strcmp(comp, "!A") == 0)
	{
		compnum = 0b110001;
	}
	else if(strcmp(comp, "-D") == 0)
	{
		compnum = 0b001111;
	}
	else if(strcmp(comp, "-A") == 0)
	{
		compnum = 0b110011;
	}
	else if(strcmp(comp, "D+1") == 0)
	{
		compnum = 0b011111;
	}
	else if(strcmp(comp, "A+1") == 0)
	{
		compnum = 0b110111;
	}
	else if(strcmp(comp, "D-1") == 0)
	{
		compnum = 0b001110;
	}
	else if(strcmp(comp, "A-1") == 0)
	{
		compnum = 0b110010;
	}
	else if(strcmp(comp, "D+A") == 0)
	{
		compnum = 0b000010;
	}
	else if(strcmp(comp, "D-A") == 0)
	{
		compnum = 0b010011;
	}
	else if(strcmp(comp, "A-D") == 0)
	{
		compnum = 0b000111;
	}
	else if(strcmp(comp, "D&A") == 0)
	{
		compnum = 0b000000;
	}
	else if(strcmp(comp, "D|A") == 0)
	{
		compnum = 0b010101;
	}
	else
	{
		compnum = 0b101010;
	}
	// dest
	if(dest != NULL)
	{
		if(strchr(dest, 'A') != NULL)
		{
			destnum |= (1 << 2);
		}
		if(strchr(dest, 'D') != NULL)
		{
			destnum |= (1 << 1);
		}
		if(strchr(dest, 'M') != NULL)
		{
			destnum |= 1;
		}
	}
	// jump
	if(jump != NULL)
	{
		if(strcmp(jump, "JGT") == 0)
		{
			jumpnum = 0b001;
		}
		else if(strcmp(jump, "JEQ") == 0)
		{
			jumpnum = 0b010;
		}
		else if(strcmp(jump, "JGE") == 0)
		{
			jumpnum = 0b011;
		}
		else if(strcmp(jump, "JLT") == 0)
		{
			jumpnum = 0b100;
		}
		else if(strcmp(jump, "JNE") == 0)
		{
			jumpnum = 0b101;
		}
		else if(strcmp(jump, "JLE") == 0)
		{
			jumpnum = 0b110;
		}
		else if(strcmp(jump, "JMP") == 0)
		{
			jumpnum = 0b111;
		}
		else
		{
			jumpnum = 0b000;
		}
	}
	// combine
	ans |= (0b111 << 13);
	ans |= (a << 12);
	ans |= (compnum << 6);
	ans |= (destnum << 3);
	ans |= jumpnum;
	return ans;
}

void make_binary_string(unsigned int input, char *output, int bit_num)
{
	int value;
	int location;
	output[bit_num] = '\0';
	int i;
	for(i = bit_num - 1;i >= 0;i--)
	{
		value = input >> i;
		location = bit_num - i - 1;
		output[location] = (value & 1) + '0';
	}
}