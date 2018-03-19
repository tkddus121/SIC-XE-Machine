#include "20141549.h"


/****************************
 *
 *	Read asm file. 
 *	And save data.
 *
 * *************************/
int Read_asm_file(char* fname)
{
	
	FILE *fp;
	char content[MAX_LENGTH] = {0};
	int i = 0;


	if( (fp = fopen(fname,"r")) == NULL )
		return -1;
	if( strstr(fname,".asm") == NULL)
		return -1;	
	_end_flag = 0;
	while( fgets(content,sizeof(content),fp) != NULL )
	{
		if( Analyze_asm(content,i) == -1)
		;
		
		i++;
		if(_end_flag == 1)
		{
			_lstnum = i;	
			return 0;
		}
	}
	_lstnum = i;
	fclose(fp);
	return 0;

}


/****************************
 *
 *	Analyze assembly code.
 *
 * *************************/
int Analyze_asm(char* asm_str,int numi)
{
	char* temp=NULL;
	int hash_;
	char buf[MAX_LENGTH]={0};
	
	temp = strtok(asm_str," \t\n");

	strcpy(buf,temp);
	/*
	 * opcode 앞에 #붙은경우 처리
	 */

	if(asm_str[0] == '.')
	{	
		_assm[numi].isComment = 1;
		return 0;
	}

	if(strcmp(temp,"END") == 0 )
	{
		strcpy(_assm[numi].mnemonic,temp);
		temp = strtok(NULL," \t,'");

		if(temp == NULL)
			return -1;
		strcpy(_assm[numi].remain,temp);
		_end_flag = 1;
		return 0;
	}


	if(temp[0] != '+')
	{
		if( strcmp(temp,"BASE") == 0)
		{
			strcpy(_assm[numi].mnemonic,temp);
			temp = strtok(NULL," \t,'");

			if(temp == NULL)
				return -1;
			strcpy(_assm[numi].state,temp);

			if(_assm[numi].state[ strlen(_assm[numi].state)-1] == '\n')
				_assm[numi].state[ strlen(_assm[numi].state)-1] = '\0';

			return 0;
		}
		hash_ = find_opcode(temp);
	}
	else
	{
		hash_ = find_opcode(get_pure(temp));
	}

	if( hash_ == -1)//opcode아닐때
	{
		strcpy(_assm[numi].label,temp);

		if(find_symbol(temp) != -1)
		{
			error_occur(numi);
			return -1;// repeated symbol
		}
		else 
		{
			insert_symbol(temp);
		}
		temp = strtok(NULL," \t\n");   
	}



	//opcode
	if(strcmp(temp,"START") == 0)
	{
		strcpy(_filename,buf);
		strcpy(_assm[numi].mnemonic,temp);
		_start_flag = 1;

		temp = strtok(NULL," \t,'");
		if(temp == NULL)
			return -1;
		
		sscanf(temp,"%X",&_start_ads);
		return 0;

	}
	else if(strcmp(temp,"END") == 0 )
	{
		strcpy(_assm[numi].mnemonic,temp);
		temp = strtok(NULL," \t,'");
		if(temp == NULL)
			return -1;
		strcpy(_assm[numi].remain,temp);
		_end_flag = 1;
		return 0;
	}
	else if( strcmp(temp,"BASE") == 0)
	{
		strcpy(_assm[numi].mnemonic,temp);

		temp = strtok(NULL," \t,'");
		if(temp == NULL)
			return -1;
		strcpy(_assm[numi].state,temp);

		return 0;
	}
	else if( strcmp(temp,"BYTE") == 0)
	{
		strcpy(_assm[numi].mnemonic,temp);
		temp = strtok(NULL," \t,'");
		if(temp == NULL)
			return -1;
		strcpy(_assm[numi].state,temp);

		temp = strtok(NULL," \t,'");

		if(temp == NULL || (strlen(temp)%2 == 1 && _assm[numi].state[0] == 'X') )
		{
			return -1;
		}
		else
		{
			_assm[numi].remain_flag = 1;
			strcpy(_assm[numi].remain,temp);
		}
		return 0;

	}
	else if( strcmp(temp,"RESW") == 0)
	{
		strcpy(_assm[numi].mnemonic,temp);
		temp = strtok(NULL," \t,'");
		if(temp == NULL)
			return -1;
		strcpy(_assm[numi].remain,temp);

		return 0;
	}
	else if( strcmp(temp,"RESB") == 0)
	{
		strcpy(_assm[numi].mnemonic,temp);
		temp = strtok(NULL," \t,'");
		if(temp == NULL)
			return -1;
		strcpy(_assm[numi].remain,temp);

		return 0;
	}
	else if( strcmp(temp, "RSUB") == 0)
	{
		strcpy(_assm[numi].mnemonic,temp);
		return 0;

	}


	if(temp[0] != '+')
	{
		hash_ = find_opcode(temp);
	}
	else
	{
		hash_ = find_opcode( get_pure(temp) );
	}


	if(hash_ == -1)
	{
		//error_occur(numi);
		return -1;//error
	}

	strcpy(_assm[numi].mnemonic,temp);

	//state
	
	temp = strtok(NULL," ,\t\'\n");


	if(temp == NULL)
	{
		return -1;
	}
	strcpy(_assm[numi].state,temp);

	//추가적으로 남은거
	temp = strtok(NULL," \t,'");
	
	if(temp == NULL)
	{
		return 0;//good
	}
	else
	{
		_assm[numi].remain_flag = 1;
		strcpy(_assm[numi].remain,temp);
	}


	return 0;

}

