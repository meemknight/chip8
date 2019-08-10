#include <stdio.h>
#include <stdlib.h>

#include "loadProgram.h"

void main()
{

	char * c =loadProgram("rom.txt");

	printf(c);

	getchar();

	free(c);

}