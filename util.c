#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // Need this?
#include "ui.h"
#include <unistd.h>
#include "logger.h"
#include <math.h>

/**
 * @file
 * 
 * Utility functions that don't really belong to other classes
 * 
 * Maybe should change this into a bunc of tiny files
 */


/**
 * Retrieves the next token from a string.
 *
 * Parameters:
 * - str_ptr: maintains context in the string, i.e., where the next token in the
 *   string will be. If the function returns token N, then str_ptr will be
 *   updated to point to token N+1. To initialize, declare a char * that points
 *   to the string being tokenized. The pointer will be updated after each
 *   successive call to next_token.
 *
 * - delim: the set of characters to use as delimiters
 *
 * Returns: char pointer to the next token in the string.
 */
char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    } else {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }

    return current_ptr;
}


/**
 * @brief      Changes current directory to args[1]  
 *
 * @param      args  Command-line args
 */
void cd_with(char **args)
{
    char* path = args[1] != NULL ? args[1] : home_dir;
    
    LOG("COMMAND HAS CD:\t'%s'\n", args[0]);
    LOG("PATH:\t'%s'\n", path);
    if (chdir(path) == -1) {
        perror("chdir");
    }

}

// Found this implementation from https://stackoverflow.com/questions/15515088/how-to-check-if-string-starts-with-certain-string-in-c/15515276


/**
 * @brief      Returns whether a given word starts with a given prefix
 *
 * @param[in]  word    word
 * @param[in]  prefix  prefix
 *
 * @return     Whether the given word starts with the given prefix
 */
int starts_with(const char *word, const char *prefix)
{
    return strncmp(word, prefix, strlen(prefix)) == 0;
}


/**
 * @brief      Returns the index from going n indices backward in a circular array
 *
 * @param[in]  i        Starting index
 * @param[in]  n        # of indices to go backward
 * @param[in]  max_len  max length of circular array
 *
 * @return     Index from going n indices backward from index i
 */

int circ_array_go_back_by(int i, int n, int max_len)
{
    return abs((100 + i - n) % max_len);

}

/**
 * @brief      Returns the index from going n indices forward in a circular array
 *
 * @param[in]  i        Starting index
 * @param[in]  n        # of indices to go foward
 * @param[in]  max_len  max length of circular array
 *
 * @return     Index from going n indices forward from index i
 */
int circ_array_go_forward_by(int i, int n, int max_len)
{
    return abs((100 + i + n) % max_len);

}

