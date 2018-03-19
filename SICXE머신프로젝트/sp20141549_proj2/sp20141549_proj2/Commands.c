#include "20141549.h"

/****************************
 *	
 *	Run h[elp] command.
 *
 *	Print all commands.
 *	
 * **************************/

void Command_help()
{

	printf("\th[elp]\n");
	printf("\td[ir]\n");
	printf("\tq[uit]\n");
	printf("\thi[story]\n");
	printf("\tdu[mp] [start, end]\n");
	printf("\te[dit] address, value\n");
	printf("\tf[ill] start, end, value\n");
	printf("\treset\n");
	printf("\topcode mnemonic\n");
	printf("\topcodelist\n");
	printf("\tassemble filename\n");
	printf("\ttype filename\n");
	printf("\tsymbol\n");

}

/****************************
 *
 *	Run q[uit] command.
 *
 *	Exit the program. 
 *
 * **************************/
void Command_quit()
{
	go_out = 1;
	mem_free();
	symbol_free();
}


/****************************
 *
 *	Run d[ir] command.
 *
 *	Print file and directories in current directory.
 *
 * **************************/
void Command_dir()
{
	DIR* dp = NULL;
	struct dirent* entry = NULL;
	struct stat buf;
	int i = 0;

	dp = opendir(".");

	while( (entry = readdir(dp)) != NULL )
	{
		lstat(entry->d_name, &buf);



		if( strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
			continue;

		if( S_ISDIR(buf.st_mode))
			printf("%25s/",entry->d_name);
		else if( (buf.st_mode & S_IEXEC) == S_IEXEC)
			printf("%25s*",entry->d_name);
		else
			printf("%25s",entry->d_name);
		i++;
		if(i == 4)
		{	
			puts("");
			i = 0;
		}
	}

	printf("\n");

	closedir(dp);
}


/****************************
 *
 *	Run hi[story] command.
 *
 *	Print all commands history.
 *
 * **************************/
void Command_history()
{
	hist* tmp = _hist;
	int cnt = 1;
	while(1)
	{
		printf("\t%d %s\n",cnt++,tmp->command);

		if(tmp->Next == NULL)
			break;
		tmp = tmp->Next;
	}

}


/****************************
 *
 *	Run du[mp] [start, end] command.
 *
 *	Print allocated memories start to end.
 *
 *	if there is no start and end print 10 lines.
 *
 * **************************/
void Command_dump()
{
	int start,end,i,j,i_buf;
	
	if(dump_idx == 1)
	{
		start = dump_adr;
		end = start+159;
	}
	else if(dump_idx == 2)
	{
		start = num[1];
		end = start + 159;
	}
	else if(dump_idx == 3)
	{
		start = num[1];
		end = num[2];
	}
	else
	{
		printf("Wrong Command!");
		return;
	}

	if(start > end% 1048576 )
		dump_adr = 0;
	else
		dump_adr = end+1;

	for(i = start - start%16 ; i <= end ;)
	{

		if(i % 16 == 0)
			printf("\t%05X  ",i);

		i_buf = i;

		for(j = 0 ; j < 16 ;j ++)
		{
			if(i < start || i> end)
				printf("   ");
			else
				printf("%02X ",mem[i%16][i/16]);
			i++;
			if(i > 1048575)
			{
				if(i == 1048575)
					dump_adr = 0;
				else
					dump_adr %= 1048576;
				break;
			}
		}

		printf("; ");

		i = i_buf;
		for(j = 0 ; j < 16 ;j++)
		{
			if(mem[i%16][i/16] < 32 || mem[i%16][i/16] > 126)
				printf(".");
			else
				printf("%c",mem[i%16][i/16]);
			i++;
		}
		
		puts("");	
		
		if(i > 1048575)
		{
			if( i == 1048575)
				dump_adr = 0;
			else
				dump_adr %= 1048576;
			break;
		}

	}


	dump_idx = 0;

}


/****************************
 *
 *	Run e[dit] address, input_value command.
 *
 *	Change value of address to input_value.
 *
 * **************************/
void Command_edit()
{
	int ads = num[1];
	mem[ads%16][ads/16] = num[2];
}


/****************************
 *
 *	Run f[ill] start, end, input_value command.
 *
 *	Change values of start to end address.
 *
 * **************************/
void Command_fill()
{
	int start = num[1], 
		end = num[2], 
		val = num[3];

	int i;
	for(i = start ; i <= end; i++)
	{
		mem[i%16][i/16] = val;
	}
}

/****************************
 *
 *	Run reset command.
 *
 *	Change all memories 0.
 *
 * **************************/
void Command_reset()
{
	memset(mem,0,sizeof(mem));
}


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
void Command_opcodelist()
{
	int i;

	for(i = 0 ; i < 20 ; i ++)
	{
		opc* tmp = _opcode[i]; 
		printf("\t%d : ",i);
		
		while(tmp != NULL)
		{
			printf("[%s,%X]",tmp->opcode,tmp->hash_val);

			if(tmp->next !=NULL)
				printf(" -> ");
			tmp = tmp->next;
		}

		puts("");
	}

}

/****************************
 *
 *	Run opcode [opcode] command.
 *
 *	print [opcode]'s opcode.
 *
 * **************************/
void Command_opcode_mnemonic(char* input)
{
	int hash_value = opcode_hash_func(input);
	opc* tmp = _opcode[hash_value];

	while(tmp != NULL)
	{
		if(strcmp(tmp->opcode,input) == 0)
		{
			printf("\topcode is %X\n",tmp->hash_val);
			op_com[0] = '\0';
			return;
		}
		tmp = tmp->next;

	}
	printf("\tInvalid opcode!\n");

}

/****************************
 *
 *	read and print file which name is filename.
 *	if there is no filename, print error.
 *
 * *************************/
void Command_type()
{
	
	FILE *fp;
	char content[MAX_LENGTH] = {0};
	//filename doesnt exist

	if( (fp = fopen(_filename_s,"r")) == NULL)
	{
		printf("Unvalid filename\n");
		return;
	}

	while(fgets(content,sizeof(content),fp) != NULL)
	{
		printf("\t%s",content);
		
		if(content[0] == '.')
			continue;
	}
	
	_filename_s[0] = '\0';

	puts("");

	fclose(fp);

}


