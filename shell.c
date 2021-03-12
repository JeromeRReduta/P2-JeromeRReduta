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

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"
#include "signal.h"

/* Function prototypes */
void skip_comment(char **current_ptr, char *next);

int main(void)
{
    init_ui();


    signal_init_handlers();
    char *command;
    while (true) {
        command = read_command();
        if (command == NULL) {
            break;
        }

        LOG("Input command: %s\n", command);

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

            char* path = args[1] != NULL ? args[1] : home_dir;
            LOG("COMMAND HAS CD:\t'%s'\n", args[0]);
            LOG("PATH:\t'%s'\n", path);
            chdir(path);

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

