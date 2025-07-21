/*
 * vm.c
 *
 *  Created on: Dec 12, 2024
 *      Author: Will
 */

#include "vm.h"

uint8_t __vm_fetch(struct VM_State* state) {
	state->instruction_length = BYTECODE_OPCODE_LEN;
	return state->exec->rom[state->pc];
}

void advance_pc(struct VM_State* state) {
	state->pc = state->pc + state->instruction_length;
}

uint32_t get_immediate(struct VM_State* state) {
	uint32_t v = *(uint32_t*)&state->exec->rom[state->pc + state->instruction_length];
	state->instruction_length += BYTECODE_IMMEDIATE_LEN;
	return v;
}

uint32_t vm_peek_ops(struct VM_State* state) {
	return state->operand_stack[state->osp];
}

/* Create a new VM */
void vm_init(
		struct VM_State* state,
		uint32_t* _call_stack,
		uint32_t* _operand_stack,
		uint32_t _call_stack_size,
		uint32_t _operand_stack_size,
		struct Executable_State* _exec,
		struct PP_HANDLE* _pp) {

	state->call_stack = _call_stack;
	state->operand_stack = _operand_stack;
	state->call_stack_size = _call_stack_size;
	state->operand_stack_size = _operand_stack_size;
	state->exec = _exec;
	state->pp = _pp;

	vm_reset(state);
}

void vm_reset(struct VM_State* state) {
	state->pc = 0;
	state->bp = 0;
	state->csp = -1;
	state->osp = -1;
}

void vm_start_timer(struct VM_State* state)  {
	state->start_tick = HAL_GetTick();
}

void vm_set(struct VM_State* state, uint32_t pin) {
	pp_set(pin);


	advance_pc(state);
}

void vm_rst(struct VM_State* state, uint32_t pin) {
	pp_reset(pin);

	advance_pc(state);
}

void vm_get(struct VM_State* state, uint32_t pin) {
	state->osp += 1;

	state->operand_stack[state->osp] = pp_read(pin);

	advance_pc(state);
}