/****************************
 *
 *	Insert symbol in  symbol table.
 *
 * *************************/
void insert_symbol(char* symbol)
{
	int hash_value;

	_symbol* new = NULL;
	_symbol* tmp = NULL;
	//나중에 location추가
	
	new = (_symbol*)malloc(sizeof(_symbol));
	strcpy(new->symbol_name,symbol);
	new->next = NULL;

	hash_value = symbol_hash_func(symbol);
	tmp = _sym[hash_value];

	if( tmp == NULL )
	{
		_sym[hash_value] = new;
	}
	else
	{
		for(;;)
		{
			if(tmp->next == NULL)
				break;
			tmp = tmp->next;
	
		}
		tmp-> next = new;
	}


}

/****************************
 *
 *	Hash function returns symbol's hash.
 *
 * **************************/
int symbol_hash_func(char* sym)
{
	unsigned int i,hash = 0,len = strlen(sym);
	for(i = 0 ; i < len ; i++)
	{
		hash = hash + sym[i];
	}

	return (hash%20);

}
/****************************
 *
 *	Check input string is opcode.
 *
 *	Function returns hash_value. 
 *
 * *************************/
int find_opcode(char* opcd)
{
	int hash_value = opcode_hash_func(opcd);
	opc* tmp = _opcode[hash_value];

	while(tmp != NULL)
	{
		if(strcmp(tmp->opcode,opcd) == 0)
		{
			return hash_value;
		}
		tmp = tmp->next;
	}
	return -1;

}

/****************************
 *
 *	Find symbol in  symbol table.
 *
 * *************************/
int find_symbol(char* sym)
{
	int hash_value = symbol_hash_func(sym);
	_symbol* tmp = _sym[hash_value];

	while(tmp != NULL)
	{
		if(strcmp(tmp->symbol_name,sym) == 0)
		{
			return tmp->loc;
		}
		tmp = tmp->next;
	}

	return -1;

}

/****************************
 *
 *	Syntax error function.
 *
 * *************************/
void Syntax_error()
{
	printf("\tSyntax Error!\n");
}

/****************************
 *
 *	 Calculate location count.
 *
 * *************************/
