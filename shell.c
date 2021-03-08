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

        int tokens = 0;
        char *next = command;
        char *current;

        char *args[10] = {(char*) 0};

        while ((current = next_token(&next, " \t\r\n")) != NULL) {
            LOG("Token %02d: '%s'\n", tokens, current);

            args[tokens++] = current;

        }
        
 


        // TODO: copied and pasted from lecture - refactor later
        // Bulitins?
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
