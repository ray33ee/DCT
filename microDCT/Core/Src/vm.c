/*
 * vm.c
 *
 *  Created on: Dec 12, 2024
 *      Author: Will
 */

#include "vm.h"

uint8_t __vm_fetch(struct VM_State* state) {
	state->instruction_length = BYTECODE_OPCODE_LEN;
	return state->rom[state->pc];
}

void advance_pc(struct VM_State* state) {
	state->pc = state->pc + state->instruction_length;
}

uint32_t get_immediate(struct VM_State* state) {
	uint32_t v = *(uint32_t*)&state->rom[state->pc + state->instruction_length];
	state->instruction_length += BYTECODE_IMMEDIATE_LEN;
	return v;
}

/* Create a new VM */
void vm_init(
		struct VM_State* state,
		uint32_t* _call_stack,
		uint32_t* _operand_stack,
		uint32_t _call_stack_size,
		uint32_t _operand_stack_size,
		uint8_t* _rom,
		uint32_t _rom_size) {
	state->call_stack = _call_stack;
	state->operand_stack = _operand_stack;
	state->call_stack_size = _call_stack_size;
	state->operand_stack_size = _operand_stack_size;
	state->rom = _rom;
	state->rom_size = _rom_size;
	state->pc = 0;
	state->bp = 0;
	state->csp = -1;
	state->osp = -1;



}

/* Execute the next instruction */
void vm_execute(struct VM_State* state) {

	if (state->pc > state->rom_size) {
		//error
	}

	uint8_t opcode = __vm_fetch(state);

	switch (opcode) {
		/* Math */
	    case 0: {//ADD
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] + state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 1: {//SUB
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] - state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 2: {//MUL
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] * state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    /* Bitwise Logic */
	    case 20: {//OR
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] | state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 21: {//XOR
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] ^ state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 22: {//AND
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] & state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 23: {//INV
	        state->operand_stack[state->osp] = ~state->operand_stack[state->osp];

	        advance_pc(state);

	        break;
	    }
	    case 24: {//NEG
	        state->operand_stack[state->osp] = -state->operand_stack[state->osp];

	        advance_pc(state);

	        break;
	    }
	    case 25: {//SHL
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] << state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 26: {//SHR
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] >> state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 27: {//BOOL
	        if (state->operand_stack[state->osp])
	            state->operand_stack[state->osp] = 1;

	        advance_pc(state);

	        break;
	    }
	    case 28: {//NOT
	        if (state->operand_stack[state->osp])
	            state->operand_stack[state->osp] = 0;
	        else
	            state->operand_stack[state->osp] = 1;

	        advance_pc(state);

	        break;
	    }
	    /* Control Flow */
	    case 40: {//JMP K
	        uint32_t K = get_immediate(state);

	        state->pc = K;

	        break;
	    }
	    case 41: {//ALLOC K
	        uint32_t K = get_immediate(state);

	        state->csp += 1;
	        state->call_stack[state->csp] = state->bp;

	        state->bp = state->csp;

	        state->csp += K;

	        advance_pc(state);

	        break;
	    }
	    case 42: {//CALL K
	      uint32_t K = get_immediate(state);

	      //get the link address
	      uint32_t link = state->pc + BYTECODE_OPCODE_LEN + BYTECODE_IMMEDIATE_LEN;

	      // Push the link on the call stack
	      state->csp += 1;
	      state->call_stack[state->csp] = link;

	      //Jump to the call location
	      state->pc = K;

	      break;
	    }
	    case 43: {//RET K
	      uint32_t K = get_immediate(state);

	      //Free local variables
	      state->csp = state->bp;

	      //Pop old base pointer off stack
	      state->bp = state->call_stack[state->csp];
	      state->csp -= 1;

	      //Pop link address
	      uint32_t link = state->call_stack[state->csp];
	      state->csp -= 1;

	      //Drop parameters passed into function, if any
	      state->csp -= K;

	      state->pc = link;

	      break;
	    }
	    case 44: {//EQ
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] == state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 45: {//NE
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] != state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 46: {//GT
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] > state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 47: {//LT
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] < state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 48: {//GE
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] >= state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 49: {//LE
	        state->osp -= 1;
	        state->operand_stack[state->osp] = state->operand_stack[state->osp] <= state->operand_stack[state->osp+1];

	        advance_pc(state);

	        break;
	    }
	    case 50: {//JT K
	        uint32_t K = get_immediate(state);

	        if (state->operand_stack[state->osp] != 0)
	        	state->pc = K;
	        else
	            advance_pc(state);

	        state->osp -= 1;

	        break;
	    }
	    case 51: {//JF K
	        uint32_t K = get_immediate(state);

	        if (state->operand_stack[state->osp] == 0)
	        	state->pc = K;
	        else
	            advance_pc(state);

	        state->osp -= 1;

	        break;
	    }
	    /* Data Transfer */
	    case 60: {//PUSHI K
	        uint32_t K = get_immediate(state);


	        state->osp += 1;
	        state->operand_stack[state->osp] = K;

	        advance_pc(state);

	        break;
	    }
	    case 61: {//POPC

	        state->csp += 1;
	        state->call_stack[state->csp] = state->operand_stack[state->osp];
	        state->osp -= 1;

	        advance_pc(state);

	        break;
	    }
	    case 62: {//PUSHL K
	        uint32_t K = get_immediate(state);

	        state->osp += 1;
	        state->operand_stack[state->osp] = state->call_stack[state->bp+K];

	        advance_pc(state);

	        break;
	    }
	    case 63: {//POPL K
	        uint32_t K = get_immediate(state);

	        state->call_stack[state->bp+K] = state->operand_stack[state->osp];

	        state->osp -= 1;



	        advance_pc(state);
	        break;
	    }
	    case 64: {//DROP

	        state->osp -= 1;

	        advance_pc(state);
	        break;
	    }

	    /* GPIO */
	    /* Timing */
	    case 100: {//DLA

	      HAL_Delay(state->operand_stack[state->osp]);
	      state->osp -= 1;

	      advance_pc(state);

	      break;
	    }
	    /* Misc */
	    case 200: {//HALT

	        printf("Halting\n");

	      while (1) {
	        HAL_Delay(1000);
	      }

	      break;
	    }
	    case 202: {//SPEEK
	      uint32_t peek = state->operand_stack[state->osp];

	      printf("Peek: %i\n", peek);

	      advance_pc(state);

	      break;
	    }
	    case 203: {//SPOP
	      uint32_t top = state->operand_stack[state->osp];
	      state->osp -= 1;

	      printf("Pop: %i\n", top);

	      advance_pc(state);

	      break;
	    }
	    default: {
	    	printf("Invalid command - (%i)\n", opcode);

	      break;
	    }

	  }



}