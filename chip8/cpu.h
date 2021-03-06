#pragma once
#include "executeEmulation.h"

///in hz
#define PROCESSOR_CLOCK_SPEED 60

void executeInstruction(unsigned char *c, regs_t *regs, uint16_t *stack, char *screen);