void Calculate_loc()
{
	int i;
	int form = 0,all_loc = 0,cur_loc = 0,operand;

	
	for(i = 0 ; i < _lstnum ;i++)
	{

		_assm[i].loc_cnt = all_loc;

		if( strcmp(_assm[i].mnemonic,"START") == 0)
		{
			cur_loc = _start_ads;
			all_loc += cur_loc;
//			continue;
		}

		if( strcmp(_assm[i].mnemonic,"BYTE") == 0 ||
			strcmp(_assm[i].mnemonic,"RESB") == 0 ||
			strcmp(_assm[i].mnemonic,"RESW") == 0 )
		{
			if(strcmp(_assm[i].mnemonic,"BYTE") == 0)
			{
				if(_assm[i].state[0] == 'C')	
					form = strlen(_assm[i].remain);
				else if(_assm[i].state[0] == 'X')
					form = strlen(_assm[i].remain)/2;
				else
					error_occur(i);
			}
			if(strcmp(_assm[i].mnemonic,"RESB") == 0)
			{
				sscanf(_assm[i].remain,"%d",&operand);	
				form = operand;
			}
			if(strcmp(_assm[i].mnemonic,"RESW") == 0)
			{
				sscanf(_assm[i].remain,"%d",&operand);	
				form = operand*3;
			}
			cur_loc = form;
			all_loc += cur_loc;
			continue;
		}
		else if(_assm[i].mnemonic[0] == '+')
		{
			if(get_opcode_form(get_pure(_assm[i].mnemonic)) != -1)
			{
				form = 4;
			}
			else 
				error_occur(i);

		}
		else
			form = get_opcode_form(_assm[i].mnemonic);

		if(1 > form || 4 < form)
				form = 0;
		cur_loc = form;
		all_loc += cur_loc;
		

	}

}

/****************************
 *
 *	Returns opcode's format.
 *
 * *************************/
int get_opcode_form(char* opcd)
{
	int hash_value = opcode_hash_func(opcd);
	opc* tmp = _opcode[hash_value];

	while(tmp != NULL)
	{
		if(strcmp(tmp->opcode,opcd) == 0)
		{
			return (tmp->format[0] - '0');
		}
		tmp = tmp->next;
	}

	return -1;

}

/****************************
 *
 *	Copy string without first character.
 *
 * *************************/
char* get_pure(char* input)
{
	int i,input_len = strlen(input);
	char* out = (char*)malloc(sizeof(char)*input_len);

	for(i = 1 ; i < input_len;i++)
		out[i-1] = input[i];

	out[strlen(input)-1] = '\0';
	return out;

}

/****************************
 *
 *	Make list file.	
 *
 * *************************/
void Creat_lst_file(char* filename)
{
	FILE* fp;

	int i;
	
	fp = fopen(filename,"w");

	
	for(i = 0 ; i < _lstnum;i++)
	{

		if(_assm[i].remain[ strlen(_assm[i].remain)-1] == '\n')
			_assm[i].remain[ strlen(_assm[i].remain)-1] = '\0';

		if(_assm[i].isComment == 1)
		{
			fprintf(fp,"%3d\n",(i+1)*5);
			continue;
		}
		else
		{

			if( strcmp(_assm[i].mnemonic,"END") == 0)
				fprintf(fp,"\t \t %s\t %s\t %s",_assm[i].label,_assm[i].mnemonic,_assm[i].state);  
			else if( strcmp(_assm[i].mnemonic,"BASE") == 0)
			{
				fprintf(fp,"%3d\t \t %s\t %s\t %s",(i+1)*5,_assm[i].label,_assm[i].mnemonic,_assm[i].state); 
			}
			else
			{
				if( strcmp( _assm[i].mnemonic,"START" ) == 0)
					fprintf(fp,"%3d\t %04X\t %s\t %s\t %X",(i+1)*5,_start_ads,_assm[i].label,_assm[i].mnemonic,_start_ads);
				else
					fprintf(fp,"%3d\t %04X\t %s\t %s\t %s",(i+1)*5,_assm[i].loc_cnt,_assm[i].label,_assm[i].mnemonic,_assm[i].state);
			}
		}
		//개행처리는 object code에서


		if( strcmp(_assm[i].mnemonic,"BYTE")  == 0)
			fprintf(fp,"'%s'",_assm[i].remain);
		else if ( strcmp(_assm[i].state,"BUFFER") == 0)
		{
			if(_assm[i].remain_flag == 1)
				fprintf(fp,",%s",_assm[i].remain);
			else 
				;    
		}
		else if ( strcmp(_assm[i].mnemonic,"COMPR") == 0)
		{
			fprintf(fp,",%s",_assm[i].remain);
		}
		else
			fprintf(fp,"%s",_assm[i].remain);

		if(strlen(_assm[i].state) + strlen(_assm[i].remain)< 7)
			fprintf(fp,"\t");
		if(_assm[i].object_code != 0 )
		{
			if( get_opcode_form(_assm[i].mnemonic) == 3 && _assm[i].mnemonic[0] != '+')
				fprintf(fp,"\t%06X\n",_assm[i].object_code);
			else if(_assm[i].state[0] == 'X' &&get_opcode_form(_assm[i].mnemonic) != 2)
				fprintf(fp,"\t%s\n",_assm[i].remain);
			else 
				fprintf(fp,"\t%X\n",_assm[i].object_code);
		}
		else
			fprintf(fp,"\n");
	}
	fclose(fp);
}

