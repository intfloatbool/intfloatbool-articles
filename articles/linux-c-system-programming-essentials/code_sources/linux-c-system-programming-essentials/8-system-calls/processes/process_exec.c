#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

	if(argc != 2)
	{
		printf("filename argument required.\n");
		exit(1);
	}

	const char* bin_to_execute = "/usr/bin/touch";

	printf("execute bin: %s ...\n", bin_to_execute);
	
	int exec_result = execl(bin_to_execute, "touch", argv[1], (char*) NULL);

	if( exec_result == -1 )
	{
		perror("executing program failed");
		exit(1);
	}
	printf("DONE!\n");
	exit(0);
}
