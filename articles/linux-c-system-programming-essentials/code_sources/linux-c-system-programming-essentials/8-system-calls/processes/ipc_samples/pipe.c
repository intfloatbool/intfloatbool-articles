#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#define MESSAGE_MAX_BYTES 256

void check_pipe_result(int res)
{
	if( res == -1 ) 
	{
		perror("pipe() failed");
		exit(1);
	}	
}

int main(void)
{
	/* pipe is undirectional data-channel */
	/* it receives 2 files descriptors
	 * [0] - the read end of the pipe
	 * [1] - the write end of the pipe
	 */
	int pipe_parent_to_child[2] = { 0 };
	int pipe_child_to_parent[2] = { 0 };	

	char parent_message[MESSAGE_MAX_BYTES];
	char child_message[MESSAGE_MAX_BYTES];

	check_pipe_result( pipe( pipe_parent_to_child )  );
	check_pipe_result( pipe( pipe_child_to_parent )  );
	

	pid_t pid = fork();

	if( pid == -1 )
	{
		perror("fork() failed");
		exit(1);
	}

	if( pid > 0 )
	{
		/* parent process */
	
		/* send message to child */
		FILE* to_child = fdopen(pipe_parent_to_child[1], "w");
		fprintf(to_child, "<message from PARENT>\n");
		fflush(to_child);

		/* read message from child */
		FILE* from_child = fdopen(pipe_child_to_parent[0], "r");
		fgets(child_message, MESSAGE_MAX_BYTES, from_child);
		printf("message to parent -> %s\n", child_message);
		fflush(from_child);
	}
	else
       	{
		/* child process */
		
		/* send message to parent */
		FILE* to_parent = fdopen(pipe_child_to_parent[1], "w"); 
		fprintf(to_parent, "<message from CHILD>\n");
		fflush(to_parent);

		/* read message from parent */	
		FILE* from_parent = fdopen(pipe_parent_to_child[0], "r");
		fgets(parent_message, MESSAGE_MAX_BYTES, from_parent);
		printf("message to child -> %s\n", parent_message);
		fflush(from_parent);
	}

	exit(0);

}
