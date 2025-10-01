#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>

/* volatile to avoid compiler optimization in loops */
/* sig_atomic_t - atomic int for signals */

static volatile sig_atomic_t g_stop = 0;

static void on_signal(int sig) 
{
	/* to supress unsused warning */
	(void)sig; 
	g_stop = 1;
}

static int sleep_monotonic(double seconds)
{
	/* requested time */
	struct timespec req;

	/* remaining time */
	struct timespec rem;

	req.tv_sec = (time_t) seconds;

	/* to save decimal part if seconds = X.YZ */
	req.tv_nsec = (long) ((seconds - req.tv_sec) * 1e9);

	while (nanosleep(&req, &rem) == -1 && errno == EINTR)
	{
		/* sleep remaining if needed */
		req = rem;
	}

	return 0;
}

/* systemd takes 'stderr' and translates it into journal */
static void log_line(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	fflush(stderr);
	va_end(ap);
}

int main(int argc, char** argv)
{
	double interval = 5.0;

	if(argc > 1)
	{
		interval = atof(argv[1]);
	}


	if(interval <= 0.0) 
	{
		interval = 5.0;
	}	

	/* signal handlers */
	struct sigaction sa = {0};
	sa.sa_handler = on_signal;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	log_line("[my_daemon] XXX start pid=%d, interval=%.3f sec", getpid(), interval);
	
	unsigned long tick = 1;

	while(!g_stop) 
	{
		log_line("[my_daemon] XXX tick=%lu", tick);
		tick++;

		sleep_monotonic(interval);
	}

	log_line("[my_daemon] XXX stop (received signal)", getpid());

	exit(0);
}
