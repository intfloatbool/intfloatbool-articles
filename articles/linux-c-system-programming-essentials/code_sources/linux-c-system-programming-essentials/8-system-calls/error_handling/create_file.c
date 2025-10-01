#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "filename agrument required!\n");
		exit(1);
	}

	
	int open_code = open(argv[1], O_CREAT | O_EXCL | O_WRONLY, 0644);

	if(open_code == -1)
	{
		fprintf(stderr, "file creation failed.\n");
		
		if(errno == EACCES)
		{
			fprintf(stderr, "permission denied!\n");
		}
		else if(errno == EEXIST)
		{
			fprintf(stderr, "file already exists!\n");
		}
		else 
		{
			fprintf(stderr, "unknown errno: %d\n", errno);
		}
		
		fprintf( stderr, "strerror() result: %s\n", strerror(errno) );
		
		exit(1);
	}

	exit(0);
}
