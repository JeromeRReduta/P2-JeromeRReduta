/**
 * @file
 *
 * Contains data structs and funcs for processing IO chars in char** args
 */

#ifndef _PROCESS_IO_H_
#define _PROCESS_IO_H_

extern char* stdin_file;
extern char* stdout_file;
extern bool append;

// TODO: Remove all these once done w/ testing
void test_p_IO_output(); // Confirmed success
void test_p_IO_reset(); // Confirmed success for: only input set, only output set, input and output set
	// Not testing for bool b/c it's just an int, but bool was set to 0 in all cases
void test_p_IO_append(); // Confirmed success
void test_p_IO_input(); // Confirmed success


void p_IO_process_IO_chars(char **args);


void p_IO_reset();


#endif