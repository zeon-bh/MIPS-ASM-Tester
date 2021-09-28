// MIPS_32 Unit test program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\Lab_01\MIPS_32.h"

#define MAX_BUFFER 50

/* Instruction Format
	I-Type = op_code | rs | rt | immediate
	R-Type = op_code | rs | rt | rd | shamt
	J-Type = op_code | address
	SB-Type = op_code | rs | immediate
*/

// function prototypes
void Print_Mips32ID(uint32_t test_inst);
void Print_State(CPU_State * theState);
void PrintMemory(void);

// Assemble instruction routines
uint32_t AssembleInstruction(char *string,int size);
uint32_t Encode_I(char * instruction);
uint32_t Encode_R(char * instruction);
uint32_t Encode_J(char * instruction);
uint32_t Encode_SB(char * instruction); // special branch
int8_t GetOpcode(char * op_string, char type);
void GetFieldData(char inst_field[6][10],char * instruction);

// MIPS_Test Main Routine
int main(){
	uint32_t test_inst = 0;
	char code[MAX_BUFFER];
	
	// Allocate dyanamic memory
	init_memory();
	
	// Initial MIPS 32 State
	printf("Current State----------\n");
	Print_State(&CURRENT_STATE);
	printf("Next State----------\n");
	Print_State(&NEXT_STATE);
	
	while(1){
		printf("Enter MIPS_32 Instruction, q to quit. ");
		gets(code);
		
		if(code[0] == 'q' && code[1] == '\0') exit(1);
		else{
			int wspace = 0;
			// Count number of white spaces
			for(int i = 0; i < MAX_BUFFER; i++){
				if(code[i] == ' ') wspace++;
			}

			test_inst = AssembleInstruction(code,wspace);
			if(test_inst){
				MIPS32_decode(test_inst);
				// Update current state
				CURRENT_STATE = NEXT_STATE;
				printf("\nUpdated Current State ----------------\n");
				Print_State(&CURRENT_STATE);
				Print_Mips32ID(test_inst);
			}else printf("Re-enter correct instruction code\n");
		}
		memset(code,0,sizeof(code)); // clear code string buffer
	}
	return 0;
}

// Test if fields in Mips32_ID are set correctly
void Print_Mips32ID(uint32_t test_inst){
	printf("\n\n");
	if(Mips32_ID.op_code == (uint8_t)SPECIAL){ // R-type
		printf("R-Type Instruction\n");
		printf("OP-CODE = %d\n", Mips32_ID.function);
		printf("Source Register = %d\n", Mips32_ID.r_source);
		printf("Target Register = %d\n", Mips32_ID.r_target);
		printf("Destination Register = %d\n", Mips32_ID.r_destination);
		printf("Shift Amount = %d\n\n", Mips32_ID.shamt);
	} else if (Mips32_ID.op_code == (uint8_t)REGIMM) {
		printf("Special Branch Instruction\n");
		printf("OP-CODE = %d\n", Mips32_ID.r_target); // rt holds op_code
		printf("Source Register = %d\n", Mips32_ID.r_source);
		printf("Immediate = %d\n\n", Mips32_ID.immediate);
	
	} else if(Mips32_ID.op_code == 2 || Mips32_ID.op_code == 3){
		printf("J-Type Instruction\n");
		printf("OP-CODE = %d\n", Mips32_ID.op_code);
		printf("Jump Address = %d\n\n", Mips32_ID.j_address);
		
	} else if(Mips32_ID.op_code > 3){
		printf("I-Type Instruction\n");
		printf("OP-CODE = %d\n", Mips32_ID.op_code);
		printf("Source Register = %d\n", Mips32_ID.r_source);
		printf("Target Register = %d\n", Mips32_ID.r_target);
		printf("Immediate = %d\n\n", Mips32_ID.immediate);
	}else {
		printf("Invalid instruction!!!\n");
	}
}

// Print the PC and registers of CURRENT_STATE
void Print_State(CPU_State * theState){
	printf("Program Counter = 0x%x\n", theState->PC);
	printf("MIPS 32 Registers----------:\n");
	
	for(int i = 0; i < 32; i++){
		if(i !=0 && i%3 == 0) printf("\n");
		printf("Register %d = 0x%x\t", i, theState->REGS[i]);
	}
	
	printf("\nHIGH REGISTER = 0x%x",theState->HI);
	printf("\tLOW REGISTER = 0x%x\n",theState->LO);
}

//************************** ASSEMBLE ***************************//

