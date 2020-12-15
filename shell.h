#ifndef SHELL_H
#define SHELL_H

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// char *sh_cmd_names[];
// int (*sh_cmd[]) (char **);

void sh_init();
void sh_loop();
void print_cwd();

char *sh_read_line();
char **sh_split_line(char *input);
char **sh_parse_line(char *input);
int sh_run(char **input_args);
int sh_run_all(char **input_lines);

int option_switch(char **input_args, char *arg);

int sh_exit();
int sh_cd(char **input_args);
int sh_ls(char **input_args);
int sh_pwd();
int sh_echo(char **input_args);

#endif
