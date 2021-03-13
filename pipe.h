/**
 * @file
 * 
 * Utility functions
 */

#ifndef _PIPE_H_
#define _PIPE_H_

int run_process(struct command_line *cmds);
void set_command_line(struct command_line *cmd, char **tokens, bool stdout_pipe, char *stdout_file);

void execute_pipeline(struct command_line *cmds);
#endif