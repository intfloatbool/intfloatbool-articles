#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE* fifo_file = fopen("/tmp/fifo_example22", "r");
	if( fifo_file == NULL )
	{
		perror("file opening error.");
		exit(1);
	}

	unsigned char c = { 0 };
	while( ( c = getc(fifo_file) ) != EOF )
	{
		putchar(c);
	}

	fclose(fifo_file);

	exit(1);
}