/****************************
 *
 *	Generate object code.
 *
 * *************************/
void gen_object_code()
{
	int i,j;
	int obj_code = 0,form = 0;
	int nixbpe = 0,op_number = 0,disp = 0;
	for(i = 0; i < _lstnum ;i++)
	{

		form = 0 ;nixbpe = 0;op_number = 0;disp = 0;obj_code=0;
		if(find_opcode(_assm[i].mnemonic) == -1 && _assm[i].mnemonic[0] != '+')
		{
			if(strcmp(_assm[i].mnemonic,"BYTE") == 0)
			{
				if(_assm[i].state[0] == 'C')
				{
					for(j = 0 ;j < strlen(_assm[i].remain);j++)
					{
						obj_code = obj_code*256;
						obj_code += _assm[i].remain[j];//character 일때
					}

				}
				else if(_assm[i].state[0] == 'X')
				{
					sscanf(_assm[i].remain,"%x",&obj_code);
				}
				else
					error_occur(i);
				_assm[i].object_code = obj_code;

			}
		}
		else
		{
			if(_assm[i].mnemonic[0] == '+')
			{
				op_number = get_opnum(get_pure(_assm[i].mnemonic));
				form = 4;
			}
			else
			{
				op_number = get_opnum(_assm[i].mnemonic);    
				form = get_opcode_form(_assm[i].mnemonic);
			}
			if(op_number == -1) ///////////////////////////////////////////
				error_occur(i);

			//get displayment

			if( _assm[i].state[0] == '#'|| _assm[i].state[0] == '@')
				disp = find_symbol(get_pure(_assm[i].state)) - _assm[i+1].loc_cnt;
			else
				disp = find_symbol(_assm[i].state) - _assm[i+1].loc_cnt;


			//get nixbpe

			//get n,i
			if(_assm[i].state[0] == '@')			
			{
				//indirect addressing 1 0
				nixbpe += 32; 

			}
			else if(_assm[i].state[0] == '#')
			{

				if(_assm[i].state[1] < '0' || _assm[i].state[1] > '9')
					_base = i;
				// immediate addressing 0 1
				nixbpe += 16;

			}
			else if(form == 1)
			{
				//SIC 0 0
				;
			}
			else
			{	
				//simple addressing 1 1
				nixbpe+= 32+16;
			}



			//get xbpe

			if(_assm[i].remain[0] == 'X')
			{
				nixbpe += 8;
			}

			if( (_assm[i].state[0] == '#' || _assm[i].state[0] == '@')	&&(_assm[i].state[1] >= '0' && _assm[i].state[1] <='9'))
				;
			else
			{
				if(disp >= -2048 && disp<= 2047 )
					nixbpe += 2;
				else if(disp >= 0 || disp <= 4095)
				{
					disp = find_symbol(_assm[i].state) - find_symbol(get_pure(_assm[_base].state));
					nixbpe += 4;
				}
			}
			if(form == 4)
			{
				nixbpe = 1;

				if(_assm[i].state[0] == '@')			
				{
					//indirect addressing 1 0
					nixbpe += 32; 

				}
				else if(_assm[i].state[0] == '#')
				{

					if(_assm[i].state[1] < '0' || _assm[i].state[1] > '9')
						_base = i;
					// immediate addressing 0 1
					nixbpe += 16;

				}
				else if(form == 1)
				{
					//SIC 0 0
					;
				}
				else
				{	
					//simple addressing 1 1
					nixbpe+= 32+16;
				}

				if(_assm[i].state[0] == '#' || _assm[i].state[0] == '@')
				{
					if(_assm[i].state[1] >= '0' && _assm[i].state[1] <='9')
					{
						sscanf(get_pure(_assm[i].state),"%d",&disp);
					}
					else
						disp = find_symbol(get_pure(_assm[i].state));
				}
				else
				{
					if(_assm[i].state[0] >= '0' && _assm[i].state[0] <='9')
					{
						sscanf(get_pure(_assm[i].state),"%d",&disp);
					}
					else
						disp = find_symbol(_assm[i].state);
				}

			}
			else
			{

				if(_assm[i].state[0] == '#' || _assm[i].state[0] == '@')
				{	
					if(_assm[i].state[1] >= '0' && _assm[i].state[1] <='9')
					{
						sscanf(get_pure(_assm[i].state),"%d",&disp);
					}
				}
			}

			if(form == 1)
			{
				obj_code = op_number;
			}
			else if(form == 2)
			{
				obj_code = object_code_form2(op_number,_assm[i].state[0],_assm[i].remain[0]);

			}
			else if(form == 3)
			{
				if(strcmp(_assm[i].mnemonic, "RSUB") == 0)
					obj_code = 0x4F0000;
				else
					obj_code = object_code_form34(op_number,nixbpe,disp);
			}
			else if(form == 4)
			{
				if(strcmp(_assm[i].mnemonic, "RSUB") == 0)  
					obj_code = 0x4F100000;
				else 
					obj_code = object_code_form34(op_number,nixbpe,disp);
			}
			else
			{
				printf("Error!!\n");
				return;
			}

			_assm[i].object_code = obj_code;

		}

	}
	

}

