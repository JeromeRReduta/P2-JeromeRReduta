#include <stddef.h>
#include <stdlib.h>

#include "history.h"
#include "logger.h"
#include "string.h"
#include "util.h"

static int command_nums[100] = {0};
static char* lines[100] = {0};
static int counter = 0;
static int max_len;
static int current_len;


static int tail_index;


/* Func prototypes */


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

unsigned int hist_last_cnum(void)
{

    return counter;
}


