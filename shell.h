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
char *fix_command(char *line_input);
char **splitline(char *input_line);
int run_command(char *command_input);
int run_args(char **args);
int run_all(char **commands);

void redirin(char *fd);
void redirout(char *fd);
void redirapp(char *fd);
void piping(char *args[]);

int sh_exit();
int sh_cd(char **input_args);

#endif
