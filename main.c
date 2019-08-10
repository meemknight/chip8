#include <stdio.h>
#include <stdlib.h>

#include "loadProgram.h"
#include "executeEmulation.h"

void main()
{

	char * c =loadProgram("rom.txt");

	execute(c);

	getchar();


}