
#include "cpu.h"
#include <Arduino.h>

CPU::CPU(uint8_t* r, unsigned int len): rom(r), rom_size(len) {
  //Clear the stack
  for (int i = 0; i < CALL_STACK_SIZE; i++) 
    call_stack[i] = 0;
    
  for (int i = 0; i < OPERAND_STACK_SIZE; i++) 
    operand_stack[i] = 0;

  //Assign aliases to the named registers and status registers
  pc = 0;
  bp = 0;
  csp = -1;
  osp = -1;

}

uint8_t CPU::fetch() {
  //Update the instruction length to include the opcode
  instruction_length = BYTECODE_OPCODE_LEN;
  return rom[pc];
}

uint8_t CPU::get_register_index() {
  uint8_t v = rom[pc + instruction_length];
  instruction_length += BYTECODE_REGISTER_LEN;
  return v;
}

uint32_t CPU::get_immediate() {
  uint32_t v = *(uint32_t*)&rom[pc + instruction_length];
  instruction_length += BYTECODE_IMMEDIATE_LEN;
  return v;
}

void CPU::advance_pc() {
  pc = pc + instruction_length;
}

void CPU::step() {

  if (pc > rom_size) {
    Serial.println("PC out of bounds");
    while (1);
  }

  //fetch
  uint8_t opcode = fetch();

  //decode & execute
  switch (opcode) {
    /* Math */
    case 0: {//ADD
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] + operand_stack[osp+1];

        advance_pc();

        break;
    }
    case 1: {//SUB
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] - operand_stack[osp+1];

        advance_pc();

        break;
    }
    case 2: {//MUL
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] * operand_stack[osp+1];

        advance_pc();

        break;
    }
	/* Bitwise Logic */
    case 20: {//OR
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] | operand_stack[osp+1];

        advance_pc();

        break;
    }
    case 21: {//XOR
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] ^ operand_stack[osp+1];

        advance_pc();

        break;
    }
    case 22: {//AND
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] & operand_stack[osp+1];

        advance_pc();

        break;
    }
    case 23: {//INV
        operand_stack[osp] = ~operand_stack[osp];

        advance_pc();

        break;
    }
    case 24: {//NEG
        operand_stack[osp] = -operand_stack[osp];

        advance_pc();

        break;
    }
    case 25: {//SHL
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] << operand_stack[osp+1];

        advance_pc();

        break;
    }
    case 26: {//SHR
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] >> operand_stack[osp+1];

        advance_pc();

        break;
    }
    case 27: {//BOOL
		if (operand_stack[osp])
			operand_stack[osp] = 1
		
		advance_pc();

		break;
    }
    case 28: {//NOT
		if (operand_stack[osp])
			operand_stack[osp] = 0
		else
			operand_stack[osp] = 1
		
		advance_pc();

		break;
    }
    /* Control Flow */
    case 40: {//JMP K
        uint32_t K = get_immediate();

        pc = K;

        break;
    }
    case 41: {//ALLOC K
        uint32_t K = get_immediate();

        csp += 1;
        call_stack[csp] = bp;

        bp = csp;

        csp += K;

        advance_pc();

        break;
    }
    case 42: {//CALL K
      uint32_t K = get_immediate();

      //get the link address
      uint32_t link = pc + BYTECODE_OPCODE_LEN + BYTECODE_IMMEDIATE_LEN;

      // Push the link on the call stack
      csp += 1;
      call_stack[csp] = link;

      //Jump to the call location
      pc = K;

      break;
    }
    case 43: {//RET K
      uint32_t K = get_immediate();

      //Free local variables
      csp = bp;

      //Pop old base pointer off stack
      bp = call_stack[csp];
      csp -= 1;

      //Pop link address
      uint32_t link = call_stack[csp];
      csp -= 1;

      //Drop parameters passed into function, if any
      csp -= K;

      pc = link;

      break;
    }
    case 44: {//EQ
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] == operand_stack[osp+1];

        Serial.println(instruction_length);

        advance_pc();

        break;
    }
    case 45: {//NE
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] != operand_stack[osp+1];

        Serial.println(instruction_length);

        advance_pc();

        break;
    }
    case 46: {//GT
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] > operand_stack[osp+1];

        Serial.println(instruction_length);

        advance_pc();

        break;
    }
    case 47: {//LT
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] < operand_stack[osp+1];

        Serial.println(instruction_length);

        advance_pc();

        break;
    }
    case 48: {//GE
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] >= operand_stack[osp+1];

        Serial.println(instruction_length);

        advance_pc();

        break;
    }
    case 49: {//LE
        osp -= 1;
        operand_stack[osp] = operand_stack[osp] <= operand_stack[osp+1];

        Serial.println(instruction_length);

        advance_pc();

        break;
    }
    case 50: {//JT K
        uint32_t K = get_immediate();

        if (operand_stack[osp] != 0)
            pc = K;
		else
			advance_pc();

		osp -= 1;

        break;
    }
    case 51: {//JF K
        uint32_t K = get_immediate();

        if (operand_stack[osp] == 0)
            pc = K;
		else
			advance_pc();

		osp -= 1;

        break;
    }
    /* Data Transfer */
    case 60: {//PUSHI K
        uint32_t K = get_immediate();


        osp += 1;
        operand_stack[osp] = K;

        advance_pc();

        break;
    }
    case 61: {//POPC

        csp += 1;
        call_stack[csp] = operand_stack[osp];
        osp -= 1;

        advance_pc();

        break;
    }
    case 62: {//PUSHL K
        uint32_t K = get_immediate();

        osp += 1;
        operand_stack[osp] = call_stack[bp+K];

        advance_pc();
    
        break;
    }
    case 63: {//POPL K
        uint32_t K = get_immediate();

        call_stack[bp+K] = operand_stack[osp];

        osp -= 1;



        advance_pc();
        break;
    }
    case 64: {//DROP

        osp -= 1;

        advance_pc();
        break;
    }

    /* GPIO */
    case 80: {//SETUP
		osp -= 2;
		uint32_t pin_number = operand_stack[osp+1];
		uint32_t pin_mode = operand_stack[osp+2];

		if (pin_mode == PIN_INPUT) {
			pinMode(pin_number, INPUT);
		} else if (pin_mode == PIN_INPUT_PULLUP) {
			pinMode(pin_number, INPUT_PULLUP);
		}  else if (pin_mode == PIN_OUTPUT) {
			pinMode(pin_number, OUTPUT);
		} 

		advance_pc();

		break;
    }
    case 81: {//READ
      
      operand_stack[osp] = digitalRead(operand_stack[osp]);

      advance_pc();

      break;
    }
    case 82: {//WRITE
      
      osp -= 1;
      digitalWrite(operand_stack[osp], operand_stack[osp+1]);

      advance_pc();

      break;
    }
    /* Timing */
    case 100: {//DLA

      delay((unsigned long)operand_stack[osp]);
      osp -= 1;

      advance_pc();

      break;
    }
    case 101: {//DLB

      delayMicroseconds((unsigned long)operand_stack[osp]);
      osp -= 1;

      advance_pc();

      break;
    }
    /* Misc */
    case 200: {//HALT

		Serial.println("Halting");

      while (1) {
        delay(1000);
      }

      break;
    }
    case 201: {//UBW K
      uint32_t K = get_immediate();

      //This instruction doesn't setup the serial properly idk why
      //Serial.begin(K);

      //while (!Serial);
      
      advance_pc();

      break;
    }
    case 202: {//SPEEK
      uint32_t peek = operand_stack[osp];

      Serial.print("Peek: ");
      Serial.print(peek);
      Serial.println();
      
      advance_pc();

      break;
    }
    case 203: {//SPOP
      uint32_t top = operand_stack[osp];
	  osp -= 1;

      Serial.print("Pop: ");
      Serial.print(top);
      Serial.println();
      
      advance_pc();

      break;
    }
    default: {
      Serial.print("Invalid command - ");
      Serial.println(opcode);

      break;
    }

  }
    
}

