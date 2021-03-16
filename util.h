/**
 * @file
 * 
 * Utility functions
 */

#ifndef _UTIL_H_
#define _UTIL_H_

char *next_token(char **str_ptr, const char *delim);
void cd_with(char **args);
int starts_with(const char *word, const char *prefix);
int circ_array_go_back_by(int i, int n, int max_len);
int circ_array_go_forward_by(int i, int n, int max_len);
#endif