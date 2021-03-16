#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "logger.h"
#include "string.h"
#include "job_list.h"
#include "pipe.h"


struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdin_file;
    char *stdout_file;

    bool append;
};

// Static int - turn to -1 if ANY child process fails
static int child_success = 0;
static int cmds_size = 0;
static int cmds_max_len = 2;
/* Func prototypes */
void log_static_vars();

// TODO: Set execute_final_command, execute_and_pipe_command, and execute_pipeline to return ints when done - -1 on failure,
// Don't rly have to worry about success (execvp will handle that)

void set_command_line(struct command_line *cmd, char **tokens, bool stdout_pipe, char *stdout_file);
int execute_final_command(struct command_line *cmds);
int execute_and_pipe_command(struct command_line *cmds);
void log_struct(struct command_line *cmds, int args_counter);
void destroy_command_line(struct command_line *cmds);

void do_pipe(char **args);

int execute_pipeline(struct command_line *cmds)
{
    if (!cmds->stdout_pipe) {

        if (execute_final_command(cmds) == -1) {
            return -1;
        }
    }
    else {
        if (execute_and_pipe_command(cmds) == -1) {
            printf("BAD EXECUTE_AND_PIPE - returning -1");
            child_success = -1;
            return -1;
        }
    }

    printf("Shouldn't reach here - execute_pipeline()\n");
    child_success = -1;
    return -1;

  // On success, code won't reach this point - will be replaced 
    // w/ base-case execvp
}

int execute_and_pipe_command(struct command_line *cmds)
{


    // Create pipe w/ 2 fds
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        child_success = -1;
        return -1; // TODO: Prob return -1
    }

    // Make child
    pid_t child = fork();

    // Case: fork() error - end pipe early (return -1 to loop)
    if (child == -1) {
        perror("fork");
        child_success = -1;
        return -1;
    }
    // Case: child - execvp and set fd[1] to stdout
    else if (child == 0) {
        if ( dup2(fd[1], fileno(stdout)) == -1) {
            perror("dup2");
            child_success = -1;
            return -1; // TODO: Prob return -1
        } 
        close(fd[0]); // Should close these - I think not closing causes infinite loop?

        if (execvp(cmds->tokens[0], cmds->tokens) == -1) {
            perror("execvp");
            LOGP("FAILED HERE\n");
            child_success = -1;
            return -1; // TODO: make int for shell later? So if this fails you can display bad emoji
        }


    }
    else {


        if ( dup2(fd[0], fileno(stdin)) == -1) {
            perror("dup2");
            child_success = -1;
            return -1; // TODO: Prob return -1
        }
        close(fd[1]);

        if (child_success == -1) {
            printf("LAST THING BEFORE PIPELINE: child_retval is -1 - returning -1\n");
            return -1;
        }
        // No waiting b/c we can just everything run in parallel
        // Faster, and means we don't have to wait for full data stream from one command before we run the next command
        if (execute_pipeline(cmds + 1) == -1) {
            child_success = -1;
            return -1;
        }

    }

    printf("Shouldn't reach here - pipe_command()\n");
    child_success = -1;
    return -1;
}

int execute_final_command(struct command_line *cmds)
{
    if (cmds->stdout_file != NULL) {

        // Creates fd for output file and sets it to
        // stdout - now outputs go to this file

        int open_flags = O_RDWR | O_CREAT | O_TRUNC;
        int open_perms = 0666;
        int output_file_fd = open(cmds->stdout_file, open_flags, open_perms);
        // Note: Don't have to close output_file_fd - it's child process's prob now :D
        if (output_file_fd == -1) {
            perror("open");
            child_success = -1;
            return -1; // TODO: Prob do return -1
        }

        if (dup2(output_file_fd, fileno(stdout)) == -1) {
            perror("dup2");
            child_success = -1;
            return -1; // TODO: Prob return -1
        }
    }

    if ( execvp(cmds->tokens[0], cmds->tokens) == -1) {
        printf("UH\n");
        child_success = -1;
        return -1; // TODO: make int for shell later? So if this fails you can display bad emoji
    }

    printf("execute_final_command - shouldn't reach here\n");
    child_success = -1;
    return -1;
}

