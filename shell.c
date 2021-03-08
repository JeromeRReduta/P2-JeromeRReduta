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
#include "util.h"
#include "ui.h"

int main(void)
{
    init_ui();

    char *command;
    while (true) {
        command = read_command();
        if (command == NULL) {
            break;
        }

        LOG("Input command: %s\n", command);

        char *args[10] = {(char*) 0};

        int tokens = 0;
    

        char *next_tok = command;
        char *curr_tok;
        /* Tokenizes. Note that the below delim will be ignored */
        while ((curr_tok = next_token(&next_tok, " \t\r\n")) != NULL) {
            args[tokens++] = curr_tok;
            LOG("Token %02d: '%s'\n", tokens++, curr_tok);
        }

        
     
        // TODO: copied and pasted from lecture - refactor later
        // Bulitins?
        // Free memory before exiting!!!!!
        if (strcmp(command, "exit") == 0) {
            return EXIT_SUCCESS;
        }


        pid_t child = fork();

        // If cannot make any more children, just keep going until
        // we can make another fork
        if (child == -1) {
            perror("fork");
            continue;
        }
        // Case: child
        else if (child == 0) {
            if (execvp(args[0], args) == -1) {
                perror("execvp");
            }
        }
        // Case: parent
        else {
            puts("Bubba Bradley is waiting for his daughter\n");

            int status;
            waitpid(child, &status, 0);

        }
    
    
    }

    return 0;
}
