/*
 * vm.h
 *
 *  Created on: Dec 12, 2024
 *      Author: Will
 */

#ifndef INC_VM_H_
#define INC_VM_H_

#include "stdint.h"
#include "stm32g4xx_hal.h"

#define BYTECODE_OPCODE_LEN (1)
#define BYTECODE_IMMEDIATE_LEN (4)

struct VM_State {
	uint32_t* call_stack;
	uint32_t* operand_stack;

	uint32_t call_stack_size;
	uint32_t operand_stack_size;

	uint8_t* rom;
	uint32_t rom_size;

	uint32_t instruction_length;

	uint32_t pc;
	uint32_t csp;
	uint32_t osp;
	uint32_t bp;
};

void vm_init(struct VM_State*,
		uint32_t*,
		uint32_t*,
		uint32_t,
		uint32_t,
		uint8_t*,
		uint32_t);

void vm_execute(struct VM_State*);

uint8_t fetch(struct VM_State*);

void advance_pc(struct VM_State*);

uint32_t get_immediate(struct VM_State*);


#endif /* INC_VM_H_ */