// assemble instruction
uint32_t AssembleInstruction(char *string,int size){
	uint32_t test_inst = 0;
	
	switch(size){
		case 1:
				test_inst = Encode_J(string);
				break;
		case 2:
				test_inst = Encode_SB(string);
				break;
		case 3:
				test_inst = Encode_I(string);
				break;
		case 4:
				test_inst = Encode_R(string);
				break;
		default:
				printf("Invalid instruction!!!\n");
				break;
	}
	
	if(test_inst == 0) printf("\nInstruction assembly failed!!!\n");
	else printf("\nTest Instruction = 0x%x\n", test_inst);
	
	return test_inst;
}

// encode I-Type instruction from string
uint32_t Encode_I(char * instruction){
	int8_t i_code;
	char inst_field[6][10]; // 4 fields - size 10 for I-Type
	char * ptr;
	
	GetFieldData(inst_field,instruction);
	i_code = GetOpcode(inst_field[0],'i');
	if(i_code != -1){
		return ((uint8_t)i_code << 26) |
			   ((uint8_t)strtoul(inst_field[1],&ptr,10) << 21) |
			   ((uint8_t)strtoul(inst_field[2],&ptr,10) << 16) |
			   (((uint16_t)strtoul(inst_field[3],&ptr,10)) & 0xFFFF);
	} else return 0;
}

// encode R-Type instruction from string
uint32_t Encode_R(char * instruction){
	int8_t r_function;
	char inst_field[6][10]; // 5 fields - size 10 for R-Type
	char * ptr;
	
	GetFieldData(inst_field,instruction);
	r_function = GetOpcode(inst_field[0],'r');
	if(r_function != -1){
		return ((uint8_t)strtoul(inst_field[0],&ptr,10) << 26) |
				((uint8_t)strtoul(inst_field[1],&ptr,10) << 21) |
				((uint8_t)strtoul(inst_field[2],&ptr,10) << 16) |
				((uint8_t)strtoul(inst_field[3],&ptr,10) << 11) |
				((uint8_t)strtoul(inst_field[4],&ptr,10) << 6) |
				((uint8_t)r_function & 0xFF);
	} else return 0;
}

// encode J-Type instruction from string
uint32_t Encode_J(char * instruction){
	char inst_field[6][10]; // 2 fields - size 10 for J-Type
	int8_t j_code;
	char * ptr;
	
	GetFieldData(inst_field,instruction);
	j_code = GetOpcode(inst_field[0],'j');
	if(j_code != -1){
		return ((uint8_t)j_code << 26) |
				((uint32_t)strtoul(inst_field[1],&ptr,10) & 0x3FFFFFF);
	}else return 0;
}

// encode Special branch instruction from string
uint32_t Encode_SB(char * instruction){
	char inst_field[6][10]; // 3 fields - size 10 for SB-Type
	int8_t sb_code;
	char * ptr;
	
	GetFieldData(inst_field,instruction);
	sb_code = GetOpcode(inst_field[0],'s');
	if(sb_code != -1){
		return ((uint8_t)REGIMM << 26) |
				((uint8_t)strtoul(inst_field[1],&ptr,10) << 21) | // source register
				((uint8_t)sb_code << 16) | // branch opcode
				((uint8_t)strtoul(inst_field[2],&ptr,10) & 0xFFFF); // immediate
	}else return 0;
	
}

// Get OP-Code
int8_t GetOpcode(char * op_string, char type){
	const Mips32_Instruction * inst_type = NULL;
	int size = 0;
	
	switch(type){
		case 'i':
				inst_type = Mips32_I;
				size = sizeof(Mips32_I)/sizeof(Mips32_Instruction);
				break;
		case 'r':
				inst_type = Mips32_R;
				size = sizeof(Mips32_R)/sizeof(Mips32_Instruction);
				break;
		case 'j':
				inst_type = Mips32_J;
				size = sizeof(Mips32_J)/sizeof(Mips32_Instruction);
				break;
		case 's':
				inst_type = Mips32_SB;
				size = sizeof(Mips32_SB)/sizeof(Mips32_Instruction);
				break;
		default:
				printf("THIS SHOULDNT HAPPEN!!!\n");
				printf("Invalid Instruction Type!!!\n");
				return -1;
	}
	
	for(int i = 0; i < size; i++){
		if(strcmp(inst_type[i].inst_string,op_string) == 0){
			return inst_type[i].identifier;
		}
	}
	printf("Instruction of type %c not found!!!\n",type);
	return -1;
}

// Get field data from string
void GetFieldData(char inst_field[6][10],char * instruction){
	int field = 0, size = 0;
	
	for(int length = 0; length <= strlen(instruction);length++){
		if(instruction[length] != ' ' && size < 9){
			inst_field[field][size] = instruction[length];
			size++;
		} else {
			if(size < 10) inst_field[field][size] = '\0';
			size = 0;
			field++;
		}
	}
}

//************************ MEMORY ******************************//

// Print the stored DATA contents of the RAM
void PrintMemory(void){
	
	
}