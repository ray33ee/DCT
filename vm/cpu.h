#ifndef CPU_H
#define CPU_H

#define REGISTER_COUNT (64)
#define STACK_SIZE (100)
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
  uint32_t stack[STACK_SIZE];
  uint32_t registers[REGISTER_COUNT];

  uint8_t* rom;

  //Calculate the length of the instruction which includes the opcode and any operands. Used to advance the program counter
  uint32_t instruction_length;


  //CPU registers
  uint32_t* sp; //NOTE: Stack pointer points to one past the top
  uint32_t* lr;
  
  //Status registers
  uint32_t* sign; //1 for positive result, -1 for negative result, 0 for zero result
  uint32_t* compare; //1 for greater, -1 for less than, 0 for equal

  uint32_t* io;

  void mutate_register(uint32_t reg, uint32_t value);

  void set_sign_register(uint32_t result); //Set the sign register based on the result value
  void set_cmp_register(uint32_t result); //Set the compare register based on the result value

  void advance_pc();

  //Access instruction operands
  uint8_t get_register_index();
  uint32_t get_immediate();

  uint8_t fetch();


public: 
  uint32_t* pc;
  
  CPU(uint8_t* r);

  void step();

  void something();
};



#endif
