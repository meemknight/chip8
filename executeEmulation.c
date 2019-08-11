#include "executeEmulation.h"
#include "cpu.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>

void execute(const char * p, long size) 
{
	
	srand(time(0));
	//resetting the state

	//reserved from 0 to 0x1ff
	uint8_t memory[4096];
	memset(memory, 0, sizeof(memory));
	//copying the roam
	memcpy(&memory[512], p, size);

	regs_t regs;
	memset(&regs, 0, sizeof(regs));
	regs.pc = 512;

	uint16_t stack[STACK_SIZE];
	memset(stack, 0, sizeof(stack));

	///milliseconds
	int deltaTime = 0;
	int time1 = clock(0);
	int time2 = clock(0);
	
	const int frameDuration = (1.f / PROCESSOR_CLOCK_SPEED) * 1000;

	while(1)
	{
		time2 = clock(0);
		deltaTime += time2 - time1;
		time1 = clock(0);
	
		if(deltaTime > frameDuration)
		{
			//cpu logic
				
			executeInstruction(memory, &regs, stack);

			//todo implement sound, properly implement duration
			if(regs.dt != 0)
			{
				regs.dt--;
			}

			if (regs.st != 0)
			{
				regs.st--;
			}

			deltaTime = 0;
		}
	
	}


}

void displayRegs(regs_t *r, uint16_t *stack, long stackSize)
{
	for (int i = 0; i < 16; i++)
	{
		printf("v%X %u\n",i , r->v[i]);
	}
	puts("");

	for (int i = 0; i < stackSize; i++)
	{
		printf("s%.2d %u\n", i, stack[i]);
	}
	puts("");
	printf("cpu count: %u\n", r->cpuCount);
};

void yieldError(regs_t *r, uint16_t *stack, const char *error)
{
	puts("");
	puts(error);
	displayRegs(r, stack, STACK_SIZE);
	getchar();
}

char bindings[] = {'x', '1', '2', '3', 'q', 'w', 'e', 'a', 's', 'd', 'z', 'c', '4', 'r', 'f', 'v'};

int isButtonPressed(int button)
{
	return GetAsyncKeyState(bindings[button]);
}