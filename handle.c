#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "util.h"

//handler for SIGINT
void sigint_handler(int sig)
{
	const int STDOUT = 1;
	write(STDOUT, "Nice try.\n", 10);
}

//handler for SIGUSR1
void sigusr1_handler(int sig)
{
	const int STDOUT = 1;
	write(STDOUT, "exiting\n", 9);
	exit(0);
}


/*
 * First, print out the process ID of this process.
 *
 * Then, set up the signal handler so that ^C causes
 * the program to print "Nice try.\n" and continue looping.
 *
 * Finally, loop forever, printing "Still here\n" once every
 * second.
 */
int main(int argc, char **argv)
{
  struct timespec t;
  long long int interval = 100000L;
  t.tv_sec = 0;
  t.tv_nsec = interval;
  long long int timer = 0;
  const int STDOUT = 1;
  pid_t pid = getpid();
  char buffer[17] = {0};
  sprintf(buffer, "%lld\n", pid);
  write(STDOUT, buffer, sizeof(buffer)) != sizeof(buffer);
  signal(SIGINT, sigint_handler);
  signal(SIGUSR1, sigusr1_handler);

  while(1)
  {
    /*sleep for every 0.0001 second*/
    nanosleep(&t, NULL);
    timer += interval;
    /*for every sceond, print Still here, and reset timer to 0*/
    if(timer >= 1000000000L)
    {
      write(STDOUT, "Still here\n", 11);
      timer = 0;
    }
  }

  return 0;
}


