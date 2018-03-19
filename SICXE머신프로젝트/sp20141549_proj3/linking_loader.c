#include "20141549.h"

/***************************
 *
 *  insert external symbol to symbol table.
 *  
 * ************************/
void insert_exsymbol(char* symbol)
{
	int hash_value;

	_ex_symbol* new = NULL;
	_ex_symbol* tmp = NULL;
	
	new = (_ex_symbol*)malloc(sizeof(_ex_symbol));
	strcpy(new->symbol_name,symbol);
	new->next = NULL;

	hash_value = symbol_hash_func(symbol);
	tmp = _exsym[hash_value];

	if( tmp == NULL )
	{
		_exsym[hash_value] = new;
	}
	else
	{
		for(;;)
		{
			if(tmp->next == NULL)
				break;
			tmp = tmp->next;
	
		}
		tmp->next = new;
	}


}
/***************************
 *
 *  Find external symbol from symbol table
 *
 * ************************/
int find_exsymbol(char* sym)
{
	int hash_value = symbol_hash_func(sym);
	_ex_symbol* tmp = _exsym[hash_value];

	while(tmp != NULL)
	{
		if(strcmp(tmp->symbol_name,sym) == 0)
		{
			return tmp->addr;
		}
		tmp = tmp->next;
	}

	return -1;

}

/***************************
 *
 *  Assigns addresses to all external symbols
 * w
 *
 * ************************/
int linking_loader_pass1(char* filename,int f_count)
{
	FILE *fp;
	int line_count = 0;
	char new_line[MAX_LENGTH];
	
	
	if( (fp = fopen(filename,"r")) == NULL)
		return -1;// if there is no filename return -1.


	while( fgets( new_line, sizeof(new_line),fp) != NULL)
	{
		strcpy(_obj_file[f_count].lines[line_count] , new_line);
		line_count++;
	}

	_obj_file[f_count].line_num = line_count;

	sscanf(_obj_file[f_count].lines[0],"H%s %6x %6x",_obj_file[f_count].Hrecord,&_obj_file[f_count].start_addr,&_obj_file[f_count].length);

	if( find_exsymbol(_obj_file[f_count].Hrecord) != -1)
	{
		return 1;// overlapped symbol.
	}
	sscanf(_obj_file[f_count].lines[1],"D%s %6x%s %x",_obj_file[f_count].Drecord,&_obj_file[f_count].ind_addr,_obj_file[f_count].Erecord,&_obj_file[f_count].end_addr);

	if( find_exsymbol( _obj_file[f_count].Drecord ) != -1 || find_exsymbol( _obj_file[f_count].Erecord ) != -1)
		return 1;// overlapped symbol.


	insert_exsymbol(_obj_file[f_count].Hrecord );	
	insert_exsymbol(_obj_file[f_count].Erecord );
	insert_exsymbol(_obj_file[f_count].Drecord );
	fclose(fp);

	return 0;
}

/*************************** 
 *
 *  Execute linkloader program.
 *
 * ************************/
int Command_Link(int f_count)
{

	int i = 0;
	int total_len = 0;
	int error_occured = 0;
	int tmp = 0,pass_flag = 0;
	_cur_progaddr = _progaddr;


	for(i = 0 ;i < f_count;i++)
	{
		pass_flag =  linking_loader_pass1(_obj_file[i].filename,i);
		if(	pass_flag == -1)
		{
			printf("File Open Error : File :: %s :: doesn't exits.\n",_obj_file[i].filename);
			error_occured++;
		}
		else if ( pass_flag == 1)
		{
			printf("Syntax Error :: %s :: Symbol Overlapped.\n",_obj_file[i].filename);
			error_occured++;
		}

	}

	if(error_occured != 0)
		return -1;
	


	for(i = 0 ;i < f_count;i++)
	{
		if( i == 0)
		{
			printf("\n\tcontrol\t\tsymbol\t\taddress\t\tlength\n");
			printf("\tsection\t\tname\n");
			printf("\t---------------------------------------------------------\n");
		}

		printf("\t%s\t\t\t\t%04X\t\t%04X\n",_obj_file[i].Hrecord,_cur_progaddr + _obj_file[i].start_addr ,_obj_file[i].length);
		total_len += _obj_file[i].start_addr + _obj_file[i].length;

		in_addr_exsymtab(_obj_file[i].Hrecord, _cur_progaddr + _obj_file[i].start_addr );

		printf("\t\t\t%s\t\t%04X\n",_obj_file[i].Drecord,_obj_file[i].ind_addr + _cur_progaddr);
	
		in_addr_exsymtab(_obj_file[i].Drecord, _cur_progaddr + _obj_file[i].ind_addr );//-------------->D record 추가

		printf("\t\t\t%s\t\t%04X\n",_obj_file[i].Erecord,_obj_file[i].end_addr + _cur_progaddr);
		
		in_addr_exsymtab(_obj_file[i].Erecord, _cur_progaddr + _obj_file[i].end_addr );

		tmp = _cur_progaddr + _obj_file[i].start_addr;
		_cur_progaddr += _obj_file[i].start_addr + _obj_file[i].length;
		_obj_file[i].start_addr = tmp;
	}

	printf("\t---------------------------------------------------------\n");
	printf("\t\t\t\t\ttotal length\t%04X\n",total_len);
	
	_total_len = total_len;

	for(i = 0 ; i < f_count;i++)
	{
		linking_loader_pass2(i);
	}


	return 0;


}

