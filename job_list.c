// Class whose sole responsibility is to manipulate "job list" data
// Job list = list of background jobs, represented as { "job_0_name", "job_1_name",...}
// Name is kind of misleading, but I don't want to type "background jobs," or ...bjobs...

#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "util.h"
#include "job_list.h"

/**
 * @file Data and functions for editing and keeping track of a list of background jobs
 * 
 * Many of these funcs are prefixed with job_list. I tried going for a shorter prefix that included "background" in some way, but "bjobs_func()" didn't sit right with me for some reason
 */

// Apparently I can't init values in struct

/**
 * Struct representing a list of background jobs
 */
typedef struct {

	// list of job names - circular array
	// Why 25 indices? 10 (for active tasks) + 10 (for inactive tasks, so don't have to use linkedlist, and so there's no chance of overriding an active task) + 5 (to make me feel better)
	char* job_list[25];

	// Array of bools - tracks whether a job with the same index is running
	bool running[25];

	// Ptrs to head and tail of list
	int head_index;
	int tail_index;


	int list_real_len;
	int size;
	int limit;
} Job_List;

static Job_List* jobs;

/**
 * @brief      Initializes job list
 */
void job_list_init()
{
	jobs = calloc(1, sizeof(Job_List));

	jobs->list_real_len = 25;
	jobs->limit = 10;

	LOGP("DONE INITALIZING JOBS:\n");

	LOG("JOB STATS:\n"
		"\t->job_list[0]:\t'%s'\n"
		"\t->running[0]:\t%d\n"
		"\t->real size:\t%d\n"
		"\t->current size:\t%d\n"
		"\t->head pts to:\t'%d'\n"
		"\t->max # of elems is:\t%d\n",
		jobs->job_list[0], jobs->running[0], jobs->list_real_len, jobs->size, jobs->head_index, jobs->limit);

}

/**
 * @brief      Adds one job to the job list
 *
 * @param      job_name  job name
 */
void job_list_add(char *job_name)
{

	if (jobs->size == jobs->limit) {
		LOG("CAN'T HOLD ANY MORE THINGS - SIZE == LIMIT:\t%d == %d\n", jobs->size, jobs->limit);
		return;
	}

	LOG("ADDING TO JOB LIST:\t'%s'\n", job_name);
	jobs->tail_index = circ_array_go_forward_by(jobs->tail_index, 1, jobs->list_real_len);

	int new_tail_index = jobs->tail_index;

	jobs->job_list[new_tail_index] = strdup(job_name);
	jobs->running[new_tail_index] = 1;

	LOG("OLD SIZE:\t%d\n", jobs->size);
	jobs->size++;
	LOG("NEW SIZE:\t%d\n", jobs->size);




}

/**
 * @brief      Removes the head of the job list
 * 
 * Note: This doesn't work
 */
void job_list_pop()
{

	if (jobs->size > 0) {
		LOG("CLEARING HEAD:\t'%s'\n", jobs->job_list[jobs->head_index]);


		int head_index = jobs->head_index;

		jobs->running[head_index] = 0;
		jobs->head_index++;
		LOG("OLD SIZE:\t%d\n", jobs->size);
		jobs->size--;


		LOG("DECREASED TO:\t%d\n", jobs->size); // Not sure why size is off but it works
	}


}

/**
 * @brief      Frees all job names and sets them to NULL
 * 
 * Note: Needs testing
 */
void job_list_destroy()
{

	int len = jobs->list_real_len;
	for (int i = 0; i < len; i++) {

		char* current = jobs->job_list[i];

		if (current != NULL) {
			free(current);
		}

	}


	free(jobs);
	jobs = NULL;
	


}

/**
 * @brief      Prints the entries in the job list, starting w/ the first index
 */
void job_list_print()
{
	LOGP("STARTING PRINT()\n");
	int len = jobs->list_real_len;
	char** list = jobs->job_list;

	for (int count = 0; count < len; count++) {
		int i = circ_array_go_forward_by(jobs->head_index, count, jobs->list_real_len);

		if (list[i] != NULL && jobs->running[i] != 0) {
			printf("[%d] %s\n", i, list[i]);
		}

		// Note: This won't show on test b/c parent is killed to see what list actually looks like
		//printf("PLACEHOLDER[%d]: BUBBA\n", count);
	}

	LOGP("PRINT FINISHED\n");

}


