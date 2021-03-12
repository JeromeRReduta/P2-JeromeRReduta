#include <stdio.h>
#include <readline/readline.h>
#include <locale.h>
#include <stdbool.h>
#include <pwd.h>

#include "history.h"
#include "logger.h"
#include "ui.h"



/* Function prototypes */
static int readline_init(void);

char *get_prompt_str();
void add_login(char *prompt_str_copy);
void add_hostname(char *prompt_str_copy);
void add_current_dir(char *prompt_str_copy);




static char prompt_str[80] = "--[enter a command]--> ";
char* home_dir = "/home/jrreduta";

static int home_dir_len = strlen("/home/jrreduta");
static bool scripting = false;


char *get_prompt_str(char *prompt_str)
{
    char prompt_str_copy[80] = "[";

    add_login(prompt_str_copy);
    add_hostname(prompt_str_copy);
    add_current_dir(prompt_str_copy);

    strcat(prompt_str_copy, "]");
    strcpy(prompt_str, prompt_str_copy);



    return prompt_str;
}

void add_login(char *prompt_str_copy)
{
    strcat(prompt_str_copy, getlogin());
    strcat(prompt_str_copy, "@");
}

void add_hostname(char *prompt_str_copy)
{
    char hostname_buf[80];
    gethostname(hostname_buf, 80);
    strcat(prompt_str_copy, hostname_buf);
    strcat(prompt_str_copy, ":\t");

}

void add_current_dir(char *prompt_str_copy)
{
    char cwd_buf[80];
    getcwd(cwd_buf, 80);
    char *home_dir_head = strstr(cwd_buf, home_dir);

    if (home_dir_head != NULL && home_dir_head - cwd_buf == 0) {
        LOG("FOUND HOME_DIR:\t%s\n", home_dir_head);
        // Replace "/home/jrreduta" w/ "~"
        strcat(prompt_str_copy, "~");
        // Add the rest
        strcat(prompt_str_copy, cwd_buf + home_dir_len);
    }
    else {
        strcat(prompt_str_copy, cwd_buf);
    }

}


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
        scripting = true;
    }

    rl_startup_hook = readline_init;
}

char *prompt_line(void) {
    get_prompt_str(prompt_str);
    return prompt_str;
}

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

char *scriptline(char **line_ptr, size_t *line_sz_ptr)
{

    // Note: Have to free mem from this later
    // Note: getline keeps \n char 
    size_t read_sz = getline(line_ptr, line_sz_ptr, stdin);

    if (read_sz == -1) {
        perror("getline");
        return NULL;
    }

    *line_ptr[read_sz-1] = '\0';

    return *line_ptr;
}



int readline_init(void)
{
    rl_bind_keyseq("\\e[A", key_up);
    rl_bind_keyseq("\\e[B", key_down);
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    rl_attempted_completion_function = command_completion;
    return 0;
}

int key_up(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'up' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: reverse history search

    return 0;
}

int key_down(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'down' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: forward history search

    return 0;
}

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
char *command_generator(const char *text, int state)
{
    // TODO: find potential matching completions for 'text.' If you need to
    // initialize any data structures, state will be set to '0' the first time
    // this function is called. You will likely need to maintain static/global
    // variables to track where you are in the search so that you don't start
    // over from the beginning.

    return NULL;
}