/***************************
 *
 *	Performs the actual loading, relocation, and linking.
 *
 * ************************/
int linking_loader_pass2(int f_count)
{

	int i, idx,j;
	char temp[MAX_LENGTH+10],rcd[MAX_LENGTH];
	char hex_dump[MAX_LENGTH];
	char sign;
	char* token = NULL;
	int where,len_or,dump_in,dump_addr;
	int hex_f = 0,refer_num = 0;
	int val_buf = 0;


	for(i = 0 ; i < _obj_file[f_count].line_num ;i++)
	{
		temp[0] = '\0';
		dump_in = 0;dump_addr = 0;
		strcpy(temp , _obj_file[f_count].lines[i]);


		if( _obj_file[f_count].lines[i][0] == 'R' )
		{
			token = strtok(temp," ");
			strcpy( _obj_file[f_count].Rrecord[1] , _obj_file[f_count].Hrecord);
			sscanf(token,"R%02x %s",&idx,rcd);
			strcpy( _obj_file[f_count].Rrecord[idx] , rcd);
			while(token != NULL)
			{
				token = strtok(NULL," ");
				if(token == NULL)
					break;
				sscanf(token,"%02x %s",&idx,rcd);
				strcpy( _obj_file[f_count].Rrecord[idx] , rcd);

			}


		}
		else if( _obj_file[f_count].lines[i][0] == '.' )
		{
			continue;
		}
		else if( _obj_file[f_count].lines[i][0] == 'T' )
		{
			// Trecord
			sscanf(_obj_file[f_count].lines[i],"T%6x %2x",&where,&len_or);
			
			dump_addr = where + _obj_file[f_count].start_addr;
			for(j = 9 ; j < len_or*2 +9;j+=2)
			{
				hex_dump[0] = _obj_file[f_count].lines[i][j];
				hex_dump[1] = _obj_file[f_count].lines[i][j+1];
				hex_dump[2] = '\0';
				sscanf(hex_dump,"%x",&hex_f);

				dump_in = hex_f;
				mem[dump_addr%16][dump_addr/16] += dump_in;
				dump_addr++;	
			}

		}
		else if( _obj_file[f_count].lines[i][0] == 'M' )
		{
			sscanf(_obj_file[f_count].lines[i],"M%6x %2x%c%x",&where,&len_or,&sign,&refer_num);
		
			dump_addr = where + _obj_file[f_count].start_addr;

			dump_in = find_exsymbol(_obj_file[f_count].Rrecord[refer_num]);
			
			
			val_buf = mem[dump_addr%16][dump_addr/16]*256*256 + mem[(dump_addr+1)%16][(dump_addr+1)/16]*16*16 + mem[(dump_addr+2)%16][(dump_addr+2)/16];
	
			//printf("%X : %X %X %X ",dump_addr,  mem[dump_addr%16][dump_addr/16], mem[(dump_addr+1)%16][(dump_addr+1)/16], mem[(dump_addr+2)%16][(dump_addr+2)/16]);
			//printf("%X %c %X",val_buf,sign, dump_in);
			if( sign == '+')
				val_buf += dump_in;
			else
			{
				val_buf -= dump_in;
			}

			if(val_buf < 0)
				val_buf -=256*256 + 256;


			//printf(" = %X %d\n",val_buf,val_buf);

			if( refer_num == 5 )
				mem[dump_addr%16][dump_addr/16] = ( val_buf%int_pow(16,5) )/int_pow(16,4);
			else
				mem[dump_addr%16][dump_addr/16] = ( val_buf%int_pow(16,6) )/int_pow(16,4);

			dump_addr++;

			mem[dump_addr%16][dump_addr/16] = ( val_buf%int_pow(16,4) )/int_pow(16,2);
			dump_addr++;

			mem[dump_addr%16][dump_addr/16] = ( val_buf%int_pow(16,2) );


		}

	}

	return 0;

}

