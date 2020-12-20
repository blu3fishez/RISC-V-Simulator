#define _CRT_SECURE_NO_WARNINGS // TEST ENVIRONMENT : VS 2019

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define FALSE 0
#define TRUE 1
enum Mode { add, addi, jal, jalr, beq, ld, sd };
long _32bit[32];

//clock cycles
long long cycles = 0;

// registers
long long int regs[32];

// program counter
unsigned long pc = 0;

// memory
#define INST_MEM_SIZE 32*1024
#define DATA_MEM_SIZE 32*1024
unsigned long inst_mem[INST_MEM_SIZE]; //instruction memory
unsigned long long data_mem[DATA_MEM_SIZE]; //data memory

//misc. function
int init(char* filename);

long fetch() { //fetch an instruction from a instruction memory
	// IF
	// inst_mem는 index당 32비트를 저장하고 있고,
	// pc가 나타내는 값의 단위는 주소의 기본 이동단위인, 8비트 이기 때문에,
	// 실제로 우리가 두번째줄의 명령어를 입력받고 싶다면, C언어 내에서는 pc/4 값으로 inst_mem을 인덱싱을 해주어야합니다. 
	return inst_mem[pc / 4];
}

void decode(unsigned long instruction, long rs[], int* mode) { //decode the instruction and read data from register file
	// ID
	// read
	rs[0] = (instruction % _32bit[12]) / _32bit[7];
	rs[1] = (instruction % _32bit[20]) / _32bit[15];
	rs[2] = (instruction % _32bit[25]) / _32bit[20];

	int opcode = instruction % _32bit[7];
	switch (opcode) {
	case 0b1101111:
		*mode = jal;
		return;
	case 0b1100111:
		*mode = jalr;
		return;
	case 0b1100011:
		*mode = beq;
		return;
	case 0b0010011:
		*mode = addi;
		return;
	case 0b0110011:
		*mode = add;
		return;
	case 0b0000011:
		*mode = ld;
		return;
	case 0b0100011:
		*mode = sd;
		return;
	default:;
	}
	printf("Decode Fail : ");
	printf("Wrong Instruction Opcode.\n");
	printf("Please Insert only add, addi, jal, jalr, ld, sd, beq.\n");
	exit(1);
}

long exe(unsigned long instruction, long rs[], int mode) { //perform the appropriate operation 
	// EXE
	long imm = 0;
	long execution = 0;
	// imm generation
	switch (mode) {
	case addi:;
	case ld:;
	case jalr:; // i type
		imm = instruction / _32bit[20];
		if (imm / _32bit[11]) imm |= 0xFFFFF000;
		break;
	case sd:; // s type
		imm = ((instruction / _32bit[25]) << 5) + (instruction % _32bit[12]) / _32bit[7];
		if (imm / _32bit[11]) imm |= 0xFFFFF000;
		break;
	case beq:; // sb type
		imm = ((instruction % _32bit[12] / _32bit[8]) << 1) + (((instruction % _32bit[31]) / _32bit[25]) << 5) + \
			((instruction % _32bit[8] / _32bit[7]) << 11) + ((instruction / _32bit[31]) << 12);
		if (imm / _32bit[12]) imm |= 0xFFFFF000;
		printf("[[[[[[ BEQ 값은 %d 입니다. 16진수 값 : %x ]]]]]]\n", imm, imm);
		break;
	case jal:; // uj type
		imm = ((instruction / _32bit[31]) << 20) + ((instruction % _32bit[31] / _32bit[21]) << 1) + \
			((instruction % _32bit[21] / _32bit[20]) << 11) + ((instruction % _32bit[20] / _32bit[12]) << 12);
		if (imm / _32bit[20]) imm |= 0xFFF00000;
		printf("[[[[[[ JAL 값은 %d 입니다. 16진수 값 : %x ]]]]]]\n", imm, imm);
		break;
	}
	// executes, branching, ALU Operation
	if (mode == add || mode == addi || mode == ld || mode == sd) {
		switch (mode) {
		case add:
			execution = regs[rs[1]] + regs[rs[2]];
			break;
		case addi:
			execution = regs[rs[1]] + imm;
			break;
		case ld:
			execution = regs[rs[1]] + imm;
			break;
		case sd:
			execution = regs[rs[1]] + imm;
			break;
		}
	}
	// Branch
	switch (mode) {
		case beq: {
			if (regs[rs[1]] == regs[rs[2]]) pc += (imm);
			else pc = pc + 4;
			return 0;
		}
		case jal: {
			unsigned long old_pc = pc + 4;
			pc += (imm);
			return old_pc;
		}
		case jalr: {
			unsigned long old_pc = pc + 4;
			pc = regs[rs[1]] + imm;
			return old_pc;
		}
		default:
			pc += 4;
			return execution;
	}
}
void mem(long rs[], long* aluResult, int mode) { //access the data memory
	// MEM
	if (mode == ld || mode == sd) {
		switch (mode) {
		case ld: {
			long buffer = data_mem[*aluResult];
			*aluResult = buffer;
			break;
		}
		case sd: {
			data_mem[*aluResult] = regs[rs[2]];
			break;
		}
		}
	}
	// Executes if the instruction mode is ld or sd
}
void wb(long rs[], long aluResult, int mode) { //write result of arithmetic operation or data read from the data memory if required
	// WB
	// save aluResult to rd if mode is jal, jalr, add, addi, ld
	switch (mode) {
	case jalr:;
	case add:;
	case addi:;
	case ld:;
	case jal:
		regs[rs[0]] = aluResult;
		break;
	default:
		break;
	}
	//Write back to 
}

