#include "20141549.h"

/******************************
 *	
 *	parameter : input string
 *	return : X
 *
 *	Read commands and check it is approprite.
 *	And run that command.
 *
 *	If there is no command, ignore it.
 *
 * ***************************/
void CommandRead(char* input)
{	


	if( strcmp(input, "help") == 0 || strcmp(input, "h") == 0 )
	{
		Command_help();
	}

	if( strcmp(input, "dir") == 0 || strcmp(input, "d") == 0 )
	{	
		Command_dir();
	}

	if( strcmp(input, "quit") == 0 || strcmp(input,"q") == 0 )
	{
		Command_quit();
	}

	if( strcmp(input, "history") == 0 || strcmp(input, "hi") == 0 )
	{
		alloc_mem(input_origin);
		Command_history();
	}

	if( strcmp(input, "dump") == 0 || strcmp(input, "du") == 0 )
	{
		Command_dump();
	}

	if( strcmp(input, "edit") == 0 || strcmp(input, "e") == 0 )
	{
		Command_edit();
	}

	if( strcmp(input, "fill") == 0 || strcmp(input, "f") == 0 )
	{
		Command_fill();
	}

	if( strcmp(input, "reset") == 0  )
	{
		Command_reset();
	}

	if( strcmp(input, "opcodelist") == 0  )
	{

		Command_opcodelist();
	}

	if( strcmp(input, "opcode") == 0  )
	{
		Command_opcode_mnemonic(op_com);
	}

	if( strcmp(input, "nope") == 0 ||  strcmp(input, "hi") == 0 ||  strcmp(input, "history") == 0  )
		;
	else
		alloc_mem(input_origin);

}


/****************************
 *
 *  allocate history memory.
 *
 * **************************/
void alloc_mem(char* newCommand)
{

	hist* new_hist = (hist*)malloc(sizeof(hist));

	strcpy(new_hist->command, newCommand);

	new_hist->Next = NULL;
	if( _hist == NULL )
	{
		_hist = new_hist;
	}
	else
	{
		hist* tmp = _hist;

		while(tmp->Next != NULL)
		{
			tmp = tmp->Next;
		}
		tmp->Next = new_hist;
	}
	
}


/****************************
 *
 *	get command and tokenize it.	
 *
 * **************************/
