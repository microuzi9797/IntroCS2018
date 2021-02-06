#ifndef FUNCTION
#define FUNCTION
// file.asm list
typedef struct asm_node
{
	char *assembly;
	unsigned int machine_code;
	struct asm_node *next;
}Asm_node;
// symbol-list
typedef struct symbol_node
{
	char *symbol;
	int memory_location;
	struct symbol_node *next;
}Symbol_node;
// initialization
void add_default_symbol(Symbol_node *head);
void add_to_symbol_list(Symbol_node *head, char *string, int value);
// first pass
void buildlist(Asm_node *head, Symbol_node *symbol_head, FILE *fp);
char *readline(FILE* fp);
char *deletecomments(char *string);
char *deletespaces(char *string);
// second pass
void outputlist(Asm_node *head, Symbol_node *symbol_head, FILE *fp);
int findsymbol(Symbol_node *head, char *string);
unsigned int A_instruction(char *assembly);
unsigned int C_instruction(char *assembly);
void make_binary_string(unsigned int input, char *output, int bit_num);
#endif