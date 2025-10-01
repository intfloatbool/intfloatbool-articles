#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

int main(void)
{	
	printf("root PID: %d\n", getpid());

	pid_t forked_pid = fork();

	if( forked_pid == -1 )
	{
		perror("fork error.");
		exit(1);
	}

	
	bool is_child = forked_pid == 0;
	bool is_parent = forked_pid > 0;	
	
	unsigned int sleep_seconds = 10;

	if( is_child )
	{
		printf("\t\tchild process started!\n");
		sleep(sleep_seconds);
		printf("\t\tchild process done!\n");
	}

	if( is_parent )
	{
		int return_child_status = -1;
		printf("parent process started!\n");
		printf("\tchild PID: %d\n", forked_pid);
		printf("\twaiting for child ends...\n");

		waitpid(forked_pid, &return_child_status, 0);

		printf("\tchild done work! return status of child: %d\n", return_child_status);
		
		printf("parent process done.\n");

		sleep(2);
	}

	exit(0);

}

