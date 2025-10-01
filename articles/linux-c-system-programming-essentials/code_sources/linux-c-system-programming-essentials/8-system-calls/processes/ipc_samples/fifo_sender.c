#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

char* fifo_filename = "/tmp/fifo_example22";
int fifo_fd = -1;

void dispose();
void on_signal(int signal);

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		fprintf(stderr, "2nd argument required.\n");
		exit(1);
	}

	struct sigaction action = { 0 };

	action.sa_handler = on_signal;
	sigfillset(&action.sa_mask);
	action.sa_flags = SA_RESTART;
	
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	sigaction(SIGABRT, &action, NULL);
	sigaction(SIGPIPE, &action, NULL);

		
	int mkfifo_result = mkfifo(fifo_filename, 0644);

	if( mkfifo_result != 0 ) 
	{
		perror("mkfifo() failed.");
		exit(1);
	}

	fifo_fd = open(fifo_filename, O_WRONLY);
	
	if( fifo_fd == -1 )
	{
		perror("open fifo failed.");
		exit(1);
	}	

	while(1)
	{
		
		dprintf(fifo_fd, "%s\n", argv[1]);
		fprintf(stdout, "message send.\n");
		sleep(1);
	}
	
	dispose();
	
}

void on_signal(int signum)
{
	/* supress compiler warning */
	(void)signum;
	dispose();
}

void dispose()
{
	/* close file descriptor */
	int close_res = close(fifo_fd);

	if(close_res == -1)
	{
		perror("dispose() file descriptor closing error.");
	}

	/* delete file */
	int unlink_res = unlink(fifo_filename);

	if(unlink_res == -1)
	{
		perror("dispose() file deleting error.");
	}

	exit(0);
}
