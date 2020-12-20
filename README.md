# RISC-V-Simulator
Simple Simulator of RISC - V
# Index
1. Project Introduction
2. Code Explanation


# 1. Project Introduction
 In this Project, I made the Simple Single Cycle RISC-V Simulator which can execute add, addi, beq, jal, jalr, ld and sd.
 I implemented the part of the source code which is the procedures of below. the main skeleton code is given by default from my class.
My code are consisted of 5 modules, according to Pipelining Model of RISC-V, which is ‘Instruction Fetch’, ‘Decoding’, ‘Execution Part’, ‘Memory’, ‘Write Back’

## 2. Code Explanation
 Before Explanation, Let me Introduce Some Modification of Skeleton Code.

The Intialization part, I made the array that indicates 32 bits from LSB to [31], because of easier decoding.

## Instruction Fetch
 The computer’s basic unit of address is 8 bit, which is 1 word.
The RISC-V uses 4 word instruction, so Program Counter is multiple of 4.
But this program, which is simulation are reading hex one instruction by one.
## Decoding
 In Decoding Part, the module should decode the instruction and read data from register file. In this section, we should specify which registers are being used in execution, loading, etc, and the important part, this module also should specify which the instruction is. For example, if instruction indicates addi, we should make an display that this instruction is addi. so, I defiend enum type variable which indicates each types of instructions.

## Execution Part
 The Execution Part Mainly does Immediate Value Creation, Execution, Program Counter Modification.
First, Immediate Value Creation makes immediate value into 64 – bit integer value.
Second, Execution Part Does arithmetic operation ordered by the instruction, whether the operend is two register value, or one register value and one immediate value.
Third, the branch operation part occurs. if beq, then if two register value are same, the program counter indicates the value of program counter that is added to immediate value.
Fianlly, the execution module returns value which is execution value or value of pc + 4 for just in case and return addres.

## Memory
If instruction is ld or sd, then Memory Procedure does Loading and Storing.

## Write Back	
