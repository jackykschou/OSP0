#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>


int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("Usage: mykill <pid to kill>\n");
	}
	else if(kill((pid_t) strtol(argv[1], (char **)NULL, 10), SIGUSR1) == -1)
	{
		printf("Failed to send signal to process %lld\n", (pid_t) strtol(argv[1], (char **)NULL, 10));
	}

  return 0;
}
