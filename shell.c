#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void sh_init() {
  printf("Welcome to lsh \n");
  char *user = getenv("USER");
  printf("USER is: @%s\n", user);
}

void sh_loop() {
  char *line;
  char **args;
  int status = 1;

  while(status) {
    print_cwd();
    line = sh_read_line();
    args = sh_parse_line(line);
    status = sh_run(args);
  }
}

void print_cwd() {
  char cwd[256];
  char *user = getenv("USER");
  getcwd(cwd, sizeof(cwd));

  printf(ANSI_COLOR_GREEN"%s:", user);
  printf(ANSI_COLOR_BLUE"%s$ ",cwd );
  printf(ANSI_COLOR_RESET);
}

char *sh_read_line() {
  char buffer[512];
  char *line = malloc(sizeof(buffer));
  fgets(buffer, sizeof(buffer), stdin);
  strncpy(line, buffer, strlen(buffer) - 1);
  return line;
}

char **sh_parse_line(char *input) {
  int n = strlen(input);
  char **args = malloc(8 * sizeof(char *));
  char *p = input;
  int i = 0;
  args[i]= strsep(&p," ");
  while (args[i]) {
    i++;
    args[i] = strsep(&p," ");
  }
  return args;
}

char *sh_cmd_names[] = {
  "exit"
};

int (*sh_cmd[]) (char **) = {
  &sh_exit
};

int sh_run(char **input_args) {
  int n = 0;
  for (n = 0; n < sizeof(sh_cmd_names); n++) {
    if (!(strcmp(input_args[0], sh_cmd_names[n]))) {
      return (*sh_cmd[n])(input_args);
    }
  }
}

int sh_exit() {
  printf("lsh exited successfully\n");
  return 0;
}
