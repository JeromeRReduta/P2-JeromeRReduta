/**
 * @file
 *
 * Contains data structs and funcs for processing IO chars in char** args
 */
#include <stdbool.h>
#ifndef _PROCESS_IO_H_
#define _PROCESS_IO_H_

extern char* stdin_file;
extern char* stdout_file;
extern bool append;

void p_IO_process_IO_chars(char **args);
void p_IO_reset();

#endif