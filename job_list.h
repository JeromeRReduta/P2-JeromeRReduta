/**
 * @file
 *
 * Contains data and funcs for manipulating background job list
 */

#ifndef _JOB_LIST_H_
#define _JOB_LIST_H_

void job_list_init();
void job_list_destroy();
void job_list_add(char *job_name);
void job_list_pop();

void job_list_print();

extern bool is_background_job;


#endif