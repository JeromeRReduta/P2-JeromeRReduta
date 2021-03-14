#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // Need this?
#include "ui.h"
#include <unistd.h>
#include "logger.h"

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
int starts_with(char *word, char *prefix)
{
    return strncmp(word, prefix, strlen(prefix)) == 0;
}