/***************************
 *
 *  Insert address to external symbol table.
 *
 * ************************/
void in_addr_exsymtab(char* sym,int addr)
{

	int hash_value = symbol_hash_func(sym);
	_ex_symbol* tmp = _exsym[hash_value];

	while(tmp != NULL)
	{
		if(strcmp(tmp->symbol_name,sym) == 0)
		{
			tmp->addr = addr;
		}
		tmp = tmp->next;
	}

	return;


}

/***************************
 *
 *  Power function.
 *  
 * ************************/
int int_pow(int num,int exp)
{
	int i,ret = num;
	for( i = 1 ; i < exp ;i++)
		ret = ret * num;

	return ret;

}


/***************************
 *
 *  Clear all breakpoint.
 *  
 * ************************/
void bp_clear(){

	int i = 0;
	for(i = 0 ; i < _bp_cnt;i++)
		_BreakPoint_[i] = 0;
	_bp_cnt = 0;
	printf("\t[OK] clear all breakpoints.\n");
}

/***************************
 *
 *  Print all breakpoint.
 *  
 * ************************/
void print_bp()
{
	int i;
	if(_bp_cnt == 0)
	{
		printf("\tSet BreakPoint First!!\n");
		return;
	}
	printf("\tBreakPoint\n");
	printf("\t----------\n");
	for(i = 0 ; i < _bp_cnt;i++)
		printf("\t%X\n",_BreakPoint_[i]);

}

void run_prog()
{
	int i;
	int a_reg = 0 ,x_reg = 0 ,l_reg = 0,b_reg = 0,pc_reg = 0,s_reg = 0,t_reg = 0;
	int pg_start = _progaddr , pg_end,memi = 0;

	if( _BreakPoint_[_ex_bp] == 0)
		pg_end = _total_len;
	else
		pg_end = _BreakPoint_[_ex_bp];

	if(_prv_bp != 0)
		pg_start = _prv_bp;
	for(i = pg_start ; i <= pg_end;i++)
	{
		pc_reg = i+1;
		if( mem[i%16][i/16] == 0xB4)
		{
			memi = mem[(i+1)%16][(i+1)/16];
			if(memi / 16 == 0x1)
				x_reg++;
			else if( memi / 16 == 0x0)
				a_reg++;
			else if( memi / 16 == 0x4)
				s_reg++;
		}
		if( mem[i%16][i/16] == 0xB8)
		{
			memi = mem[(i+1)%16][(i+1)/16];
			if(memi / 16 == 0x5)
				t_reg++;

		}
		if( mem[i%16][i/16] == 0xA0)
		{
			memi = mem[(i+1)%16][(i+1)/16];
			if(memi / 16 == 0x5)
				t_reg++;
			else if(memi / 16 == 0x1)
				x_reg++;
			else if( memi / 16 == 0x0)
				a_reg++;
			else if( memi / 16 == 0x4)
				s_reg++;

			if(memi % 16 == 0x5)
				t_reg++;
			else if(memi % 16 == 0x1)
				x_reg++;
			else if( memi % 16 == 0x0)
				a_reg++;
			else if( memi % 16 == 0x4)
				s_reg++;
		}
		if( mem[i%16][i/16] == 0x69)
		{
			b_reg = 0x30 - mem[(i+2)%16][(i+2)/16];
		}

	}

	printf("\tA : %6X  X : %6X\n",a_reg,x_reg);
	printf("\tL : %6X  PC: %6X\n",l_reg,pc_reg);
	printf("\tB : %6X  S : %6X\n",b_reg,s_reg);
	printf("\tT : %6X \n",t_reg);

	printf("\tStop at checkpoint[%X]\n",pg_end);
	_prv_bp = _BreakPoint_[_ex_bp];
	_ex_bp++;
}
