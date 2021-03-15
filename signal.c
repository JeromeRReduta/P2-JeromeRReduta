/**
 * signal.c
 *
 * Demonstrates signal handling. During normal operation this program counts up
 * each second, starting from 1. Upon receiving SIGINT, the final count will be
 * printed and the program will exit. Many programs use this type of signal
 * handling to clean up and gracefully exit when the user presses Ctrl+C.
 *
 * Compile: gcc -g -Wall signal.c -o signal
 * Run: ./signal
 */

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "logger.h"
#include "job_list.h"






/* Function prototypes: */
void sigint_handler(int signo);
void sigchld_handler(int signo);


// Initializes the signals
void signal_init_handlers()
{
    signal(SIGINT, sigint_handler);
    LOGP("ADDED SIGINT HANDLER\n");
    signal(SIGCHLD, sigchld_handler);
    LOGP("ADDED SIGCHLD HANDLER\n");

    LOGP("ATTEMPTING TO SEND SIGINT SIGNAL AGAIN");
    signal(SIGINT, sigint_handler);
    
}

// Handles inputted signals
void sigint_handler(int signo)
{

    if (signo == SIGINT) {

        
        LOGP("CTRL C PRESSED\n");

        /* Note: ui.c already prints prompt by default if task is not currently running, while
         * cancelling running process. I have no idea why */

        fflush(stdout);
    }
}

// Handles child exit signals
void sigchld_handler(int signo)
{
	if (signo == SIGCHLD) {
	LOGP("CHILD EXITED PROCESS\n");

		LOGP("WAS ALSO BACKGROUND JOB\n");
		LOGP("CLEARING FIRST AVAIL CHILD\n");

		job_list_pop();
	}

}


