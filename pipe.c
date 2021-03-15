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

struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
    char* stdin_file;

    bool is_appending;
};

static int cmds_size = 0;
static int cmds_max_len = 2;



/* Function prototypes */
int run_process(struct command_line *cmds);
void set_command_line_tokens_and_pipe(struct command_line *cmd, char **tokens, bool stdout_pipe);
void destroy_command_line(struct command_line *cmds);
void log_cmd_stats(struct command_line *cmds, int args_counter);
void redirect_output(struct command_line *cmds, int args_counter, int i, char **args);
void clean_file_text(char **args, int *i_ptr, char **curr_command);


void execute_pipeline(struct command_line *cmds)
{
    /**
     * TODO: design an algorithm that sets up a pipeline piece by piece.
     * Solutions will probably either iterate over the pieces of the pipeline or
     * work recursively. Imagine you have three commands in a pipeline:
     *
     *  - cat
     *  - tr
     *  - sed
     *
     * Use 'stdout_pipe' to determine when you've reached the last command in
     * the pipeline, and 'stdout_file' to decide whether the final result gets
     * written to a file or the terminal.
     *
     * If we aren't at the last command, then we need to set up a pipe for the
     * current command's output to go into. For example, let's say our command
     * is `cat file.txt`. We will create a pipe and have the stdout of the
     * command directed to the pipe. Before running the next command, we'll set
     * up the stdin of the next process to come from the pipe, and
     * execute_pipeline will run whatever command comes next (for instance,
     * `tr '[:upper:]' '[:lower:]'`).
     *
     * Here's some pseudocode to help:
     *
     * create a new pipe
     * fork a new process
     * if pid is the child:
     *     dup2 stdout to pipe[1]
     *     close pipe[0]
     *     execvp the command
     * if pid is the parent:
     *     dup2 stdin to pipe[0]
     *     close pipe[1]
     *     move on to the next command in the pipeline
     *
     * The special case here is when there are no more commands left. In that
     * case, you can simply execvp the command (no need to create another pipe).
     * If you created a handler process in main(), then it will be replaced by
     * this last call to execvp.
     */


    // printf("SIZEOF COMMANDLINE[3]:\t%d\n", num_of_commands);

    // Change this to num_cof_commands (int param) and figure out how to fit htis
    for (int i = 0; i < cmds_size; i++) {
        
        int success = run_process(cmds + i);

        if (success == -1) {
            LOGP("WE MESSED UP\n");
            return;
        }
    }
  
    LOGP("________EXECUTE_PIPELINE() FINISHED________\n");

}

/// A lot of this lovingly adapted from lab code
int run_process(struct command_line *cmds)
{
   if (cmds->stdout_pipe == false) {
       if (cmds->stdout_file != NULL) {
     
           int open_flags = O_RDWR | O_CREAT | O_TRUNC;
           int open_perms = 0666;
           int fd = open(cmds->stdout_file, open_flags, open_perms);

           if (fd == -1) {
               perror("open");
               return -1;
           }
           dup2(fd, STDOUT_FILENO);
       }

       // Forgot that b/c I don't have a helper process in execute_pipeline(), I need to make a fork here...
        // Need to sacrifice a child for execvp (wait that sounds p bad)
       pid_t child = fork();

        if (child == -1) {
            perror("fork");
            return -1;
       }
       else if (child == 0) {

           if (execvp(cmds->tokens[0], cmds->tokens) == -1) {
               perror("execvp");
               return -1;
            }

       }
       else {
        int status;
        waitpid(child, &status, 0);
        LOGP("DONE\n");
       }

    
   }
   
    // Create new pipe
    int fd[2];

    if (pipe (fd) == -1) {
         return -1;
     }

    pid_t child = fork();

    
    if (child == -1) {
        perror("fork");
        return -1;
    }
    // Child case - execute program
    else if (child == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        execvp(cmds->tokens[0], cmds->tokens);
    }
    else {
        // Parent
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
    }

    LOGP("FINISHED_______________\n");
    return 0;

}

