#ifndef CPU_H
#define CPU_H

#define REGISTER_COUNT (64)
#define CALL_STACK_SIZE (100)
#define OPERAND_STACK_SIZE (100)
#define ROM_SIZE (200)

//Instruction opcode and operand lengths
#define BYTECODE_OPCODE_LEN (1)
#define BYTECODE_IMMEDIATE_LEN (4)
#define BYTECODE_REGISTER_LEN (1)


#define PIN_INPUT (1)
#define PIN_INPUT_PULLUP (2)
#define PIN_OUTPUT (3)

#include "stdint.h"

class CPU {
private:
  uint32_t call_stack[CALL_STACK_SIZE];
  uint32_t operand_stack[OPERAND_STACK_SIZE];
  uint32_t registers[REGISTER_COUNT];

  uint8_t* rom;
  unsigned int rom_size;

  //Calculate the length of the instruction which includes the opcode and any operands. Used to advance the program counter
  uint32_t instruction_length;

  //CPU registers
  uint32_t bp;
  uint32_t csp;
  uint32_t osp;


  void advance_pc();

  //Access instruction operands
  uint8_t get_register_index();
  uint32_t get_immediate();

  uint8_t fetch();


public: 
  uint32_t pc;
  
  CPU(uint8_t* r, unsigned int);

  void step();

  void something();
};



#endif
