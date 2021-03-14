/**
 * @file
 * 
 * Utility functions
 */

#ifndef _UTIL_H_
#define _UTIL_H_

char *next_token(char **str_ptr, const char *delim);
void cd_with(char **args);
int starts_with(char *word, char *prefix);
#endif