/* Execute the next instruction */
uint32_t vm_execute(struct VM_State* state) {


	if (state->pc >= state->exec->rom_length) {
		//error
		return PC_OVERLOW;
	}

	if (state->exec->rom_length == 0) {
		//No code is loaded, error
	}

	if (state->osp > state->operand_stack_size - 10 && state->osp != -1) {
		//If the OSP is close to the end of the op stack, raise a op stack overflow
		return OP_STACK_OVERFLOW;
	}

	if (state->csp > state->call_stack_size - 10 && state->csp != -1) {
		//If the csp is close to the end of the call stack, raise a call stack overflow
		return CALL_STACK_OVERFLOW;
	}

	if (HAL_GetTick() - state->start_tick > state->timeout) {
		return TIMEOUT_EXCEEDED;
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
	    case 3: {//RNG
	        state->osp += 1;
	        state->operand_stack[state->osp] = rng_global_next32();

	        advance_pc(state);

	        break;
	    }
	    case 4: {//SEED

	    	uint32_t seed = state->operand_stack[state->osp];

	        state->osp -= 1;

	    	rng_global_seed(seed);

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
	    case 65: {//PUSHG K
	        uint32_t K = get_immediate(state);

	        state->osp += 1;
	        state->operand_stack[state->osp] = state->call_stack[K];

	        advance_pc(state);

	        break;
	    }
	    case 66: {//POPG K
	        uint32_t K = get_immediate(state);

	        state->call_stack[K] = state->operand_stack[state->osp];

	        state->osp -= 1;



	        advance_pc(state);
	        break;
	    }
	    case 67: {//PUSHM K

	    	//Virtual ports are represented by a pointer to the location in the stack that contains the information
	    	//the the VP was constructed with.
	    	// PUSHM constructs this pointer by subtracting the offset from the current stack pointer, and adds this
	    	// pointer to the stack where it can be assigned to global variables.

	        uint32_t K = get_immediate(state);

	        //Get a pointer minus the K offset, which should give a pointer to the first data in the VP
	        uint32_t* start = &state->operand_stack[state->osp-K];

	        state->osp += 1;
	        state->operand_stack[state->osp] = start;

	        advance_pc(state);

	        break;
	    }

	    /* GPIO */
	    case 81: {//READ

	    	uint32_t pin_number = state->operand_stack[state->osp];

	    	uint32_t result = (uint32_t)pp_read(pin_number);

	    	state->operand_stack[state->osp] = result;

	        advance_pc(state);

	    	break;
	    }
	    case 82: {//WRITE

			uint32_t pin_level = state->operand_stack[state->osp];

	        state->osp -= 1;

	        uint32_t pin_number = state->operand_stack[state->osp];

	        state->osp -= 1;

	        pp_write(pin_number, (GPIO_PinState)pin_level);

	        advance_pc(state);

	    	break;
	    }
	    /* Virtual Ports */
	    case 90: {//VPO





			uint32_t* pointer = (uint32_t*)state->operand_stack[state->osp];

	    	struct VP_DATA vp = { pointer };

	        state->osp -= 1;

	        uint32_t value = state->operand_stack[state->osp];

	        state->osp -= 1;

	        vp_write_port(&vp, value);



	    	advance_pc(state);

			break;
	    }
	    case 91: {//VPI
	    	uint32_t* pointer = (uint32_t*)state->operand_stack[state->osp];

	    	struct VP_DATA vp = { pointer };


	    	state->operand_stack[state->osp] = vp_read_port(&vp);



	    	advance_pc(state);

			break;
	    }
	    /* Timing */
	    case 100: {//DLA

	    	uint32_t delay = state->operand_stack[state->osp];

	      HAL_Delay(delay);
	      state->osp -= 1;

	      advance_pc(state);

	      break;
	    }
	    case 101: {//DLB

	    	uint32_t delay = state->operand_stack[state->osp];

	    	misc_delay_us(delay);
	      state->osp -= 1;

	      advance_pc(state);

	      break;
	    }
	    /* Set */
	    case 111: { //SET1
	    	vm_set(state, 0);
	    	break;
	    }
	    case 112: { //SET2
	    	vm_set(state, 1);
	    	break;
	    }
	    case 113: { //SET3
	    	vm_set(state, 2);
	    	break;
	    }
	    case 114: { //SET4
	    	vm_set(state, 3);
	    	break;
	    }
	    case 115: { //SET5
	    	vm_set(state, 4);
	    	break;
	    }
	    case 116: { //SET6
	    	vm_set(state, 5);
	    	break;
	    }
	    case 117: { //SET7
	    	vm_set(state, 6);
	    	break;
	    }
	    case 118: { //SET8
	    	vm_set(state, 7);
	    	break;
	    }
	    case 119: { //SET9
	    	vm_set(state, 8);
	    	break;
	    }
	    case 120: { //SET10
	    	vm_set(state, 9);
	    	break;
	    }
	    case 121: { //SET11
	    	vm_set(state, 10);
	    	break;
	    }
	    case 122: { //SET12
	    	vm_set(state, 11);
	    	break;
	    }
	    case 123: { //SET13
	    	vm_set(state, 12);
	    	break;
	    }
	    case 124: { //SET14
	    	vm_set(state, 13);
	    	break;
	    }
	    case 125: { //SET15
	    	vm_set(state, 14);
	    	break;
	    }
	    case 126: { //SET16
	    	vm_set(state, 15);
	    	break;
	    }
	    /* Reset */

	    case 141: { //RST1
	    	vm_rst(state, 0);
	    	break;
	    }
	    case 142: { //RST2
	    	vm_rst(state, 1);
	    	break;
	    }
	    case 143: { //RST3
	    	vm_rst(state, 2);
	    	break;
	    }
	    case 144: { //RST4
	    	vm_rst(state, 3);
	    	break;
	    }
	    case 145: { //RST5
	    	vm_rst(state, 4);
	    	break;
	    }
	    case 146: { //RST6
	    	vm_rst(state, 5);
	    	break;
	    }
	    case 147: { //RST7
	    	vm_rst(state, 6);
	    	break;
	    }
	    case 148: { //RST8
	    	vm_rst(state, 7);
	    	break;
	    }
	    case 149: { //RST9
	    	vm_rst(state, 8);
	    	break;
	    }
	    case 150: { //RST10
	    	vm_rst(state, 9);
	    	break;
	    }
	    case 151: { //RST11
	    	vm_rst(state, 10);
	    	break;
	    }
	    case 152: { //RST12
	    	vm_rst(state, 11);
	    	break;
	    }
	    case 153: { //RST13
	    	vm_rst(state, 12);
	    	break;
	    }
	    case 154: { //RST14
	    	vm_rst(state, 13);
	    	break;
	    }
	    case 155: { //RST15
	    	vm_rst(state, 14);
	    	break;
	    }
	    case 156: { //RST16
	    	vm_rst(state, 15);
	    	break;
	    }
	    /* Read */
	    case 171: { //GET1
	    	vm_get(state, 0);
	    	break;
	    }
	    case 172: { //GET2
	    	vm_get(state, 1);
	    	break;
	    }
	    case 173: { //GET3
	    	vm_get(state, 2);
	    	break;
	    }
	    case 174: { //GET4
	    	vm_get(state, 3);
	    	break;
	    }
	    case 175: { //GET5
	    	vm_get(state, 4);
	    	break;
	    }
	    case 176: { //GET6
	    	vm_get(state, 5);
	    	break;
	    }
	    case 177: { //GET7
	    	vm_get(state, 6);
	    	break;
	    }
	    case 178: { //GET8
	    	vm_get(state, 7);
	    	break;
	    }
	    case 179: { //GET9
	    	vm_get(state, 8);
	    	break;
	    }
	    case 180: { //GET10
	    	vm_get(state, 9);
	    	break;
	    }
	    case 181: { //GET11
	    	vm_get(state, 10);
	    	break;
	    }
	    case 182: { //GET12
	    	vm_get(state, 11);
	    	break;
	    }
	    case 183: { //GET13
	    	vm_get(state, 12);
	    	break;
	    }
	    case 184: { //GET14
	    	vm_get(state, 13);
	    	break;
	    }
	    case 185: { //GET15
	    	vm_get(state, 14);
	    	break;
	    }
	    case 186: { //GET16
	    	vm_get(state, 15);
	    	break;
	    }

	    /* Misc */
	    case 201: {//SUC

	    	return SUCCESS;

	      break;
	    }
	    case 202: {//FAIL

	    	return FAILURE;

	      break;
	    }
	    default: {
	    	printf("Invalid command - (%i)\n", (int)opcode);

	      break;
	    }

	  }


	return RUNNING;

}
