#include <stdlib.h>
#include <stdio.h>

#define LINE_BUFFER_SIZE 1024

int main(int argc, char* argv[]) 
{
	FILE* file = NULL;

	char line_buffer[LINE_BUFFER_SIZE] = { 0 };

	if ( argc != 2 )
	{
		fprintf(stderr, "filepath required!\n");
		exit(1);
	}

	file = fopen(argv[1], "w");
	if( file == NULL )
	{
		perror("file opening error!");
		exit(1);
	}

	while( fgets( line_buffer, sizeof(line_buffer), stdin ) != NULL )
	{
		fprintf( file, line_buffer );
	}

	fclose( file );
	
	exit(0);

}
