#include "cpu.h"

CPU* cpu;

void setup() {



  //uint8_t t[22] = {80, 13, 0, 0, 0, 3, 0, 0, 0, 61, 12, 1, 0, 0, 0, 82, 13, 0, 0, 0, 12, 250, };
  uint8_t t[34] = {61, 26, 4, 0, 0, 0, 60, 6, 26, 42, 20, 0, 0, 0, 60, 27, 6, 252, 27, 250, 62, 26, 60, 26, 6, 6, 26, 26, 60, 6, 26, 63, 26, 43, };
  
  Serial.begin(9600);

  while (!Serial);

  cpu = new CPU(t);
}

void loop() {
  cpu->step();
  //Serial.println(*cpu->pc);
}