// Note: command should already be tokenized by whitespace
void test_pipe_one_arg(char **args)
{

    char *input_file = NULL;
    char *output_file = NULL;



    LOGP("CURRENT ARGS:\n");
    int i = 0;
    while (args[i] != NULL) {

        LOG("\tARGS[%d] = '%s'\n", i, args[i]);

        if (strcmp(args[i], "|") == 0) {
            LOG("FOUND |:\t'%s' @ index %d\n", args[i], i);
        }
        i++;
    }
    LOG("\tARGS[%d] = '%s'\n", i, args[i]);

    char** command1 = args;





    struct command_line cmds[3] = { 0 };

   // set_command_line_tokens_and_pipe(&(cmds[0]), command1, false, NULL);

    execute_pipeline(cmds);

    //create_pipe("ls -l");

}

// CONFIRMED TO WORK ON 2 and 3 ARGS
void run_with_pipe(char **args)
{
    char *input_file = NULL;
    char *output_file = NULL;

    // TODO: Create dynamically-sized cmd_line struct and thing to track size (like command_line_sz and if (args_count >= command_line_sz grow array))


    struct command_line* cmds = calloc(2, sizeof(struct command_line));


    if (cmds == NULL) {
        LOGP("ERROR - OUT OF MEM");
        return;
    }


    LOGP("CURRENT ARGS:\n");

    char** first_command = args; // Case: head
    char** last_command = first_command;

    set_command_line_tokens_and_pipe(&(cmds[0]), first_command, true);
    cmds_size++;

    int args_counter = 1;
    int i = 1;



    /* 
        While we still have tokens:
            If found pipe: 
                1. Change this token to NULL
                2. Set curr_command to next token
                3. set commandline (cmds[args_counter], curr_command, true, NULL, NULL, false)
                4. args_counter++; cmds_size++;

                5. Resize cmds if necessary

            If found >, <, or >>:
                1. Add permissions and things as necessary to cmd[args_counter]

    */

    char** curr_command;

    bool already_init = false;
    while (args[i] != NULL) {

        // Initailize everything in cmds[args_counter] to 0
        if (!already_init) {
            cmds[args_counter] = (struct command_line) {0};
            already_init = true;
        }

         LOG("\tARGS[%d] = '%s'\n", i, args[i]);


        if (strcmp(args[i], ">") == 0) {
            LOG("FOUND >: '%s'\n", args[i]);
         /*
            redirect_output(cmds, arg_counter, i);
            clean_file_text(args, &i, curr_command);
            */
        }
        /*
        else if(strcmp(args[i], ">>") == 0) {
            LOG("FOUND >>: '%s'\n", args[i]);
            append_output(...)
        }
        else if (strcmp(args[i], "<") == 0) {
            LOG("FOUND <: '%s'\n", args[i]);
            redirect_input(...)
        }
        */
        else if (strcmp(args[i], "|") == 0) {
            LOG("FOUND PIPE:\t'%s'\n", args[i]);

            args[i] = (char *)NULL;

            LOG("REPLACED w/ NULL: ARGS[%d] = '%s'\n", i, args[i]);

            i += 1;
            curr_command = args + i;
            last_command = curr_command;

            LOG("ADDING TO COMMANDLINE w/ FIRST ARG = '%s'\n", *curr_command);
            set_command_line_tokens_and_pipe(&(cmds[args_counter]), curr_command, true);

            args_counter++;
            cmds_size++;
            already_init = false;

            log_cmd_stats(cmds, args_counter-1);

            LOG("CURRENT CMDS_SIZE:\t%d\n", cmds_size);
            if (cmds_size == cmds_max_len) {
                LOGP("OH\n");
                LOG("CMDS_SIZE == CMDS_MAX_LEN:\t %d == %d; RESIZING\n", cmds_size, cmds_max_len);
                cmds_max_len *= 2;
                cmds = realloc(cmds, cmds_max_len * sizeof(struct command_line));

                if (cmds == NULL) {
                    LOGP("OUT OF MEM\n");
                    return;
                }
            }


        }

        i++;

        LOG("i is now:\t%d\n", i);
        LOG("ARGS[i] IS:\t'%s'\n", args[i]);
    }
    
    set_command_line_tokens_and_pipe(&(cmds[args_counter-1]), last_command, false);

    LOG("LAST COMMAND_LINE[%d]:\n"
                "-> ARGS[0]:\t'%s'\n"
                "-> STDOUT_PIPE:\t'%d'\n"
                "-> stdin_file:\t'%s'\n"
                "-> stdout_file:\t'%s'\n"
                "-> appending?:\t%d\n",
                args_counter,
                *(cmds[args_counter-1].tokens),
                cmds[args_counter-1].stdout_pipe,
                cmds[args_counter-1].stdin_file,
                cmds[args_counter-1].stdout_file, 
                cmds[args_counter-1].is_appending );


    LOGP("---------------------EXECUTING--------------------\n");

    LOG("FIRST CMD STDOUT_FILE NULL?\t%d\n", cmds[0].stdout_file == NULL);
    execute_pipeline(cmds);


    destroy_command_line(cmds);




}

