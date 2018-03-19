#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<stdlib.h>

#define MAX_LENGTH 256

typedef struct _opcode
{
	char opcode[20];
	int hash_val;
	struct _opcode* next;
}opc;

typedef struct _history
{
	char command[MAX_LENGTH];
	struct _history* Next;
}hist;

hist* _hist;
opc* _opcode[20];



unsigned char mem[16][65536];
int num[10],go_out;
int dump_idx,dump_adr;
char op_com[10];
char input_origin[MAX_LENGTH];

/******************************
 *	
 *	parameter : input string
 *	return : X
 *
 *	Read commands and check it is approprite.
 *	And run that command.
 *
 * ***************************/
void CommandRead(char* input);


/****************************
 *	
 *	Run h[elp] command.
 *
 *	Print all commands.
 *	
 * **************************/
void Command_help();


/****************************
 *
 *	Run d[ir] command.
 *
 *	Print file and directories in current directory.
 *
 * **************************/
void Command_dir();


/****************************
 *
 *	Run q[uit] command.
 *
 *	Exit the program. 
 *
 * **************************/
void Command_quit();


/****************************
 *
 *	Run hi[story] command.
 *
 *	Print all commands history.
 *
 * **************************/
void Command_history();


/****************************
 *
 *	Run du[mp] [start, end] command.
 *
 *	Print allocated memories start to end.
 *
 *	if there is no start and end print 10 lines.
 *
 * **************************/
void Command_dump();


/****************************
 *
 *	Run e[dit] address, input_value command.
 *
 *	Change value of address to input_value.
 *
 * **************************/
void Command_edit();


/****************************
 *
 *	Run f[ill] start, end, input_value command.
 *
 *	Change values of start to end address.
 *
 * **************************/
void Command_fill();


/****************************
 *
 *	Run reset command.
 *
 *	Change all memories 0.
 *
 * **************************/
void Command_reset();


/****************************
 *
 *	Run opcode [opcode] command.
 *
 *	print [opcode]'s opcode.
 *
 * **************************/
void Command_opcode_mnemonic(char* input);


/****************************
 *
 *	Run opcodelist command.
 *
 *	print opcode Hash Table.
 *
 *	(ex) 0 : [ADD,18] -> [JEQ,30]
 *		 1 : [STS,7C] -> [LDS,6C] -> [JEQ,30]
 *						…
 *		 19 : [LPS,D0]
 *
 * **************************/
void Command_opcodelist();


/****************************
 *
 *  allocate history memory.
 *
 * **************************/
void alloc_mem(char* newCommand);


/****************************
 *
 *	get command and tokenize it.	
 *
 * **************************/
char* get_commands();


/****************************
 *
 *	Hash function returns opcode's hash.
 *
 * **************************/
int opcode_hash_func(char* code);

/****************************
 *
 *	get opcode from opcode.txt.
 *	
 * **************************/
void get_opcode();

/****************************
 *
 *	free allocated memories.
 *
 * *************************/
void mem_free();
