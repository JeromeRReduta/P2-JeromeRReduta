#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"
#include "signal.h"
#include "job_list.h"
#include "process_io.h"
#include "pipe.h"

/**
 * @file Driver function
 */

/* Function prototypes */
void skip_comment(char **current_ptr, char *next);
void replaceIfBang(char *command);

bool is_background_job = false;
char* background_command = NULL;


/**
 * @brief      Driver func
 *
 * @return     main() return value
 */
int main(void)
{
    init_ui();
    signal_init_handlers();
    hist_init(100);
    job_list_init();

/*
    test_p_IO_output();
    test_p_IO_reset();
    test_p_IO_append();
    test_p_IO_input();
*/


// take command and add to history
// have to worry about !command_name
    // If !aiweghaihge doesn't return anything (returns NULL), don't add to memory
// strdup() p impt for adding command
// BAD IDEA: hard-coding history_entry size (but good when starting)
// Also, have to free all cases of readline()


    

    
    while (true) {
        char *command;
        char *command_copy; // FREE THIS LTER
        command = read_command();


        replaceIfBang(command);

       

        if (command == NULL) {
            break;
        }

        LOG("Input command: %s\n", command);


        // TODO: COMMENT BELOW ONE OUT WHEN DONE TESTING
        // PIPE TESTING - REPLACING LINE W/ "ls -l"
        //==command = "ls -a | sort -r | sort";
     
        hist_add(command);

        command_copy = strdup(command);


        LOG("COMMAND:\t'%s', COMMAND COPY:\t'%s'\n", command, command_copy);

        int tokens = 0;
        char *next = command_copy;
        char *current;



        // 4096 b/c according to Prof. Malensek, POSIX standard requires
        // shells to accept 4096 args (1st being name)
        // Now compiles w/ posix standards!
        char *args[_POSIX_ARG_MAX] = {(char*) 0};



        current = next_token(&next, " \t\r\n");


        while (current != NULL) {

            //LOG("Token %02d: '%s'\n", tokens, current);

            // Case: comments
            if (current[0] == '#') {
                skip_comment(&current, next);
    
            }
            else {
                args[tokens++] = current;
                current = next_token(&next, " \t\r\n");
           
            }   
           
        }


        //PIPE_TESTING
        // test_pipe_one_arg(args); // CONFIRMED SUCCESS

        // test_pipe_two_args(args); // CONFIRMED SUCCESS ON 2 AND EVEN 3 ARGS




        if (strcmp(args[tokens - 1], "&") == 0) {
            is_background_job = true;


            

            args[tokens - 1] = (char *) NULL;

            background_command = strdup(command);


            LOG("REMOVING &:\t'%s'\n", args[tokens-1]);
        }

/*
        LOGP("CURRENT ARGS:\t\n");
        for (int i = 0; i < tokens; i++) {
            LOG("\t->:'%s'\n", args[i]);
        }
        LOGP("\n");
*/

        // Case: no args (just pressing enter)
        if (tokens == 0) {
            LOGP("NO TOKENS:\n");
            continue;

        }

        p_IO_process_IO_chars(args);
        
/*
        if (strstr(command, "|") != NULL) {
            LOGP("FOUND PIPE:\n");
            do_pipe(args);
        }
        */
        // TODO: copied and pasted from lecture - refactor later
        // Case: exit
        if (strcmp(args[0], "exit") == 0) {
            return EXIT_SUCCESS;
        }
        // Case: cd
        else if (strcmp(args[0], "cd") == 0) {
            cd_with(args);
        }
        else if (strcmp(args[0], "history") == 0) {
            LOG("HISTORY INPUTTED:\t'%s'\n", command);
            hist_print();
        }
        else if (strcmp(args[0], "jobs") == 0) {
            LOG("JOBS LIST INPUTTED:\t'%s'\n", command);
            job_list_print();
        } // Note - can't run pipe on single-arg thing for now - there is bug
        // Case: anything else
       else {
            do_pipe(args);
        }

        is_background_job = false;
        // background_command freed in job_list_destroy   
        free(command_copy);
        p_IO_reset();
    
    
    }

    job_list_destroy();

    return 0;
}

/**
 * @brief      Skips comments in arg parsing
 *
 * @param      current_ptr  current ptr
 * @param      next         ptr to thing after current_ptr
 */
void skip_comment(char **current_ptr, char *next) {
    LOGP("current_ptr HAS #");
    next_token(&next, "\r\n");
    *current_ptr = next_token(&next, "\r\n");
    LOG("current_ptr NOW:\t'%s'\n", *current_ptr);
}

/**
 * @brief      Replaces commands that start w/ ! with outputs of their history commands
 *
 * @param      command  commandline arguments
 */
void replaceIfBang(char *command)
{


    if (command == NULL) {
        return;
    }
    else if (command[0] == '!' && isdigit(command[1])) {
        int command_num = atoi(command + 1);
        const char* replacement = hist_search_cnum(command_num);


        LOG("DOING HIST NUM:\n"
            "\t->command:\t'%s'\n"
            "\t->command_num:\t%d\n"
            "\t->replacement:\t'%s'\n",
            command, command_num, replacement);

        if (replacement != NULL) {
            strcpy(command, replacement);
        }
    }
    else if (command[0] == '!' && isalpha(command[1])) {
        char* prefix = command + 1;
        char* newline_loc = strstr(prefix, "\n");

        if (newline_loc != NULL) {
            *newline_loc = '\0';
        }

        const char* replacement = hist_search_prefix(prefix);


        LOG("DOING HIST PREFIX:\n"
            "\t->command:\t'%s'\n"
            "\t->prefix:\t'%s'\n"
            "\t->replacement:\t'%s'\n",
            command, prefix, replacement);

        if (replacement != NULL) {
            strcpy(command, replacement);
        }


    }

    else if (strncmp(command, "!!", 2) == 0) {

        const char* replacement = hist_search_cnum(hist_last_cnum());

        LOG("DOING !!:\n"
            "\t->replacement:\t'%s'\n",
            replacement);

        if (replacement != NULL) {
            strcpy(command, replacement);
        }
    }

}