#include <stdio.h>
#include <stdlib.h>

#include "loadProgram.h"
#include "executeEmulation.h"


#define WIN32MODE

void main(int argc, char *argv[])
{
	unsigned char * c = 0;
	long size = 0;

	if(argc > 1)
	{
		c = loadProgram(argv[1], &size);

	}else
	{
		puts("Type ROM name:");

		while (c == 0) 
		{
			char in[250];

			scanf("%s", in);
			c = loadProgram(in, &size);

		}

		system("cls");
	}

#ifdef WIN32MODE
	win32execute(c, size);
#else
	execute(c, size);
#endif // WIN32MODE

	//free(c);

	getchar();


}
