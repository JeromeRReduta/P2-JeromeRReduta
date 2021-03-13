#include <stddef.h>
#include <stdlib.h>

#include "history.h"
#include "logger.h"
#include "string.h"

static int command_nums[100] = {0};
static char* lines[100] = {0};
static int counter = 0;
static int max_len;
static int current_len;


static int tail_index;


/* Func prototypes */
int circ_array_go_back_by(int i, int n);
int circ_array_go_forward_by(int i, int n);

void hist_init(unsigned int limit)
{

    max_len = limit;
    current_len = 0;

    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.

    // TODO: Figure out how limit works into this
}

void hist_destroy(void)
{

    // free(&hist); // Not in heap, so don't have to free?

}

// TODO: change counter % max_len to counter % max_len once done w/ testing
void hist_add(const char *cmd)
{

    LOG("LOGGING CMD:\t'%s'\n", cmd);
    command_nums[counter % max_len] = counter+1;


    // strlen(cmd) - 1 to exclude \n @ end
    lines[counter % max_len] = strndup(cmd, strlen(cmd) - 1); // TODO: FREE ALL LINES LATER

    
    tail_index = (tail_index + 1) % max_len;


    LOG("Hist command %d = '%s'\ttail index is now %d \n", command_nums[counter % max_len], lines[counter % max_len], tail_index);
    
    counter++;

    if (current_len < 100) {
        current_len++;
        
    }

    LOG("CURRENT_LEN IS NOW:\t%d\n", current_len);
    
}

void hist_print(void)
{
    LOG("PRINTING:\n"
        "\tcirc_array_go_back_by(tail_index, current_len-1):\t%d\n"
        "\ttail_index:\t%d\n",
        circ_array_go_back_by(tail_index, current_len),
        tail_index);

    int i = circ_array_go_back_by(tail_index, current_len)- 1;


    // Case: Everything except tail
    while (i != tail_index - 2 ) {
         i = circ_array_go_forward_by(i, 1);
        LOG("I IS NOW:\t%d\n", i);
        printf("    %d  %s\n", command_nums[i], lines[i]);
       
    }
    // Case: tail (most recent entry)
    i = circ_array_go_forward_by(i, 1);
    printf("    %d  %s\n", command_nums[i], lines[i]);

}

const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    return NULL;
}

const char *hist_search_cnum(int command_number)
{

    if (command_number <= counter) {
        LOG("COMMAND NUM <= COUNTER\n");

        int i = command_number % max_len - 1;

        LOG("LOOKING FOR COMMAND W/ NUM:\t%d\n"
            "FOUND:\n"
                "\t->COMMAND_NUM:\t%d\n"
                "\t->CONTENT:\t%s\n",
            command_nums[i], lines[i]);

        return lines[i];
    }
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    return NULL;
}

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    return 0;
}

int circ_array_go_back_by(int i, int n)
{
    return abs((100 + i - n) % max_len);

}

int circ_array_go_forward_by(int i, int n)
{
    return abs((100 + i + n) % max_len);

}