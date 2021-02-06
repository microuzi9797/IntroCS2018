#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function.h"

int main(int argc, char *argv[])
{
	char *filename;
	filename = malloc(strlen(argv[1] + 1));
	strcpy(filename, argv[1]);
	// open th file
	FILE *file_ptr;
	file_ptr = fopen(filename, "r");
	// read the file and build file-list
	Asm_node *asm_head;
	asm_head = calloc(1, sizeof(Asm_node));
	Symbol_node *symbol_head;
	symbol_head = calloc(1, sizeof(Symbol_node));
	add_default_symbol(symbol_head);
	buildlist(asm_head, symbol_head, file_ptr);
	fclose(file_ptr);
	// change .asm to .hack
	char *dot;
	dot = strrchr(filename, '.');
	*dot = '\0';
	filename = realloc(filename, strlen(filename) + 6);
	filename = strcat(filename, ".hack");
	// write the file
	file_ptr = fopen(filename, "w");
	outputlist(asm_head, symbol_head, file_ptr);
	// end
	free(filename);
	fclose(file_ptr);
	return 0;
}