#include <stdio.h>
#include <readline/readline.h>
#include <locale.h>
#include <stdbool.h>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdbool.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"

/*
 *@file Functions for dealing with the user interfaec
 */


/* Function prototypes */
static int readline_init(void);
static int current_command_num = 0;

char *get_prompt_str();
void add_login(char *prompt_str_copy);
void add_hostname(char *prompt_str_copy);
void add_current_dir(char *prompt_str_copy);




static char prompt_str[80] = "--[enter a command]--> ";
char* home_dir = "/home/jrreduta";

static int home_dir_len = strlen("/home/jrreduta");
static bool scripting = false;

static DIR* dir_ptr = NULL;
static char* dir_name = NULL; // For debugging
static int current_arg_i = 0;
static int current_builtin_i = 0;
static bool checked_for_builtins = false;


/**
 * @brief      Constructs the prompt_str and returns it
 *
 * @param      prompt_str  Initial prompt_str buffer (this function overwrites prompt_str)
 *
 * @return     Constructed prompt_str
 */
char *get_prompt_str(char *prompt_str)
{
    char prompt_str_copy[80] = "You're li-shell-ing to KRASH at [";

    add_login(prompt_str_copy);
    add_hostname(prompt_str_copy);
    add_current_dir(prompt_str_copy);

    strcat(prompt_str_copy, "]");
    strcpy(prompt_str, prompt_str_copy);



    return prompt_str;
}

/**
 * @brief      Adds login info to prompt_str
 *
 * @param      prompt_str_copy  prompt_str buffer copy
 */
void add_login(char *prompt_str_copy)
{
    strcat(prompt_str_copy, getlogin());
    strcat(prompt_str_copy, "@");
}

/**
 * @brief      Adds hostname info to prompt_str
 *
 * @param      prompt_str_copy  prompt_str buffer copy
 */
void add_hostname(char *prompt_str_copy)
{
    char hostname_buf[80];
    gethostname(hostname_buf, 80);
    strcat(prompt_str_copy, hostname_buf);
    strcat(prompt_str_copy, ":\t");

}

/**
 * @brief      Adds current working directory name to prompt_str
 *
 * @param      prompt_str_copy  prompt_str buffer copy
 */
void add_current_dir(char *prompt_str_copy)
{
    char cwd_buf[80];
    getcwd(cwd_buf, 80);
    char *home_dir_head = strstr(cwd_buf, home_dir);

    if (home_dir_head != NULL && home_dir_head - cwd_buf == 0) {
        strcat(prompt_str_copy, "~");
        strcat(prompt_str_copy, cwd_buf + home_dir_len);
    }
    else {
        strcat(prompt_str_copy, cwd_buf);
    }
}

/**
 * @brief      Initializes UI
 */
void init_ui(void)
{
    LOGP("Initializing UI...\n");

    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");

    if (isatty(STDIN_FILENO)) {
        LOGP("stdin is a TTY -> interactive\n");
    }
    else {
        LOGP("Data is piped in on stdin -> script\n");

        char cwd_buf[80];
        getcwd(cwd_buf, 80);

        LOG("CWD:\t'%s'\n", cwd_buf);
        scripting = true;
    }

    rl_startup_hook = readline_init;
}

/**
 * @brief      Returns a constructed prompt_str
 *
 * @return     A constructed prompt_str
 */
char *prompt_line(void) {
    get_prompt_str(prompt_str);
    current_command_num = hist_last_cnum(); // Refreshes history tracking after every entry
    return prompt_str;
}

/**
 * @brief      Prompts a user in interactive mode for input (no prompt in script mode), and gets their input
 *
 * @return     { description_of_the_return_value }
 */
char *read_command(void)
{

    if (scripting) {
        char *line = NULL;
        size_t line_sz = 0;

        size_t read_sz = getline(&line, &line_sz, stdin);
        if (read_sz == -1) {
            perror("getline");
            return NULL;
        }

        return line;
       
        
    }
    else {
        return readline(prompt_line());
    }
}


/**
 * @brief      Initializes readline ilbrary
 *
 * @return     0 (success) for readline library
 */
int readline_init(void)
{
    rl_bind_keyseq("\\e[A", key_up);
    rl_bind_keyseq("\\e[B", key_down);
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    rl_attempted_completion_function = command_completion;
    return 0;
}

/**
 * @brief      When the user presses up, replaces user's input with the previous line.
 *
 * @param[in]  count  count
 * @param[in]  key    up key (for compliance with readline lib)
 *
 * @return     0 (success) for readline
 */
int key_up(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'up' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    LOG("CURRENT COMMAND_NUM:\t%d\n", current_command_num);

    int new_command_num = current_command_num-1 != 0 ? current_command_num - 1 : current_command_num;

    LOG("NEW COMMAND_NUM:\t%d\n", new_command_num);
    rl_replace_line(hist_search_cnum(new_command_num), 0);

    current_command_num = new_command_num;

    return 0;
}

/**
 * @brief      When the user presses down, replaces user's input with the next line in history (if the user was already at the most recent line, this func replaces input with a blank line instead)
 *
 * @param[in]  count  count
 * @param[in]  key    down key (for compliance with readline lib)
 *
 * @return     0 (success) for readline
 */
