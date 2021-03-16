#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "pipe.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "logger.h"
#include "string.h"
#include "job_list.h"
#include "process_io.h"

/**
 * @file Class for creating and executing a pipe of commands (this includes single commands)
 */


/**
 * @brief      Structure representing one command_line entry, including tokens, whether this has a pipe char after, input and output files, and whether we are supposed to append or write outputs
 */
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

void init_command_line(struct command_line *cmd, char **tokens, bool stdout_pipe, char *stdin_file, char *stdout_file, bool append);
int execute_final_command(struct command_line *cmds);
int execute_and_pipe_command(struct command_line *cmds);
void log_struct(struct command_line *cmds, int args_counter);
void destroy_command_line(struct command_line *cmds);

void do_pipe(char **args);

/**
 * @brief      Executes each command in a pipeline
 *
 * @param      cmds  Array of command_line entries
 *
 * @return     -1 on failure; uses execvp, so doesn't return on success
 * 
 * Note: I don't think this returns properly?
 */
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

/**
 * @brief      Executes any pipeline command except the
 *
 * @param      cmds  The commands
 *
 * @return     -1 on failure; uses execvp, so doesn't return on success
 * 
 * Note: I don't think this returns properly?
 */
int execute_and_pipe_command(struct command_line *cmds)
{


    // Create pipe w/ 2 fds
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        child_success = -1;
        return -1; // TODO: Prob return -1
    }


    int input_file_fd = fd[0];

    LOG("INPUT FILE IN THIS FUNC:\t'%s'\n", stdin_file);

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
        close(input_file_fd); // Should close these - I think not closing causes infinite loop?

        if (execvp(cmds->tokens[0], cmds->tokens) == -1) {
            perror("execvp");
            LOGP("FAILED HERE\n");
            close(fd[0]);
            close(fd[1]);
            child_success = -1;
            return -1; // TODO: make int for shell later? So if this fails you can display bad emoji
        }


    }
    else {



        if ( dup2(input_file_fd, fileno(stdin)) == -1) {
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

/**
 * @brief      Executes the last command in an array of command line entries
 *
 * @param      cmds  Pointer to last command
 *
 * @return     -1 on failure; uses execvp, so doesn't return on success
 * 
 * Note: I don't think this returns properly?
 */
int execute_final_command(struct command_line *cmds)
{

    if (cmds->stdout_file != NULL) {

        LOG("stdout_file is '%s'\n", cmds->stdout_file);

        LOG("APPEND IS: %d\n", append);

        // Creates fd for output file and sets it to
        // stdout - now outputs go to this file

        // Apparently you can't add O_APPEND | O_TRUNC, or it'll clear all text before appending
        int open_flags = cmds->append ? O_RDWR | O_CREAT | O_APPEND : O_RDWR | O_CREAT | O_TRUNC;

        if (cmds->append) {
            LOG("APPEND IS TRUE %d; USING APPEND FLAGS\n", append);
        }

        int open_perms = 0666;
        int output_file_fd = open(cmds->stdout_file, open_flags, open_perms);
        // Note: Don't have to close output_file_fd - it's child process's prob now :D
        if (output_file_fd == -1) {
            perror("open");
            child_success = -1;
            return -1; // TODO: Prob do return -1
        }

        // Redirects any output from stdout to output_file_fd
        if (dup2(output_file_fd, fileno(stdout)) == -1) {
            perror("dup2");
            child_success = -1;
            return -1; // TODO: Prob return -1
        }
    }

    if (stdin_file != NULL) {
        LOG("stdin_file is '%s'\n", stdin_file);


        int open_flags = O_RDONLY | O_CREAT;

        int open_perms = 0666;

        int input_file_fd = open(stdin_file, open_flags, open_perms);

        if (input_file_fd == -1) {
            perror("open");
            child_success = -1;
            return -1;
        }

        if (dup2(input_file_fd, fileno(stdin)) == -1) {
            perror("dup2");
            child_success = -1;
            return -1;
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


/**
 * @brief      A sort of driver function for pipe - inits the command lines and executes them
 *
 * @param      args  A tokenized version of the original entry, e.g. if the original entry is
 *  "ls -l", then args is ("ls", "-l")
 *  
 * @note: If I try to condense these into sub-functions, the file descriptor magic fails
 */
void do_pipe(char **args)
{


    LOGP("STARTING DO_PIPE()____________\n");



    log_static_vars();


    struct command_line* cmds = calloc(cmds_max_len, sizeof(struct command_line));

    if (cmds == NULL) {
        LOGP("ERROR - OUT OF MEM\n");
        return;
    }

    LOG("Current args[0]:\t'%s'\n", args[0]);

    char** first_command = args; // Case: head
    char** last_command = first_command;

    init_command_line(cmds + 0, first_command, true, stdin_file, NULL, false);
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
             init_command_line(cmds + args_counter, curr_command, true, NULL, NULL, false);




             args_counter++;
             cmds_size++;
             already_init = true;

             //log_struct(cmds, args_counter - 1);
             LOG("CURRENT CMDS_SIZE:\t%d\n", cmds_size);

              if (cmds_size == cmds_max_len) {
                LOGP("OH\n");
                LOG("CMDS_SIZE == CMDS_MAX_LEN:\t %d == %d; RESIZING\n", cmds_size, cmds_max_len);
                cmds_max_len *= 2;
                struct command_line* new_cmds = (struct command_line*)realloc(cmds, cmds_max_len * sizeof(struct command_line));

                if (new_cmds == NULL) {
                    LOGP("OUT OF MEM\n");
                    free(cmds);
                }
               else {
                cmds = new_cmds;
               }
            }
        }


        i++;


    }
        LOG("i is now:\t%d\n", i);
        LOG("ARGS[%d] IS:\t'%s'\n", i, args[i]);

        LOGP("SETTING LAST COMMAND\n");
        LOG("OUTPUT FILE IS\t'%s'\n", stdout_file);
        LOG("ARGS_COUNTER IS:\t%d\n", args_counter);
        init_command_line(cmds + args_counter -1, last_command, false, NULL, stdout_file, append);

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
                    LOG("IS BACKGROUND JOB:\t'%s'\n", background_command);
                    job_list_add(background_command);
                    waitpid(-1, &status, WNOHANG);
                }
                else {
                    waitpid(child, &status, 0);

                }

                //LOG("STATUS:\t%d___________________________________________\n", status);
            

            }

        // Reset child_success
        child_success = 0;

        destroy_command_line(cmds);

}

/**
 * @brief      Initializes the values of one command_line struct
 *
 * @param      cmd          Ptr to command-line struct
 * @param      tokens       commandline arguments, i.e. the things you type in terminal
 * @param[in]  stdout_pipe  whether this command will be piped to another (i.e. if this is not the last command)
 * @param      stdin_file   input file
 * @param      stdout_file  output file
 * @param[in]  append       Whether to append output
 */
void init_command_line(struct command_line *cmd, char **tokens, bool stdout_pipe, char *stdin_file, char *stdout_file, bool append)
{
    cmd->tokens = tokens;
    cmd->stdout_pipe = stdout_pipe;
    cmd->stdin_file = stdin_file;
    cmd->stdout_file = stdout_file;
    cmd->append = append;
}

/**
 * @brief      Logging function, because I wanted to use this multiple times
 */
void log_static_vars()
{
    LOG("STATIC VARS:\n"
        "\t->Current max len: %d\n"
        "\t->child_success (unused for now): %d\n",
        cmds_max_len, child_success);
}

/**
 * @brief      Logging function, because this would be WAAAAY too big to copy and paste
 *
 * @param      cmds          Ptr to command-line struct
 * @param[in]  args_counter  num of command_line structs, used to reference a given command_line
 */
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

/**
 * @brief      Resets all data in this file, including freeing allocated memory
 *
 * @param      cmds  Ptr to the one command_line struct used here
 */
void destroy_command_line(struct command_line *cmds)
{
    

    free(cmds);

    cmds_size = 0;
    cmds_max_len = 2;



    // Note: UNTESTED - NEED TO CHECK THIS OUT LATER
}