/****************************
 *
 *	Insert location count to symbol table.
 *
 * *************************/
void loc_to_symtab()
{
	int i;
	int hash_value;
	for(i = 0 ; i < _lstnum;i++)
	{
		if(_assm[i].label == NULL)
			continue;
		hash_value = symbol_hash_func(_assm[i].label);
		_symbol* tmp = _sym[hash_value];

		while(tmp != NULL)
		{
			if(strcmp(tmp->symbol_name , _assm[i].label) == 0)
			{
				tmp->loc = _assm[i].loc_cnt;
				break;
			}
			tmp = tmp->next;

		}

	}

}

/****************************
 *
 *	returns opcode number.
 *
 * *************************/
int get_opnum(char* opcd)
{
	int hash_value = opcode_hash_func(opcd);
	opc* tmp = _opcode[hash_value];

	while(tmp != NULL)
	{
		if(strcmp(tmp->opcode,opcd) == 0)
		{
			return tmp->hash_val;
		}
		tmp = tmp->next;
	}
	return -1;

}

/****************************
 *
 *	If format is 2, generate object code
 *	from register.
 *
 * **************************/
int object_code_form2(int opnum,char regA,char regB)
{
	int ret = 0;
	char r1={0},r2={0};

	if( regA == 'A')
		r1 = 0;
	else if( regA == 'X')
		r1 = 1;
	else if( regA == 'B')
		r1 = 3;
	else if( regA == 'S')
		r1 = 4;
	else if( regA == 'T')
		r1 = 5;


	if( regB == 'A')
		r2 = 0;
	else if( regB == 'X')
		r2 = 1;
	else if( regB == 'B')
		r2 = 3;
	else if( regB == 'S')
		r2 = 4;
	else if( regB == 'T')
		r2 = 5;

	opnum <<= 8;
	ret |= opnum;
	r1 <<= 4;
	ret |= r1;
	ret |= r2;

	return ret;
}
/****************************
 *
 *	If format is 3, generate object code
 *	from nixbpe register and displayment.
 *
 * **************************/
