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

/* Function prototypes */
void skip_comment(char **current_ptr, char *next);
void cd_with(char **args);
void replaceIfBang(char *command);

int main(void)
{
    init_ui();


// take command and add to history
// have to worry about !command_name
    // If !aiweghaihge doesn't return anything (returns NULL), don't add to memory
// strdup() p impt for adding command
// BAD IDEA: hard-coding history_entry size (but good when starting)
// Also, have to free all cases of readline()

    signal_init_handlers();
    hist_init(100);
    char *command;

    char *command_copy; // FREE THIS LTER
    while (true) {
        command = read_command();


        replaceIfBang(command);

       

        if (command == NULL) {
            break;
        }

        LOG("Input command: %s\n", command);


        hist_add(command);

        int tokens = 0;
        char *next = command;
        char *current;

        // 4096 b/c according to Prof. Malensek, POSIX standard requires
        // shells to accept 4096 args (1st being name)
        // Now compiles w/ posix standards!
        char *args[_POSIX_ARG_MAX] = {(char*) 0};

        current = next_token(&next, " \t\r\n");


        while (current != NULL) {

            LOG("Token %02d: '%s'\n", tokens, current);

            // Case: comments
            if (current[0] == '#') {
                skip_comment(&current, next);
    
            }
            else {
                args[tokens++] = current;
                current = next_token(&next, " \t\r\n");
           
            }   
           
        }

        LOGP("CURRENT ARGS:\t\n");
        for (int i = 0; i < tokens; i++) {
            LOG("\t->:'%s'\n", args[i]);
        }
        LOGP("\n");


        // Case: no args (just pressing enter)
        if (tokens == 0) {
            LOGP("NO TOKENS:\n");

        }
        // TODO: copied and pasted from lecture - refactor later
        // Case: exit
        else if (strcmp(command, "exit") == 0) {
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
        // Case: anything else
       else {


            LOGP("DOING CHILD FORK THING:\n");


            pid_t child = fork();

            // If cannot make any more children, just keep going until
            // we can make another fork
            if (child == -1) {
                perror("fork");
                continue;
            }
            // Case: child
            // Note: possible for child to reset stdin stream, so have to close child
            // Linux only
            else if (child == 0) {
                if (execvp(args[0], args) == -1) {
                    perror("execvp");
                }

                // Close these 3 so that child doesn't reset data
                // Last 2 not too necessary but 1st one is
                close(fileno(stdin));
                close(fileno(stdout));
                close(fileno(stderr));
                
                // Putting this out of error case - see if it works
                // If I don't put this return outside of the error case, I think we end up with an infinite loop?
                return EXIT_FAILURE;
            }
            // Case: parent
            else {
                //puts("Bubba Bradley is waiting for his daughter\n");

                int status;
                waitpid(child, &status, 0);

            }

        }
    
    
    }

    return 0;
}

void skip_comment(char **current_ptr, char *next) {
    LOGP("current_ptr HAS #");
    next_token(&next, "\r\n");
    *current_ptr = next_token(&next, "\r\n");
    LOG("current_ptr NOW:\t'%s'\n", *current_ptr);

}

void cd_with(char **args)
{
    char* path = args[1] != NULL ? args[1] : home_dir;
    
    LOG("COMMAND HAS CD:\t'%s'\n", args[0]);
    LOG("PATH:\t'%s'\n", path);
    if (chdir(path) == -1) {
        perror("chdir");
    }

}

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

        char* replacement = hist_search_cnum(hist_last_cnum());

        LOG("DOING !!:\n"
            "\t->replacement:\t'%s'\n",
            replacement);

        if (replacement != NULL) {
            strcpy(command, replacement);
        }
    }

}