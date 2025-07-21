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
#include "rng.h"
#include "executable.h"
#include "pp.h"
#include "vp.h"

#define BYTECODE_OPCODE_LEN (1)
#define BYTECODE_IMMEDIATE_LEN (4)

#define RUNNING 				(0)
#define SUCCESS 				(1)
#define FAILURE 				(2)
#define OP_STACK_OVERFLOW 		(3)
#define CALL_STACK_OVERFLOW 	(4)
#define TIMEOUT_EXCEEDED 		(5)
#define PC_OVERLOW				(6) //Program pointer points beyond program memory

struct VM_State {
	uint32_t* call_stack;
	uint32_t* operand_stack;

	uint32_t call_stack_size;
	uint32_t operand_stack_size;

	struct Executable_State* exec;

	uint32_t instruction_length;

	uint32_t pc;
	uint32_t csp;
	uint32_t osp;
	uint32_t bp;

	uint32_t timeout;
	uint32_t start_tick;

	uint32_t global_count;

	struct PP_HANDLE* pp;
};

void vm_init(struct VM_State*,
		uint32_t*,
		uint32_t*,
		uint32_t,
		uint32_t,
		struct Executable_State*,
		struct PP_HANDLE*);

void vm_start_timer(struct VM_State*);

uint32_t vm_execute(struct VM_State*);

uint32_t vm_peek_ops(struct VM_State*);

void vm_reset(struct VM_State*);

uint8_t fetch(struct VM_State*);

void advance_pc(struct VM_State*);

uint32_t get_immediate(struct VM_State*);

void vm_set(struct VM_State*, uint32_t);

void vm_rst(struct VM_State*, uint32_t);

void vm_get(struct VM_State*, uint32_t);


#endif /* INC_VM_H_ */
