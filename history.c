#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "history.h"
#include "logger.h"
#include "string.h"
#include "util.h"

/**
 * @file data and functions for keeping track and manipulating a history list (i.e. a list of previously-entered commands)
 */





static int command_nums[100] = {0};
static char* lines[100] = {0};
static int counter = 0;
static int max_len = 0;
static int current_len = 0;


static int tail_index = 0;


/* Func prototypes */


/**
 * @brief      Initalizes a circular array to hold previous commands
 *
 * @param[in]  limit  Size of array
 */
void hist_init(unsigned int limit)
{

    max_len = limit;
    current_len = 0;

}

/**
 * @brief      Resets all history data, including freeing allocated memory
 */
void hist_destroy(void)
{

    for (int i = 0; i < max_len; i++) {
        if (lines[i] != NULL) {
            free(lines[i]);
        }
        command_nums[i] = 0;

    }
    current_len = 0;
    tail_index = 0;
    max_len = 0;

}


/**
 * @brief      Adds one string to history array
 *
 * @param[in]  cmd   command string
 */
void hist_add(const char *cmd)
{

    //LOG("LOGGING CMD:\t'%s'\n", cmd);
    command_nums[counter % max_len] = counter+1;


    // strlen(cmd) - 1 to exclude \n @ end
    lines[counter % max_len] = strndup(cmd, strlen(cmd)); // TODO: FREE ALL LINES LATER

    char* newline_loc = strstr(lines[counter % max_len], "\n");
    if (newline_loc != NULL) {
        *newline_loc = '\0';
    }

    
    tail_index = (tail_index + 1) % max_len;


    //LOG("Hist command %d = '%s'\ttail index is now %d \n", command_nums[counter % max_len], lines[counter % max_len], tail_index);
    
    counter++;

    if (current_len < 100) {
        current_len++;
        
    }

    LOG("CURRENT_LEN IS NOW:\t%d\n", current_len);
    
}

/**
 * @brief      Prints the contents of the history array, starting from the one with the lowest command number
 */
void hist_print(void)
{
    LOG("PRINTING:\n"
        "\tcirc_array_go_back_by(tail_index, current_len-1, max_len):\t%d\n"
        "\ttail_index:\t%d\n",
        circ_array_go_back_by(tail_index, current_len, max_len),
        tail_index);


    // Case: Head (first entry)

    int i = circ_array_go_back_by(tail_index, current_len, max_len);
    printf("    %d  %s\n", command_nums[i], lines[i]);
   
    // Case: Everything except tail
    while (i != tail_index - 1 ) {
        i = circ_array_go_forward_by(i, 1, max_len);
        printf("    %d  %s\n", command_nums[i], lines[i]);
       
    }
    fflush(stdout); // Hey, this is pretty essential for printing stuff in right order...
 

}

/**
 * @brief      Searches for a string in the history array, based on prefix
 *
 * @param      prefix  prefix
 *
 * @return     A matching string, or NULL if no match
 */
const char *hist_search_prefix(char *prefix)
{

    /*
            LOG("WHICH IS LARGEST?\n"
            "\t->line[counter-1]:\t'%s'\n"
            "\t->line[counter]:\t'%s'\n"
            "\t->line[counter+1]:\t'%s'\n",
            hist_search_cnum(counter-1), hist_search_cnum(counter), hist_search_cnum(counter+1));

            LOG("WHICH NUM IS LARGEST?\n"
                "\t->line[counter-1]:\t'%d'\n"
                "\t->line[counter]:\t'%d'\n"
                "\t->line[counter+1\t %d \n",
                command_nums[(counter -1) % max_len - 1],
                command_nums[(counter) % max_len - 1],
                command_nums[(counter + 1) % max_len - 1]);
    */
    LOG("PREFIX:\t'%s'\n", prefix);
    // Case: Head (last entry)
    int i = tail_index - 1;

    if (starts_with(lines[i], prefix)) {
        LOG("LINES[I]:\t%s\tPREFIX:\t%s\n", lines[i], prefix);
        return lines[i];
    }
    while (i != tail_index) {
        i = circ_array_go_back_by(i, 1, max_len);
          LOG("i:\t%d\tcommand_num:\t%d\tlines[i]\t%s\n", i, command_nums[i], lines[i]);
        if (starts_with(lines[i], prefix)) {
            LOG("LINES[I]:\t%s\tPREFIX:\t%s\n", lines[i], prefix);
            return lines[i];
        }
    }

    LOGP("NOTHING MATCHES: RETURNING NULL\n");
    return NULL;

}

/**
 * @brief      Searches for a string in history array by command number
 *
 * @param[in]  command_number  command number
 *
 * @return     The matching string, or NULL if there is none
 */
const char *hist_search_cnum(int command_number)
{


    if (command_number <= counter) {
        LOGP("COMMAND NUM <= COUNTER\n");

        int i = circ_array_go_back_by(command_number, 1, max_len);

        LOG("LOOKING FOR COMMAND W/ NUM:\t%d\n"
            "FOUND:\n"
                "\t->COUNTER:\t%d\n"
                "\t->COMMAND_NUM:\t%d\n"
                "\t->CONTENT:\t%s\n"
                "\t->i:\t%d\n",
            command_number, counter, command_nums[i], lines[i], i);

        return command_number == command_nums[i] ? lines[i] : NULL;
    }
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    return NULL;
}

/**
 * @brief      Returns the most recent command num (which just so happens to = counter)
 *
 * @return     Most recent command num
 */
unsigned int hist_last_cnum(void)
{

    return counter;
}


