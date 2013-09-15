/* 
 * msh - A mini shell program with job control
 * 
 * <Put your name and login ID here>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "util.h"
#include "jobs.h"
 
/* Global variables */
int verbose = 0;            /* if true, print additional output */

extern char **environ;      /* defined in libc */
static char prompt[] = "msh> ";    /* command line prompt (DO NOT CHANGE) */
static struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
void usage(void);
void sigquit_handler(int sig);



/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
	default:
            usage();
	}
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{
    if(strcmp(cmdline, "\n") != 0) //ignore empty input
    {
        const int STDOUT = 1;
        int is_bg;
        char command[MAXLINE][MAXARGS];
        char **command_ptr = command;

        is_bg = parseline(cmdline, command);

        /*execute the program if it is not a build-in command*/
        if(!builtin_cmd(command))
        {
            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGCHLD);
            sigprocmask(SIG_BLOCK, &mask, NULL);
            pid_t child = fork(); //start new process for the new job
            if(child == 0)
            {
                //set the pid group
                setpgid(0, 0);

                //execute the job
                if(execvp(command_ptr[0], command_ptr) < 0) 
                {
                    sigprocmask(SIG_UNBLOCK, &mask, NULL); //unblock signal so that the job can be deleted now
                    strcat (command_ptr[0],": command not found\n");
                    write(STDOUT, command_ptr[0], 20 + sizeof(command_ptr[0]));
                    exit(1);
                }
            }
            else
            {
                //the parent add the job to the job list
                addjob(jobs, child, (is_bg ? BG : FG), cmdline);
                struct job_t *j = getjobpid(jobs, child);
                sigprocmask(SIG_UNBLOCK, &mask, NULL); //unblock signal so that the job can be deleted now
                //print background job information
                if(is_bg)
                {
                    printf("[%d] (%lld) %s", j->jid, j->pid, j->cmdline);
                }
                if(!is_bg)
                {
                    waitfg(child);
                }
            }
        }
    }
    return;
}


/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 * Return 1 if a builtin command was executed; return 0
 * if the argument passed in is *not* a builtin command.
 */
int builtin_cmd(char **argv) 
{
    /*quit*/
    if(strcmp(argv[0], "quit") == 0)
    {
        exit(0);
    }
    /*jobs*/
    else if(strcmp(argv[0], "jobs") == 0)
    {
        listjobs(jobs);
        return 1;
    }
    /*fg or bg*/
    else if(strcmp(argv[0], "bg") == 0 || strcmp(argv[0], "fg") == 0)
    {
        do_bgfg(argv);
        return 1;
    }

    return 0;     /* not a builtin command */
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    /*get the jid or pid*/
    struct job_t *j;
    long long int id;
    if(argv[1][0] == '%')
    {
        //remove the %
        size_t len = strlen(argv[1]) - 1;
        char *num;
        num = strncpy(num, argv[1][1], strlen(argv[1]) - 1);
        id = strtol(num, (char **)NULL, 10);
        printf("hehe: %d\n", id);
    }
    else
    {
        id = strtol(argv[1], (char **)NULL, 10);
    }
    //conversion fail, invalid input
    if(id == 0)
    {
        printf("msh: %s: %s: no such job\n", argv[0], argv[1]);
        return;
    }
    j = getjobpid(jobs, (pid_t) id);
    //if fails to get pid, tries to get jid
    if(j == NULL)
    {
        j = getjobjid(jobs, (int) strtol(argv[1], (char **)NULL, 10));
    }
    //if fails to get jid too, it must be an invalid input
    if(j == NULL)
    {
        printf("msh: %s: %s: no such job\n", argv[0], argv[1]);
        return;
    }
    /*check if bg or fg*/
    if(strcmp(argv[0], "bg") == 0)
    {
        //if bg, just sent the continue signal
        kill(j->pid, SIGCONT);
    }
    else
    {
        //if fg, change the state of the job, send continue signal and wait for it to finish
        j->state = FG;
        kill(j->pid, SIGCONT);
        waitfg(j->pid);
    }
    return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 100000L;
    while(1)
    {
        /*sleep for every 0.0001 second*/
        nanosleep(&t, NULL);
        struct job_t *j = getjobpid(jobs, pid);
        /*for 0.0001 second, check for is the foreground process has stopped or terminated*/
        if(j == NULL || j->state != FG)
        {
            return;
        }
    }
    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
    int *status;
    //get the pid of possible stopped or terminated child or skip immediately if there is no such one
    pid_t pid = waitpid(-1, &status, WNOHANG|WUNTRACED);
    //error for waitpid
    if(pid < 0)
    {
        unix_error("waitfg: waitpid error");
    }
    //if a child has stopped or terminated
    else if(pid != 0)
    {
        struct job_t *j = getjobpid(jobs, pid);
        //job exit normally
        if(WIFEXITED(status))
        {
            deletejob(jobs, pid);
        }
        //exited because a signal was not caught
        else if(WIFSIGNALED(status))
        {
            printf("Job [%d] (%lld) terminated by signal %d\n", j->jid, j->pid, WTERMSIG(status));
            deletejob(jobs, pid);
        }
        //stopped by signal
        else if(WIFSTOPPED(status))
        {
            printf("Job [%d] (%lld) stopped by signal %d\n", j->jid, j->pid, WSTOPSIG(status));
            j->state = ST;
        }
    }
    return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
    pid_t pid;
    if ((pid = fgpid(jobs)) != 0)
    {
        kill(-pid, SIGINT);
    }
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    pid_t pid;
    if (pid = fgpid(jobs) != 0)
    {
        kill(-pid, SIGTSTP);
    }
    return;
}

/*********************
 * End signal handlers
 *********************/



/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}



