#include "cpu.h"

CPU* cpu;

uint8_t t[86] = {42, 6, 0, 0, 0, 200, 41, 1, 0, 0, 0, 60, 13, 0, 0, 0, 63, 1, 0, 0, 0, 62, 1, 0, 0, 0, 60, 3, 0, 0, 0, 80, 60, 1, 0, 0, 0, 51, 81, 0, 0, 0, 62, 1, 0, 0, 0, 60, 1, 0, 0, 0, 82, 60, 244, 1, 0, 0, 100, 62, 1, 0, 0, 0, 60, 0, 0, 0, 0, 82, 60, 244, 1, 0, 0, 100, 40, 32, 0, 0, 0, 43, 0, 0, 0, 0, };

void setup() {

  Serial.begin(9600);

  while (!Serial);

  cpu = new CPU(t, sizeof(t));
}

void loop() {
  cpu->step();
  Serial.print(cpu->pc);
  Serial.print(" ");
  Serial.println(t[cpu->pc]);
}
