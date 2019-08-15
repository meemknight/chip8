#include "executeEmulation.h"
#include "cpu.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>

 void setFontSize(int a, int b)
{

	HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_FONT_INFOEX consoleFont;

	consoleFont.cbSize = sizeof(CONSOLE_FONT_INFOEX);

	GetCurrentConsoleFontEx(stdOut, 0, &consoleFont);

	consoleFont.dwFontSize.X = a;
	consoleFont.dwFontSize.Y = b;

	SetCurrentConsoleFontEx(stdOut, 0, &consoleFont);

}

char *getPixel(int x, int y, char *buffer)
{
	x = x % 64;
	y = y % 32;

	return &buffer[x + (y * 64)];
}

 const uint8_t sprites[] = { 
	 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	 0x20, 0x60, 0x20, 0x20, 0x70, // 1
	 0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	 0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	 0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	 0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	 0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	 0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	 0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	 0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	 0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	 0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	 0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	 0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	 0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	 0xF0, 0x80, 0xF0, 0x80, 0x80, // F
 };

void execute(const char * p, long size) 
{

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	setFontSize(18, 18);

	srand(time(0));
	//resetting the state

	//reserved from 0 to 0x1ff
	uint8_t memory[4096];
	memset(memory, 0, sizeof(memory));

	//adding characters sprites
	memcpy(memory, sprites, sizeof(sprites));

	//copying the roam
	memcpy(&memory[512], p, size);

	regs_t regs;
	memset(&regs, 0, sizeof(regs));
	regs.pc = 512;

	uint16_t stack[STACK_SIZE];
	memset(stack, 0, sizeof(stack));

	char screen[32 * 64];
	for (int i = 0; i < 32 * 64; i++)
	{
		screen[i] = ' ';
	}

	///milliseconds
	int deltaTime = 0;
	int time1 = clock(0);
	int time2 = clock(0);
	int time = 16;

	const int frameDuration = (1.f / PROCESSOR_CLOCK_SPEED) * 1000;

	while(1)
	{
		time2 = clock(0);
		deltaTime += time2 - time1;
		time1 = clock(0);
	
		if(deltaTime > frameDuration)
		{
			//cpu logic
				
			executeInstruction(memory, &regs, stack, screen);

			if(regs.shouldRedraw)
			{
				redrawScreen(screen, h);
				regs.shouldRedraw = 0;
			}

			//todo implement sound, properly implement duration
			time -= deltaTime;

			if(time <= 0)
			{
				time = 16 + time;
				if (regs.dt != 0)
				{
					regs.dt--;
					if(regs.dt == 0)
					{
						Beep(240, 10);
					}
				}

				if (regs.st != 0)
				{
					regs.st--;
				}
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

char bindings[] = {'X', '1', '2', '3', 'Q', 'W', 'E', 'A', 'S', 'D', 'Z', 'C', '4', 'R', 'F', 'V'};

int isButtonPressed(int button)
{

	if (GetAsyncKeyState(bindings[button]))
	{
		return 1;
	}else
	{
		return 0;
	}	

}

void redrawScreen(char * c, HANDLE h)
{
	COORD begin;
	begin.X = 0;
	begin.Y = 0;
	SetConsoleCursorPosition(h, begin);
	//system("cls");

	char copy[2] = { BLOCK_CHARACTER,0 };

	for(int i=0; i<64; i++)
	{
		printf(copy);
	}
	printf("\n");
	
	for(int y=0; y<32; y++)
	{
		for(int x=0; x<64; x++)
		{
			copy[0] = c[x + (y * 64)];
			printf(copy);
		}
		printf("\n");
	}

}
