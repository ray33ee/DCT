
#include "cpu.h"
#include <Arduino.h>

CPU::CPU(uint8_t* r): rom(r) {
  //Clear the stack
  for (int i = 0; i < STACK_SIZE; i++) 
    stack[i] = 0;
  
  //Clear the registers
  for (int i = 0; i < REGISTER_COUNT; i++) 
    registers[i] = 0;

  //Assign aliases to the named registers and status registers
  pc = &registers[0];
  sp = &registers[1];
  lr = &registers[2];

  sign = &registers[3];
  compare = &registers[4];

  io = &registers[5];

}

void CPU::set_sign_register(uint32_t result) {
  int32_t s = (int32_t)result;

  if (s == 0) {
    *sign = 0;
    return;
  }

  if (s < 0) {
    *sign = -1;
  } else {
    *sign = 1;
  }

}

void CPU::set_cmp_register(uint32_t result) {
  int32_t s = (int32_t)result;

  if (s == 0) {
    *compare = 0;
    return;
  }

  if (s < 0) {
    *compare = -1;
  } else {
    *compare = 1;
  }

}

void CPU::mutate_register(uint32_t reg, uint32_t value) {

  // Assign the value to the register
  registers[reg] = value;

  //Update flags
  set_sign_register(value);
}

uint8_t CPU::fetch() {
  //Update the instruction length to include the opcode
  instruction_length = BYTECODE_OPCODE_LEN;
  return rom[*pc];
}

uint8_t CPU::get_register_index() {
  uint8_t v = rom[*pc + instruction_length];
  instruction_length += BYTECODE_REGISTER_LEN;
  return v;
}

uint32_t CPU::get_immediate() {
  uint32_t v = *(uint32_t*)&rom[*pc + instruction_length];
  instruction_length += BYTECODE_IMMEDIATE_LEN;
  return v;
}

void CPU::advance_pc() {
  *pc = *pc + instruction_length;
}

void CPU::step() {
  //fetch
  uint8_t opcode = fetch();

  //decode & execute

  switch (opcode) {
    /* Math */
    case 0: {//ADD Rd, Rr
        uint8_t d = get_register_index();
        uint8_t r = get_register_index();

        mutate_register(d, registers[d] + registers[r]);

        advance_pc();

        break;
    }
    case 1: {//ADDI Rd, K
        uint8_t d = get_register_index();
        uint32_t K = get_immediate();

        mutate_register(d, registers[d] + K);

        advance_pc();

        break;
    }
    case 6: {//MUL Rd, Rr
        uint8_t d = get_register_index();
        uint8_t r = get_register_index();

        mutate_register(d, registers[d] * registers[r]);

        advance_pc();

        break;
    }
    /* Control Flow */
    case 41: {//JMPI K
        uint32_t K = get_immediate();

        *pc = K;

        break;
    }
    case 42: {//CALL K
      uint32_t K = get_immediate();

      //Save the address of the next instruction into the link register
      *lr = *pc + BYTECODE_OPCODE_LEN + BYTECODE_IMMEDIATE_LEN;

      //Jump to the call location
      *pc = K;

      break;
    }
    case 43: {//RET
      *pc = *lr;

      break;
    }
    case 44: {//CMP Rd, Rr
        uint8_t d = get_register_index();
        uint8_t r = get_register_index();

        set_cmp_register(registers[d] - registers[r]);
        set_sign_register(registers[d] - registers[r]);

        advance_pc();

        break;
    }
    case 52: {//JZ K
        uint32_t K = get_immediate();

        if (*sign == 0) {
          *pc = K;
        } else {
          advance_pc();
        }

        break;
    }
    /* Data Transfer */
    case 60: {//MOV Rd, Rr

      uint8_t d = get_register_index();
      uint8_t r = get_register_index();

      mutate_register(d, registers[r]);

      advance_pc();

      break;
    }
    case 61: {//MOVI Rd, K

      uint8_t d = get_register_index();
      uint32_t K = get_immediate();

      mutate_register(d, K);

      advance_pc();

      break;
    }
    case 62: {//PUSH Rd

      uint8_t d = get_register_index();

      stack[*sp] = registers[d];

      *sp += 1;

      advance_pc();

      break;
    }
    case 63: {//POP Rd

      uint8_t d = get_register_index();

      *sp -= 1;

      registers[d] = stack[*sp];

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
    case 81: {//READ K, Rd

      uint32_t pin_number = get_immediate();
      uint8_t d = get_register_index();

      mutate_register(d, digitalRead(pin_number));

      advance_pc();

      break;
    }
    case 82: {//WRITE K, Rd

      uint32_t pin_number = get_immediate();
      uint8_t d = get_register_index();

      digitalWrite(pin_number, registers[d]);

      advance_pc();

      break;
    }
    /* Timing */
    case 100: {//DLA Rd
      uint8_t d = get_register_index();

      delay((unsigned long)registers[d]);

      advance_pc();

      break;
    }
    case 101: {//DLB Rd
      uint8_t d = get_register_index();

      delayMicroseconds((unsigned long)registers[d]);

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
    case 252: {//USR Rd
      uint32_t d = get_register_index();

      Serial.print("r");
      Serial.print(d);
      Serial.print(" = ");
      Serial.print(registers[d]);
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

