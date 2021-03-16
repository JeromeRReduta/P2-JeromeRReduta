#include <stdlib.h>
#include <stdio.h>


#include "logger.h"
#include "string.h"
#include "process_io.h"


/**
 * @file Functions and data for processing IO chars
 */

/* Extern vars */
char* stdin_file;
char* stdout_file;
bool append = false;

/* Local func prototypes */
void p_IO_output(char **args);
void p_IO_append(char **args);
void p_IO_input(char **args);
void p_IO_clean_text(char **args);
void reset_str(char **string);

void log_all_args(char **args);
void log_extern_vars();



/**
 * @brief      A sort of driver function that:
 * 				1. Parses a list of string tokens for certain chars that point to filenames
 * 				2. Adds the filename the appropriate place (stdin_file or stdout_file)
 * 				3. Sets append if necessary
 * 				4. Removes that char and the filename from the list
 *						
 * @param      args  List of string tokens
 */	
void p_IO_process_IO_chars(char **args)
{

	int i = 0;

	while (args[i] != NULL) {

		if (strcmp(args[i], ">") == 0) {
			LOG("FOUND >? '%s'\n", args[i]);

			LOGP("BEFORE OUTPUT()___________________\n");
			log_all_args(args);
			p_IO_output(args + i);
			LOGP("AFTER OUTPUT()_____________________\n");
			log_all_args(args);
			log_extern_vars();
		}
		else if (strcmp(args[i], ">>") == 0) {
			LOG("FOUND >>? '%s'\n", args[i]);

			LOGP("BEFORE APPEND()___________________\n");
			log_all_args(args);
			p_IO_append(args + i);

			LOGP("AFTER APPEND()_____________________\n");
			log_all_args(args);
			log_extern_vars();
		}
		else if (strcmp(args[i], "<") == 0) {
			LOG("FOUND <? '%s'\n", args[i]);

			LOGP("BEFORE INPUT()___________________\n");
			log_all_args(args);
			p_IO_input(args + i);

			LOGP("AFTER INPUT()_____________________\n");
			log_all_args(args);
			log_extern_vars();
		}


		else {
			i++;
		}
	}
}


/**
 * @brief      Assuming that args points to &gt;, stores args[1] (a filename) as stdout_file.
 *  Then removes &gt; and the next token (filename) from the list.
 *
 * @param      args  Assumed ptr to &gt;
 */
void p_IO_output(char **args)
{
	LOG("CURRENT TOKEN IS > == '%s'; PROCESSING OUTPUT\n", args[0]);
	
	stdout_file = strdup(args[1]);

	p_IO_clean_text(args);
}

/**
 * @brief      Assuming that args points to &gt;&gt;, stores args[1] (a filename) as stdout_file
 *  and sets append to true. Then removes &gt;&gt; and the next token (filename) from the list.
 *
 * @param      args  Assumed ptr to &gt;&gt;
 */
void p_IO_append(char **args)
{
	LOG("CURRENT TOKEN IS >>? '%s'; PROCESSING APPEND\n", args[0]);

	append = true;

	LOGP("CALLING OUTPUT TOO\n");
	p_IO_output(args);
}

/**
 * @brief      Assuming that args points to &lt;, stores args[1] (a filename) as stdout_file
 *  and sets append to true. Then removes &lt; and the next token (filename) from the list.
 *
 * @param      args  Assumed ptr to &lt;
 */
void p_IO_input(char **args)
{
	LOG("CURRENT TOKEN IS <? '%s'; PROCESSING INPUT\n", args[0]);
	stdin_file = strdup(args[1]);
	p_IO_clean_text(args);

}


/**
 * @brief      Removes args[0] and args[1] (assumed IO char and filename) from the list
 *
 * @param      args  Assumed ptr to IO char
 */
void p_IO_clean_text(char **args) {
	
	int i = 0;

	LOG("args[2] = '%s'\n", args[i + 2]);

	while (args[i+2] != NULL) {

		LOG("args[%d] becomes args[%d] - '%s' becomes '%s'\n", i, i+2, args[i], args[i+2]);
		args[i] = args[i+2];
		i++;
	}
	args[i] = args[i + 2]; // Moving null-terminator to spots to left


}

/**
 * @brief      Resets all data in this file, including freeing allocated memory
 */
void p_IO_reset()
{
	reset_str(&stdin_file);
	reset_str(&stdout_file);
	append = false;


	LOGP("AFTER RESET()____________\n");
	log_extern_vars();

}

/**
 * @brief      Given a string ptr's address, frees the string ptr and sets its value to NULL (if not already NULL)
 *
 * @param      string  Ptr to a string ptr
 */
void reset_str(char **string)
{

	if (*string != NULL) {
		free(*string);
		LOGP("FREED PTR\n");
		*string = NULL;
	}

}


/**
 * @brief      Convenience function for logging all args. Used to compare args after mutating data.
 *
 * @param      args  list of string tokens
 */
void log_all_args(char **args)
{
	int i = 0;
	while (args[i] != NULL) {
		LOG("Args[%d]:\t'%s'\n", i, args[i]);
		i++;

	}
	LOG("Args[%d]:\t'%s'\n", i, args[i]);
}

/**
 * @brief      Convenience function for logging all extern variables. Used to compare data before and after mutating.
 */
void log_extern_vars()
{
	LOG("Extern vars:\n"
		"\t->IN:\t'%s'\n"
		"\t->OUT:\t'%s'\n"
		"\t->APPEND?:\t%d\n",
		stdin_file, stdout_file, append);
}