void print_cycles();
void print_reg();
void print_pc();

int main(int ac, char* av[])
{
	// this Code Only detects add, addi, jal, jalr, ld, sd, beq
	if (ac < 3)
	{
		printf("./mips_sim filename mode\n");
		return -1;
	}

	// initialization
	unsigned long instruction = 0;
	long rs[3] = { 0, }; //rs0 = rd, rs1, rs2 
	int mode = 0;
	long aluResult;
	for (int i = 0; i < 32; i++) {
		if (i == 0) _32bit[i] = 1;
		else _32bit[i] = _32bit[i - 1] * 2;
	}

	char done = FALSE;
	if (init(av[1]) != 0)
		return -1;
	while (!done)
	{
		regs[0] = 0; // x0 은 0으로 hard-wire 되어있다.
		instruction = fetch();
		decode(instruction, rs, &mode);
		aluResult = exe(instruction, rs, mode);
		mem(rs, &aluResult, mode);
		wb(rs, aluResult, mode);


		cycles++;    //increase clock cycle

		//if debug mode, print clock cycle, pc, reg 
		if (*av[2] == '0') {
			print_cycles();  //print clock cycles
			print_pc();		 //print pc
			print_reg();	 //print registers
		}

		// check the exit condition, do not delete!! 
		if (regs[9] == 10)  //if value in $t1 is 10, finish the simulation
			done = TRUE;
	}

	if (*av[2] == '1')
	{
		print_cycles();  //print clock cycles
		print_pc();		 //print pc
		print_reg();	 //print registers
	}

	return 0;
}


/* initialize all datapat elements
//fill the instruction and data memory
//reset the registers
*/
int init(char* filename)
{
	FILE* fp = fopen(filename, "r");
	int i;
	long inst;

	if (fp == NULL)
	{
		fprintf(stderr, "Error opening file.\n");
		return -1;
	}

	/* fill instruction memory */
	i = 0;
	while (fscanf(fp, "%lx", &inst) == 1)
	{
		inst_mem[i++] = inst;
	}


	/*reset the registers*/
	for (i = 0; i < 32; i++)
	{
		regs[i] = 0;
	}

	/*reset pc*/
	pc = 0;
	/*reset clock cycles*/
	cycles = 0;
	return 0;
}

void print_cycles()
{
	printf("---------------------------------------------------\n");

	printf("Clock cycles = %lld\n", cycles);
}

void print_pc()
{
	printf("PC	   = %ld\n\n", pc);
}

void print_reg()
{
	printf("x0   = %lld\n", regs[0]);
	printf("x1   = %lld\n", regs[1]);
	printf("x2   = %lld\n", regs[2]);
	printf("x3   = %lld\n", regs[3]);
	printf("x4   = %lld\n", regs[4]);
	printf("x5   = %lld\n", regs[5]);
	printf("x6   = %lld\n", regs[6]);
	printf("x7   = %lld\n", regs[7]);
	printf("x8   = %lld\n", regs[8]);
	printf("x9   = %lld\n", regs[9]);
	printf("x10  = %lld\n", regs[10]);
	printf("x11  = %lld\n", regs[11]);
	printf("x12  = %lld\n", regs[12]);
	printf("x13  = %lld\n", regs[13]);
	printf("x14  = %lld\n", regs[14]);
	printf("x15  = %lld\n", regs[15]);
	printf("x16  = %lld\n", regs[16]);
	printf("x17  = %lld\n", regs[17]);
	printf("x18  = %lld\n", regs[18]);
	printf("x19  = %lld\n", regs[19]);
	printf("x20  = %lld\n", regs[20]);
	printf("x21  = %lld\n", regs[21]);
	printf("x22  = %lld\n", regs[22]);
	printf("x23  = %lld\n", regs[23]);
	printf("x24  = %lld\n", regs[24]);
	printf("x25  = %lld\n", regs[25]);
	printf("x26  = %lld\n", regs[26]);
	printf("x27  = %lld\n", regs[27]);
	printf("x28  = %lld\n", regs[28]);
	printf("x29  = %lld\n", regs[29]);
	printf("x30  = %lld\n", regs[30]);
	printf("x31  = %lld\n", regs[31]);
	printf("\n");
}