#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loadProgram.h"

char* loadProgram(const char *name)
{

	//error checking
	FILE *f = fopen(name, "rb");
	if(f == 0)
	{
		printf("error loadint the %s file\n", name);
		return 0;
	}

	fseek(f, 0, SEEK_END);
	long size = 0;
	size = ftell(f);

	//reading and null terminating the string
	rewind(f);
	char* data = malloc((size + 2) * sizeof(char));
	fread(data, sizeof(char), size, f);
	data[size - 1] = '\0';
	data[size - 2] = '\0';
	//last 2 bytes are 0

	fclose(f);

	return data;
}