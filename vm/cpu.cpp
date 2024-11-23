
#include "cpu.h"
#include <Arduino.h>

CPU::CPU(uint8_t* r): rom(r) {
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

    /* GPIO */
    case 80: {//SETUP K, K
      uint32_t pin_number = get_immediate();
      uint32_t pin_mode = get_immediate();

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
    case 81: {//READ K

      uint32_t pin_number = get_immediate();
      
      osp += 1;
      operand_stack[osp] = digitalRead(pin_number);


      advance_pc();

      break;
    }
    case 82: {//WRITE K

      uint32_t pin_number = get_immediate();
      


      digitalWrite(pin_number, operand_stack[osp]);
      osp -= 1;

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
    case 250: {//HALT

      while (1) {
        delay(1000);
      }

      break;
    }
    case 251: {//UBW K
      uint32_t K = get_immediate();

      //This instruction doesn't setup the serial properly idk why
      //Serial.begin(K);

      //while (!Serial);
      
      advance_pc();

      break;
    }
    case 252: {//USR
      uint32_t peek = operand_stack[osp];

      Serial.print("Peek: ");
      Serial.print(peek);
      Serial.println();
      
      advance_pc();

      break;
    }
    default: {
      Serial.println("Invalid command");

      break;
    }

  }
    
}