int object_code_form34(int opnum,int reg,int disp)
{
	int ret = 0;

	if( reg%2 != 1) 
	{
		disp &= 0xFFF;
		opnum <<= 16;
		ret |= opnum;
		reg <<= 12;
		ret |= reg;
		ret |= disp;
	}
	else 
	{
		disp &= 0xFFFFF;
		opnum <<= 24;
		ret |= opnum;
		reg <<= 20;
		ret |= reg;
		ret |= disp;
	}
	return ret;

}

/****************************
 *
 *	Make object file.
 *
 * *************************/
void Creat_obj_file(char* filename)
{
	FILE* fp;
	int i,j;
	int line_cnt = 0,ow = 1,next_loc = 0,rare_len = 0;
	char rare[MAX_LENGTH],buffer[MAX_LENGTH];
	
	fp = fopen(filename,"w");


	fprintf(fp,"H");
	fprintf(fp,"%-6s",_filename);
	fprintf(fp,"%06X",_start_ads);
	fprintf(fp,"%06X\n",_assm[_lstnum - 1].loc_cnt - _assm[0].loc_cnt);

	for(i = 0 ; i < _lstnum;i++)
	{
		next_loc = 0;
		if(strcmp( _assm[i].mnemonic,"END") == 0)
		{
			modification_find();
			fprintf(fp,"%02X%s\n",rare_len,rare);
			for(j = 0 ; j < _modi_cnt ;j++)
			{
				fprintf(fp,"M%06X05\n",_modafi[j] - _start_ads + 1 );
			}
			
			fprintf(fp,"E%06X",_start_ads);
			break;
		}

		for(j = 1;;j++)
		{
			if(_assm[i+j].loc_cnt > _assm[i].loc_cnt)
			{
				next_loc = _assm[i+j].loc_cnt - _assm[i].loc_cnt;
				break;
			}
		}

		if(ow == 1)
		{
			fprintf(fp,"T");
			fprintf(fp,"%06X",_assm[i].loc_cnt);
			

			if(_assm[i].object_code != 0)
			{
				if(give_hex_length(_assm[i].object_code) == 5)
					sprintf(buffer,"%06X",_assm[i].object_code);
				else if(give_hex_length(_assm[i].object_code) == 3)
					sprintf(buffer,"%04X",_assm[i].object_code);
				else if(give_hex_length(_assm[i].object_code) == 1)
					sprintf(buffer,"%02X",_assm[i].object_code);
				else
					sprintf(buffer,"%X",_assm[i].object_code);

				strcat(rare,buffer);
			}

			line_cnt += give_hex_length(_assm[i].object_code);
			ow = 0;
		}
		else	
		{
			if(_assm[i].object_code != 0)
			{
				if(give_hex_length(_assm[i].object_code) == 5)
					sprintf(buffer,"%06X",_assm[i].object_code);
				else if (give_hex_length(_assm[i].object_code) == 3)
					sprintf(buffer,"%04X",_assm[i].object_code);
				else if(give_hex_length(_assm[i].object_code) == 1)
					sprintf(buffer,"%02X",_assm[i].object_code);
				else
					sprintf(buffer,"%X",_assm[i].object_code);

				strcat(rare,buffer);
				line_cnt += give_hex_length(_assm[i].object_code);
	
			}

		}

		rare_len = (strlen(rare)/2);

		if(line_cnt + give_hex_length(_assm[i+1].object_code) > 60 ||  next_loc >= 0x1000 )
		{
			ow = 1;
			fprintf(fp,"%X%s\n",rare_len,rare);
			rare[0] = '\0';
			rare_len = 0;
			line_cnt = 0;

		}

	}

	fclose(fp);

}
/****************************
 *
 *	return hex length. 
 *
 * *************************/
