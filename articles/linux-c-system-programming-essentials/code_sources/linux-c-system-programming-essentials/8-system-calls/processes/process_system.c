#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int system_result = system("touch zika.txt");

	if( system_result == -1)
	{
		perror("some error in command.");
		exit(1);
	}
	
	exit(0);
}
