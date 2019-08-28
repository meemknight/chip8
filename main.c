#include <stdio.h>
#include <stdlib.h>

#include "loadProgram.h"
#include "executeEmulation.h"


void main(int argc, char *argv[])
{
	unsigned char * c;
	long size = 0;

	if(argc > 1)
	{
		c = loadProgram(argv[1], &size);

	}else
	{
		char in[250];

		scanf("%s", in);
		c = loadProgram(in, &size);

	}

	win32execute(c, size);

	//free(c);

	getchar();


}