int give_hex_length(int hex)
{
	int len;
	int hex_buf = hex;
	for(len = 0 ;;len++)
	{
		if(hex_buf == 0)
			break;
		hex_buf = hex_buf/(16);
	}
	
	return len;
}

/****************************
 *
 *	get modification
 *
 * *************************/
void modification_find()
{
	int i;
	_modi_cnt = 0;
	for(i = 0; i < _lstnum;i++)
	{
		if( _assm[i].mnemonic[0] == '+' && (_assm[i].state[1] < '0' || _assm[i].state[1] > '9' ))
		{
			_modafi[_modi_cnt] = _assm[i].loc_cnt;

			_modi_cnt++;
		}
	}
}

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
void Command_assemble()
{	

	char fill[MAX_LENGTH]={0},file2[MAX_LENGTH]={0};
	char* file1 = NULL;

	int i;
	fill[0] = '\0';


	for(i = 0 ; i < _Error_cnt;i++)
	{
		_error_lines[i] = 0;
	}
	_Error_cnt = 0;
	
	symbol_free();
	strcpy(fill,_asmfile);

	if(	Read_asm_file(fill)  == -1)
	{
		printf("FILE OPEN ERROR!!\n");	
		return;
	}

	if(_Error_cnt > 0)
	{
		error_print();
		return;
	}

	file1 = strtok(fill,".");

	strcpy(file2,file1);
	Calculate_loc();
	loc_to_symtab();
	gen_object_code();

	strcat(file1,".lst");
	strcat(file2,".obj");
	Creat_lst_file(file1);
	Creat_obj_file(file2);
/*
	Read_asm_file("2_5.asm");
	Calculate_loc();
	loc_to_symtab();
	gen_object_code();
	//strcat(file1,".lst");
	//strcat(file2,".obj");
	Creat_lst_file("2_5.lst");
	Creat_obj_file("2_5.obj");
*/
	printf("\toutput file : [%s], [%s]\n",file1,file2);
	return;
}

/****************************
 *
 *	printf if there is error.
 *
 * *************************/
void error_print()
{
	int i;


	printf("\t==== asm file Error ====\n");
	for(i = 0 ; i < _Error_cnt ;i++)
	{
		printf("Error : Line %d\n",_error_lines[i]);
	}
	printf("\t========================\n");

}

/****************************
 *
 *	insert new error
 *
 * *************************/
void error_occur(int where)
{

	_error_lines[_Error_cnt] = where;
	_Error_cnt++;

}

/****************************
 *
 *	Sort symbol table and print it.
 *
 * *************************/
void Command_symbol()
{

	char symbs[MAX_LENGTH][MAX_LENGTH],str_tmp[MAX_LENGTH];
	int symb_loc[MAX_LENGTH];
	int i,j,sym_cnt = 0,t;

	for(i = 0 ; i < 20; i++)
	{
		_symbol* tmp = _sym[i];

		while(tmp != NULL)
		{
			strcpy( symbs[sym_cnt] , tmp->symbol_name );
			symb_loc[sym_cnt] = tmp->loc;
			sym_cnt++;
			if(tmp->next != NULL)
				;
			tmp = tmp->next;
		}

	}


	for(i = 0 ; i < sym_cnt;i++)
		for(j = 0 ; j < sym_cnt;j++)
		{
			if(strcmp(symbs[i],symbs[j]) > 0)
			{	
				strcpy( str_tmp , symbs[i]);
				strcpy( symbs[i] , symbs[j]);
				strcpy( symbs[j] , str_tmp);
				swap(symb_loc[i],symb_loc[j],t);	
			}

		}

	for(i = 0 ; i < sym_cnt;i++)    
		printf("\t%s\t%04X\n",symbs[i],symb_loc[i]);



}

/****************************
 *
 *	symbol free.
 *
 * *************************/
void symbol_free()
{
	int i;
	for( i = 0 ; i < 20 ;i++)
	{
		while(_sym[i] != NULL)
		{
			_symbol* tmp = _sym[i];
			_sym[i] = _sym[i]->next;

			free(tmp);
		}
	}

}