char* get_commands()
{
	char input[MAX_LENGTH] = {0};
	char* toke_str = NULL;
	int i = 0,j,k,l,coma_cnt = 0;
	char* cmd = (char*)malloc(MAX_LENGTH);

	input_origin[0] = '\0';

	printf("sicsim> ");
	gets(input);

	strcpy(input_origin,input);


	if(input[0]==',')
		return "ER";

	if(input[0] == '\0' || strcmp(input," ") == 0 || strcmp(input,"\t") == 0 )
	{
		return "nope";
	}


	for(i = 0 ; i < strlen(input);i++)
	{	
		if(input[i] == ' ' || input[i] == '\t' || input[i] == ',' )
		{
			continue;
		}
		else 
			break;

	}

	for(k = 0 ; k < strlen(input);k++)
	{
		if(input[k] == ',')
			coma_cnt++;

		if(input[k] == ',' && input[k-1] == ',')
			return "ER";
	}


	if(i == (int)strlen(input))
	{
		return "nope";
	}

	toke_str = strtok(input," ,\t");
	
	strcpy(cmd,toke_str);

	if(cmd == '\0')
		return "nope";

	for(j = 0 ; j < 10 ;j++)
		num[j] = 0;


	if(strcmp(cmd,"opcode") == 0)
	{

		toke_str = strtok(NULL," ,\t");   
		if(toke_str == NULL || coma_cnt > 0)
			return "ER";

		strcpy(op_com,toke_str);

		return cmd;
	}

	while(toke_str != NULL)
	{
		sscanf(toke_str,"%x",&num[i++]);

		if(strcmp(cmd,toke_str) != 0)
		for(l = 0 ; l < strlen(toke_str);l++)
		{
			if( (toke_str[l] >= '0' && toke_str[l] <= '9') || 
				(toke_str[l] >= 'a' && toke_str[l] <= 'f') || 
				(toke_str[l] >= 'A' && toke_str[l] <='F'))
				;
			else
				return "ER";


		}
		toke_str = strtok(NULL," ,");
	}


	if( (strcmp(cmd,"f") == 0 || strcmp(cmd,"fill") == 0) && (i != 4  || coma_cnt !=2 ))
		return "ER";

	if( (strcmp(cmd,"du") == 0 || strcmp(cmd,"dump") == 0) && i>3 )
		return "ER";

	if( (strcmp(cmd,"e") == 0 || strcmp(cmd,"edit") == 0) && (i != 3 || coma_cnt != 1))
		return "ER";

	//if it is not number
	if(num[1]< 0 || num[1] >= 0x100000 ||
	   num[2]< 0 || num[2] >= 0x100000 ||
	   num[3]< 0 || num[3] >= 0x100000 )
		return "ER";

	if(  (strcmp(cmd,"du") == 0 || strcmp(cmd,"dump") == 0) )
	{
		dump_idx = i;
	
		if(i == 3 && (num[1] > num[2] || coma_cnt != 1))
			return "ER";
		else if(i<3 && coma_cnt >=1)
			return "ER";
	}


	if( (strcmp(cmd,"du") == 0 || strcmp(cmd,"dump") == 0) ||
		(strcmp(cmd,"e") == 0 || strcmp(cmd,"edit") == 0) ||
		(strcmp(cmd,"f") == 0 || strcmp(cmd,"fill") == 0)
	  )
		;
	else if(i > 1)
		return "ER";


	if(strcmp(cmd,"h") == 0 || strcmp(cmd,"help") == 0 
			||strcmp(cmd,"d") == 0 ||strcmp(cmd,"dir") == 0 
			||strcmp(cmd,"q") == 0 ||strcmp(cmd,"quit") == 0 
			||strcmp(cmd,"hi") == 0 ||strcmp(cmd,"history") == 0 
			||strcmp(cmd,"du") == 0 ||strcmp(cmd,"dump") == 0 
			||strcmp(cmd,"e") == 0 ||strcmp(cmd,"edit") == 0 
			||strcmp(cmd,"f") == 0 ||strcmp(cmd,"fill") == 0 
			||strcmp(cmd,"reset") == 0 
			||strcmp(cmd,"opcode") == 0 
			||strcmp(cmd,"opcodelist") == 0)
	{
		return cmd;
	}
	

	return "ER";

}
/**********************
 *
 *	Read opcode from opcode.txt
 *
 * *******************/
void get_opcode()
{
	FILE* fp = fopen("opcode.txt", "r");
	char in_line[MAX_LENGTH]={0};
	int hash_value;
	
	while(fgets(in_line,sizeof(in_line), fp) != NULL)
	{
		char mnemonic[MAX_LENGTH],sss[100];
		int opcode;
		opc* news = NULL;
		opc* tmps = NULL;

		sscanf(in_line,"%X %s %s",&opcode,mnemonic,sss);

		news =  (opc*)malloc(sizeof(opc));
		strcpy(news->opcode,mnemonic);
		news->hash_val = opcode;
		news->next = NULL;

		hash_value = opcode_hash_func(mnemonic);
		tmps = _opcode[hash_value];

	
		if( tmps == NULL)
		{
			_opcode[hash_value] = news;
		}
		else
		{

			for(;;)
			{
				if(tmps->next == NULL )
					break;
				tmps = tmps->next;
			}
			tmps->next = news;

		}

	}

	fclose(fp);

}

/****************************
 *
 *	Hash function returns opcode's hash.
 *
 * **************************/
int opcode_hash_func(char* code)
{
	unsigned int i,hash = 0,len = strlen(code);
	for(i = 0 ; i < len ; i++)
	{
		hash = hash + code[i]*42.306992;//42.306992 , 522.3075561523437500
	}

	return (hash%20);

}

/****************************
 *
 *	free allocated memories.
 *
 * *************************/
void mem_free()
{
	int i;
	for(i = 0 ;i < 20 ;i++)
	{
		while(_opcode[i] != NULL)
		{
			opc* tmp = _opcode[i];
			_opcode[i] = _opcode[i]->next;
			free(tmp);
		}
	}

}