void do_pipe(char **args)
{

    LOGP("STARTING DO_PIPE()____________\n");
    char *input_file = NULL;
    char *output_file = NULL;


    log_static_vars();


    struct command_line* cmds = calloc(cmds_max_len, sizeof(struct command_line));

    if (cmds == NULL) {
        LOGP("ERROR - OUT OF MEM\n");
        return;
    }

    LOG("Current args[0]:\t'%s'\n", args[0]);

    char** first_command = args; // Case: head
    char** last_command = first_command;

    set_command_line(cmds + 0, first_command, true, NULL);
    cmds_size++;


    int args_counter = 1;
    int i = 1;

    char** curr_command;
    bool already_init = false;

    while (args[i] != NULL) {

        // Init everything in cmds[arg_counter] to 0
        if (!already_init) {
            LOGP("Already_init is false; initalizing command_line\n");
            cmds[args_counter] = (struct command_line) {0};
            already_init = true;
        }

        LOG("\tARGS[%d] = '%s'\n", i, args[i]);

        if (strcmp(args[i], "|") == 0) {
            LOG("FOUND PIPE:\t'%s'\n", args[i]);
            args[i] = (char *) NULL;
             LOG("\tARGS[%d] NOW = '%s'\n", i, args[i]);

             i += 1;
             curr_command = args + i;
             last_command = curr_command;

             //LOG("ADDING TO COMMANDLINE[%d] w/ FIRST ARG = '%s'\n", args_counter, *curr_command);
             set_command_line(cmds + args_counter, curr_command, true, NULL);

             args_counter++;
             cmds_size++;
             already_init = true;

             //log_struct(cmds, args_counter - 1);
             LOG("CURRENT CMDS_SIZE:\t%d\n", cmds_size);

              if (cmds_size == cmds_max_len) {
                LOGP("OH\n");
                LOG("CMDS_SIZE == CMDS_MAX_LEN:\t %d == %d; RESIZING\n", cmds_size, cmds_max_len);
                cmds_max_len *= 2;
                cmds = (struct command_line*)realloc(cmds, cmds_max_len * sizeof(struct command_line));

                if (cmds == NULL) {
                    LOGP("OUT OF MEM\n");
                    return;
                }
            }
        }


        i++;


    }
        LOG("i is now:\t%d\n", i);
        LOG("ARGS[%d] IS:\t'%s'\n", i, args[i]);

        LOGP("SETTING LAST COMMAND\n");
        LOG("OUTPUT FILE IS\t'%s'\n", output_file);
        LOG("ARGS_COUNTER IS:\t%d\n", args_counter);
        set_command_line(cmds + args_counter -1, last_command, false, output_file);
        log_struct(cmds, args_counter - 1);




            LOGP("DOING CHILD FORK THING:\n");


            pid_t child = fork();

            // If cannot make any more children, just keep going until
            // we can make another fork
            if (child == -1) {
                perror("fork");
                return;
            }
            // Case: child
            // Note: possible for child to reset stdin stream, so have to close child
            // Linux only
            else if (child == 0) {

                if (execute_pipeline(cmds) == -1) {
                    LOGP("PROBLEM w/ EXECUTING PIPELINE\n");
                }

                // Close these 3 so that child doesn't reset data
                // Last 2 not too necessary but 1st one is
                LOGP("CLOSING\n");
                close(fileno(stdin));
                LOGP("CLOSING\n");
                close(fileno(stdout));
                LOGP("DONE CLOSING\n");
                close(fileno(stderr));
                
                // Putting this out of error case - see if it works
                // If I don't put this return outside of the error case, I think we end up with an infinite loop?
                return;
            }
            // Case: parent
            else {
                //puts("Bubba Bradley is waiting for his daughter\n");

                //LOG("LAST ARG = ARG[%d]:\t'%s'\n", tokens - 1, args[tokens-1]);
                int status;
 

                if (is_background_job) {
                  //  LOG("IS BACKGROUND JOB:\t'%s'\n", command);
                    //job_list_add(command);
                    waitpid(-1, &status, WNOHANG);
                }
                else {
                    waitpid(child, &status, 0);

                }

            

            }

        // Reset child_success
        child_success = 0;

        destroy_command_line(cmds);

}

void set_command_line(struct command_line *cmd, char **tokens, bool stdout_pipe, char *stdout_file)
{
    cmd->tokens = tokens;
    cmd->stdout_pipe = stdout_pipe;
    cmd->stdout_file = stdout_file;
}

void log_static_vars()
{
    LOG("STATIC VARS:\n"
        "\t->Current max len: %d\n"
        "\t->child_success (unused for now): %d\n",
        cmds_max_len, child_success);
}

void log_struct(struct command_line *cmds, int args_counter)
{
    LOG("COMMAND_LINE[%d]:\n"
    "-> ARGS[0]:\t'%s'\n"
    "-> STDOUT_PIPE:\t'%d'\n"
    "-> stdin_file:\t'%s'\n"
    "-> stdout_file:\t'%s'\n"
    "-> appending?:\t%d\n",
    args_counter, *(cmds[args_counter].tokens), cmds[args_counter].stdout_pipe,
    cmds[args_counter].stdin_file, cmds[args_counter].stdout_file,  cmds[args_counter].append);

}





void destroy_command_line(struct command_line *cmds)
{
    

    free(cmds);

    cmds_size = 0;
    cmds_max_len = 2;



    // Note: UNTESTED - NEED TO CHECK THIS OUT LATER
}

