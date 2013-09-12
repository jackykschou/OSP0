#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

const int MAX = 13;

static void doFib(int n, int doPrint);


/*
 * unix_error - unix-style error routine.
 */
inline static void 
unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}


int main(int argc, char **argv)
{
  int arg;
  int print;

  if(argc != 2){
    fprintf(stderr, "Usage: fib <num>\n");
    exit(-1);
  }

  if(argc >= 3){
    print = 1;
  }

  arg = atoi(argv[1]);
  if(arg < 0 || arg > MAX){
    fprintf(stderr, "number must be between 0 and %d\n", MAX);
    exit(-1);
  }

  doFib(arg, 1);

  return 0;
}

/* 
 * Recursively compute the specified number. If print is
 * true, print it. Otherwise, provide it to my parent process.
 *
 * NOTE: The solution must be recursive and it must fork
 * a new child for each call. Each process should call
 * doFib() exactly once.
 */
static void 
doFib(int n, int doPrint)
{

  //special case: no Fork is called because user just input 0 or 1
  if(doPrint)
  {
    if(n == 0 || n == 1)
    {
      fprintf(stdout, "%d\n", n);
      return;
    }
  }
  if(n == 0)
    exit(0);
  else if(n == 1)
    exit(1);
  else
  {
    pid_t child = fork();
    if(child == 0)
      doFib(n - 1, 0);
    else
    {
      child = fork();
      if(child == 0)
        doFib(n - 2, 0);
      else
      {
        int status;
        int sum = 0;
        pid_t pid = 0;

        while(1)
        {
          pid = wait(&status);
          if(pid == -1)
          {
            if(!doPrint)
              exit(sum);
            else
            {
              fprintf(stdout, "%d\n", sum);
              break;
            }
          }
          else
            sum += WEXITSTATUS(status);
        } 
      }
    }
  }
}


