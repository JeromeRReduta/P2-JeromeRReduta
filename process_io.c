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

void p_IO_clean_text(char **args);
void reset_str(char **string);

void log_all_args(char **args);
void log_extern_vars();

/* TODO: Implmeent the following: */







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

	p_IO_output(args);
}

void p_IO_input(char **args)
{
	LOG("CURRENT TOKEN IS <? '%s'; PROCESSING INPUT\n", args[0]);
	stdin_file = strdup(args[1]);
	p_IO_clean_text(args);

}



void p_IO_clean_text(char **args) {
	args[0] = "\0";
	args[1] = "\0";
}


void p_IO_reset()
{
	reset_str(&stdin_file);
	reset_str(&stdout_file);
	append = false;

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