int key_down(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'down' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    if (current_command_num == hist_last_cnum()) {
        rl_replace_line(" ", 0);
    }
    else {
        current_command_num++;
        rl_replace_line(hist_search_cnum(current_command_num), 0);
    }


    return 0;
}

/**
 * @brief      Autocomplete driver function
 *
 * @param[in]  text   text to find matches against
 * @param[in]  start  unused by code
 * @param[in]  end    unused by code
 *
 * @return     { description_of_the_return_value }
 */
char **command_completion(const char *text, int start, int end)
{
    /* Tell readline that if we don't find a suitable completion, it should fall
     * back on its built-in filename completion. */
    rl_attempted_completion_over = 0;

    return rl_completion_matches(text, command_generator);
}

/**
 * This function is called repeatedly by the readline library to build a list of
 * possible completions. It returns one match per function call. Once there are
 * no more completions available, it returns NULL.
 */

/**
 * @brief       From Prof. Malensek: "This function is called repeatedly by the readline library to build a list of
 * possible completions. It returns one match per function call. Once there are
 * no more completions available, it returns NULL"
 *
 * @param[in]  text   text to match against
 * @param[in]  state  unused
 *
 * @return     the next string that starts w/ text, or NULL if no more strings
 * 
 * @note       Function searches in this order: executables in PATH, built-in commands (exit, jobs, history, cd), file-names in current working directory
 */
char *command_generator(const char *text, int state)
{


    LOGP("____________________START AUTO-COMPLETE____________________\n\n");


    LOGP("GETTING PATH...\n");

    char* path = getenv("PATH");
    if (path == NULL) {
        LOGP("WHY IS PATH NULL??");
        return NULL;
    }

    char *path_copy = strdup(path); // free later

    LOG("PATH_COPY:\t'%s'\n", path_copy);
    LOG("INPUT_STR:\t'%s'\n", text);
    LOG("PATH EMPTY? %d\tINPUT_STR EMPTY? %d\n", strlen(path_copy) == 0, strlen(text) == 0);


    int tokens = 0;
    char *next = path_copy;
    char *current;

    // 4096 b/c according to Prof. Malensek, POSIX standard requires
    // shells to accept 4096 args (1st being name)
    // Now compiles w/ posix standards!
    char *args[4096] = {(char*) 0};

    current = next_token(&next, ":");


    while (current != NULL) {

        LOG("PATH %02d: '%s'\n", tokens, current);

        args[tokens++] = current;
        current = next_token(&next, ":");
   
       
    }

    LOG("# of tokens:\t%d\n", tokens);
    for (int i = 0; i < tokens; i++) {
        LOG("arg[%d]:\t'%s'\n", i, args[i]);
    }



    if (!checked_for_builtins) {
        for (int i = current_arg_i; i < tokens; i++) {

            current_arg_i = i;
            LOG("CURRENT ARG I:\t%d\n", current_arg_i);
            if (dir_ptr == NULL) {

                LOGP("dir_ptr is NULL\n");
                dir_ptr = opendir(args[i]);
                dir_name = args[i];

            }

            LOG("CURRENT_ARG_I:\t%d\n", current_arg_i);

            if (dir_ptr == NULL) {
                continue;
            }

            LOG("DIR RN POINTING TO:\t'%s'\n", dir_name);
            // from readdir.c
            struct dirent *entry;

            while ((entry = readdir(dir_ptr)) != NULL) {

                
                char* name = entry->d_name; // Using common var for debugging
                 LOG("-> %s\n", name);
                if (starts_with(name, text)) {
                   


                    return strdup(name); // Note: to fix mem leak, just have static
                    // array of matches in this file, then have destroy() func that
                    // frees everything in array of matches
                }
            }
            LOGP("OUT OF ENTRIES\n");
            dir_ptr = NULL;
            dir_name = args[i];



        }
    }

    // Reset current_arg_i to 0

    LOG("STATS:\n"
        "\t->current_arg_i:\t%d\n"
        "\t->DIR PTR NULL?:\t%d\n"
        "\t->DIR POINTING TO:\t'%s'\n", current_arg_i, dir_ptr == NULL, dir_name);

    char* builtins[] = {"exit", "jobs", "history", "cd"};

    checked_for_builtins = true;
    LOG("CURRENT BUILTIN I:\t%d\n", current_builtin_i);

    for (int i = current_builtin_i; i < 4; i++) {
        current_builtin_i = i;
        if (starts_with(builtins[current_builtin_i], text)) {
            LOG("MATCHES:\t'%s'\n", builtins[current_builtin_i]);
            current_builtin_i++;




            return builtins[i];
        }


    }



    // Completely finished w/ searching - reset all static vars
    LOGP("DONE SEARCHING\n");
       

    dir_ptr = NULL;
    dir_name = NULL;
    current_arg_i = 0;
    current_builtin_i = 0;
    checked_for_builtins = false;

    LOGP("RETURNING NULL:\n");
    return NULL;
}
