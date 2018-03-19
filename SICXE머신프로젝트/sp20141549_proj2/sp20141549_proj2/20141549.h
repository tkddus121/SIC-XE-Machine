#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<stdlib.h>
#define MAX_LENGTH 256
#define swap(a,b,t) ((t)=(a),(a)=(b),(b)=(t))
typedef struct _opcode
{
	char opcode[20];
	int hash_val;
	char format[10];
	
	struct _opcode* next;
}opc;

typedef struct symbol_tb
{
	int loc;
	char symbol_name[20];
	struct symbol_tb* next;
}_symbol;

typedef struct _history
{
	char command[MAX_LENGTH];
	struct _history* Next;
}hist;

hist* _hist;
opc* _opcode[20];
_symbol* _sym[20];

typedef struct _Line
{		
	char label[MAX_LENGTH];
	char mnemonic[MAX_LENGTH];
	char state[MAX_LENGTH];
	char remain[MAX_LENGTH];
	int object_code;
	int  loc_cnt;
	int  isComment,remain_flag;
}_line;

unsigned char mem[16][65536];
int num[10],go_out;
int dump_idx,dump_adr,_lstnum;
int _start_flag,_end_flag;
char op_com[10],_filename[MAX_LENGTH];
char input_origin[MAX_LENGTH];
char hi_in[MAX_LENGTH];
int _base;
char _filename_s[MAX_LENGTH];
char _asmfile[MAX_LENGTH];
int _start_ads,_modi_cnt;
int _modafi[MAX_LENGTH],_Error_cnt;
int _error_lines[MAX_LENGTH];

_line _assm[MAX_LENGTH];

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

/****************************
 *
 *	read and print file which name is filename.
 *	if there is no filename, print error.
 *
 * *************************/
void Command_type();

/****************************
 *
 *	Print recent assembled symbol table descending.
 *
 *	line : tab + symbol + address + \n
 *
 * *************************/
void Command_symbol();

/****************************
 *
 *	Make symbol hash table.
 *
 * *************************/
int symbol_hash_func(char* sym);

/****************************
 *
 *	Read asm file. 
 *	And save data.
 *
 * *************************/
int Read_asm_file(char* fname);

/****************************
 *
 *	Analyze assembly code.
 *
 * *************************/
int Analyze_asm(char* asm_str,int numi);

/****************************
 *
 *	Check input string is opcode.
 *
 * *************************/
int find_opcode(char* opcd);

/****************************
 *
 *	Insert symbol in  symbol table.
 *
 * *************************/
void insert_symbol(char* symbol);

/****************************
 *
 *	Fine symbol in  symbol table.
 *
 * *************************/
int find_symbol(char* sym);

/****************************
 *
 *	 Calculate location count.
 *
 * *************************/
void Calculate_loc();

/****************************
 *
 *	Syntax error function.
 *
 * *************************/
void Syntax_error();

/****************************
 *
 *	Returns opcode's format.
 *
 * *************************/
int get_opcode_form(char* opcd);

/****************************
 *
 *	Copy string without first character.
 *
 * *************************/
char* get_pure(char* input);

/****************************
 *
 *	Make list file.	
 *
 * *************************/
void Creat_lst_file(char* filename);

/****************************
 *
 *	Generate object code.
 *
 * *************************/
void gen_object_code();

/****************************
 *
 *	Insert location count to symbol table.
 *
 * *************************/
void loc_to_symtab();

/****************************
 *
 *	returns opcode number.
 *
 * *************************/
int get_opnum(char* opcd);

/****************************
 *
 *	If format is 2, generate object code
 *	from register.
 *
 * **************************/
int object_code_form2(int opnum,char regA,char regB);

/****************************
 *
 *	If format is 3, generate object code
 *	from nixbpe register and displayment.
 *
 * **************************/
int object_code_form34(int opnum,int reg,int disp);

/****************************
 *
 *	Make object file.
 *
 * *************************/
void Creat_obj_file(char* filename);
/****************************
 *
 *	return hex length. 
 *
 * *************************/
int give_hex_length(int hex);

/****************************
 *
 *	get modification
 *
 * *************************/
void modification_find();

/****************************
 *
 *	Read source code and generate obj file and listing file.
 *	
 *	Extension of source is .asm .
 *	Extension of obj file is .obj .
 *	
 *	If there exist error, print error and exit command.
 *	Print where error happened ( line ).
 *
 * *************************/
void Command_assemble();


/****************************
 *
 *	printf if there is error.
 *
 * *************************/
void error_print();

/****************************
 *
 *	insert new error
 *
 * *************************/
void error_occur(int where);
/****************************
 *
 *	symbol free.
 *
 * *************************/
void symbol_free();

