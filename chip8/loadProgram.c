#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loadProgram.h"

unsigned char* loadProgram(const char *name, long *s)
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
	unsigned char* data = malloc((size + 2) * sizeof(char));
	fread(data, sizeof(char), size, f);
	data[size - 1] = '\0';
	data[size - 2] = '\0';
	//last 2 bytes are 0

	fclose(f);

	for(int i=0; i< size; i+=2)
	{
		int test = data[i];
		int test2 = data[i+1];
		int a = (data[i] & 0xF0) >> 4;
		int b = data[i] & 0x0F;
		int c = (data[i + 1] & 0xF0) >> 4;
		int d = data[i + 1] & 0x0F;
		//printf("%X %X %X %X\n", a, b, c, d);

	}
	//puts("\n");

	*s = size;	
	return data;
}