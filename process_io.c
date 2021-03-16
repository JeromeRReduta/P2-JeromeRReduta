#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "process_io.h" // Note - if this @ bottom, then header file will consider bools
#include "logger.h"
#include "string.h"



/* Extern vars */
char* stdin_file;
char* stdout_file;
bool append;

/* Local func prototypes */

void p_IO_output(char **args);
void p_IO_append(char **args);
void p_IO_input(char **args);
void p_IO_clean_text(char **args);
void reset_str(char **string);

void log_all_args(char **args);
void log_extern_vars();



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






void p_IO_output(char **args)
{
	LOG("CURRENT TOKEN IS > == '%s'; PROCESSING OUTPUT\n", args[0]);
	
	stdout_file = strdup(args[1]);

	p_IO_clean_text(args);
}

void p_IO_append(char **args)
{
	LOG("CURRENT TOKEN IS >>? '%s'; PROCESSING APPEND\n", args[0]);

	append = true;

	LOGP("CALLING OUTPUT TOO\n");
	p_IO_output(args);
}

void p_IO_input(char **args)
{
	LOG("CURRENT TOKEN IS <? '%s'; PROCESSING INPUT\n", args[0]);
	stdin_file = strdup(args[1]);
	p_IO_clean_text(args);

}

// Shifts every arg 2 elems to the left, starting from args[i + 2] (2nd arg after ">" or "<" or ">>"), essentially elminating args[i] and args[i+1]
// Also moves 1st null so we don't have weird things happen

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


void p_IO_reset()
{
	reset_str(&stdin_file);
	reset_str(&stdout_file);
	append = false;


	LOGP("AFTER RESET()____________\n");
	log_extern_vars();

}

void reset_str(char **string)
{

	if (*string != NULL) {
		free(*string);
		LOGP("FREED PTR\n");
		*string = NULL;
	}

}





void test_p_IO_output()
{
	LOGP("TESTING P_IO_OUTPUT________________________\n");


	char* args[] = {"echo", "aaaaa", ">", "bubba.txt", NULL};


	LOGP("BEFORE PROCESSING:\t\n");
	log_extern_vars();
	log_all_args(args);



	int i = 0;
	while (args[i] != NULL) {

		if (strcmp(args[i], ">") == 0) {
			LOG("FOUND > :\t'%s'\n", args[i]);

			p_IO_output(args + i);
		}

		i++;
	}

	LOGP("AFTER P_IO_OUTPUT()\n");
	log_all_args(args);

	log_extern_vars();

	p_IO_reset();

}

void test_p_IO_append()
{
	LOGP("TESTING p_io_append________________________\n");


	char* args[] = {"echo", "aaaaa", ">>", "bubba.txt", NULL};


	LOGP("BEFORE PROCESSING:\t\n");
	log_extern_vars();
	log_all_args(args);



	int i = 0;
	while (args[i] != NULL) {

		if (strcmp(args[i], ">>") == 0) {
			LOG("FOUND >> :\t'%s'\n", args[i]);

			p_IO_append(args + i);
		}

		i++;
	}

	LOGP("AFTER p_io_append()\n");
	log_all_args(args);

	log_extern_vars();

	p_IO_reset();

}

void test_p_IO_input()
{

	LOGP("TESTING p_IO_input________________________\n");


	char* args[] = {"echo", "aaaaa", "<", "bubba.txt", NULL};


	LOGP("BEFORE PROCESSING:\t\n");
	log_extern_vars();
	log_all_args(args);



	int i = 0;
	while (args[i] != NULL) {

		if (strcmp(args[i], "<") == 0) {
			LOG("FOUND < :\t'%s'\n", args[i]);

			p_IO_input(args + i);
		}

		i++;
	}

	LOGP("AFTER p_IO_input()\n");
	log_all_args(args);

	log_extern_vars();


	p_IO_reset();




}
void test_p_IO_reset()
{

	LOGP("TESTING RESET_________________________________________\n\n");

	//stdin_file = strdup("a.txt");
	//stdout_file = strdup("b.txt");
	append = 1;

	LOGP("NEW EXTERN VARS\n");
	log_extern_vars();

	p_IO_reset();

	LOGP("AFTER FREEING\n");
	log_extern_vars();

}


void log_all_args(char **args)
{
	int i = 0;
	while (args[i] != NULL) {
		LOG("Args[%d]:\t'%s'\n", i, args[i]);
		i++;

	}
	LOG("Args[%d]:\t'%s'\n", i, args[i]);


}

void log_extern_vars()
{
	LOG("Extern vars:\n"
		"\t->IN:\t'%s'\n"
		"\t->OUT:\t'%s'\n"
		"\t->APPEND?:\t%d\n",
		stdin_file, stdout_file, append);

}