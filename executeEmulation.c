#include "executeEmulation.h"
#include <stdint.h>
#include <string.h>

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

	///program counter
	uint16_t I;
	
	///stack pointer
	u_char sp;

	///delay timer
	u_char dt;

	///sound timer
	u_char st;

}regs_t;


void execute(const char * p) 
{
	//resetting the state
	regs_t regs;
	memset(&regs, 0, sizeof(regs));

	uint16_t stack[STACK_SIZE];
	memset(stack, 0, sizeof(stack));


	while(1)
	{
	
	
	
	}

}
