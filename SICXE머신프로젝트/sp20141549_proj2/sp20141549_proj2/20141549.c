#include "20141549.h"

int main()
{

	char input[MAX_LENGTH];
	//get opcode
	get_opcode();
	//Command_assemble();
	while(1)
	{

		strcpy(input,get_commands());
		
		//error!!;
		if(strcmp(input, "ER") == 0)
		{
			printf("\tInvalid Command!\n");
			continue;
		}
	//	Command_assemble();
		CommandRead(input);
		//if input is 'q' or 'quit'
		if(go_out == 1)
			break;	
	}

	return 0;
}
