#include <stdio.h>
#include <stdlib.h>

#include "loadProgram.h"
#include "executeEmulation.h"


void main()
{

	long size = 0;
	unsigned char * c =loadProgram("roms/HIDDEN", &size);

	execute(c, size);

	free(c);

	getchar();


}