// Literally made this b/c log statement was too big
void log_cmd_stats(struct command_line *cmds, int args_counter)
{

    LOG("COMMAND_LINE[%d]:\n"
    "-> ARGS[0]:\t'%s'\n"
    "-> STDOUT_PIPE:\t'%d'\n"
    "-> stdin_file:\t'%s'\n"
    "-> stdout_file:\t'%s'\n"
    "-> appending?:\t%d\n",
    args_counter, *(cmds[args_counter].tokens), cmds[args_counter].stdout_pipe,
    cmds[args_counter].stdin_file, cmds[args_counter].stdout_file,  cmds[args_counter].is_appending );
}

// Note - going to have to log this extensively
void redirect_output(struct command_line *cmds, int args_counter, int i, char **args)
{
    cmds[args_counter].stdout_file = args[i + 1];



}

void clean_file_text(char **args, int *i_ptr, char **curr_command)
{

    args[*i_ptr] = NULL;
    args[*i_ptr + 1] = NULL;
    *i_ptr += 2;
    curr_command = args + *i_ptr;

}
/*

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    char *output_file = NULL;

    if (argc < 2 || argc > 3) {
        printf("Usage: %s file-to-leetify [output-file]\n", argv[0]);
        return 1;
    }

    input_file = argv[1];

    if (argc == 3) {
        output_file = argv[2];
    }



    char *command1[] = { "cat", input_file, (char *) NULL };
    char *command2[] = { "tr", "[:upper:]", "[:lower:]", (char *) NULL };
    char *leet = "s|the|teh|g; s|a|4|g; s|e|3|g; s|i|!|g; s|l|1|g; s|o|0|g; s|s|5|g;";
    char *command3[] = { "sed", leet, (char *) NULL };
    struct command_line cmds[3] = { 0 };

    set_command_line_tokens_and_pipe(&(cmds[0]), command1, true, NULL);
    set_command_line_tokens_and_pipe(&(cmds[1]), command2, true, NULL);
    set_command_line_tokens_and_pipe(&(cmds[2]), command3, false, output_file);

   


    execute_pipeline(cmds);

    return 0;
}
*/

// stdin_file, stdout_file, and is_appending all 0 by default (null for char*s, false for bools)
// Will update in process_IO_redirection as needed
void set_command_line_tokens_and_pipe(struct command_line *cmd, char **tokens, bool stdout_pipe)
{
    cmd->tokens = tokens;
    cmd->stdout_pipe = stdout_pipe;
}

void destroy_command_line(struct command_line *cmds)
{
    

    free(cmds);


    cmds_size = 0;
    cmds_max_len = 2;



    // Note: UNTESTED - NEED TO CHECK THIS OUT LATER
}

