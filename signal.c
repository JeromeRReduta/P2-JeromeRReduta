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


/* Function prototypes: */
void sigint_handler(int signo);



// Initializes the signals
void signal_init_handlers()
{
    signal(SIGINT, sigint_handler);
    LOGP("ADDED SIGINT HANDLER\n");
    
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
