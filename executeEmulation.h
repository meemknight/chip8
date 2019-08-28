#pragma once
#include <stdint.h>
#include <Windows.h>

#define STACK_SIZE 24
#define BLOCK_CHARACTER 219

typedef unsigned char u_char;

typedef struct
{
	union
	{
		u_char v[16];
		struct
		{
			u_char v0;
			u_char v1;
			u_char v2;
			u_char v3;
			u_char v4;
			u_char v5;
			u_char v6;
			u_char v7;
			u_char v8;
			u_char v9;
			u_char va;
			u_char vb;
			u_char vc;
			u_char vd;
			u_char ve;

			///also flag register
			u_char vf;
		};
	};

	///adress register
	uint16_t i;

	///program counter
	uint16_t pc;

	///stack pointer
	u_char sp;

	///delay timer
	u_char dt;

	///sound timer
	u_char st;

	u_char shouldRedraw;

	unsigned int cpuCount;

}regs_t;


void execute(const char * p, long size);
void displayRegs(regs_t *r, uint16_t *stack, long stackSize);
void yieldError(regs_t *r, uint16_t *stack, const char *error);
int isButtonPressed(int button);
void redrawScreen(char * c, HANDLE h);
void setFontSize(int a, int b);
char *getPixel(int x, int y, char *buffer);
void win32execute(const char * p, long size);
