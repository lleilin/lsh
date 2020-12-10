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
    args = sh_split_line(line);
    int i;
    for (i = 0; i < 3; i++) {
      printf("%s\t", args[i]);
    }
  }
}

void print_cwd() {
  char cwd[256];
  char *user = getenv("USER");
  getcwd(cwd, sizeof(cwd));

  printf(ANSI_COLOR_GREEN"%s:", user);
  printf(ANSI_COLOR_BLUE"%s$",cwd );
  printf(ANSI_COLOR_RESET);
}

char *sh_read_line() {
  char buffer[512];
  char *line = malloc(sizeof(buffer));
  fgets(buffer, sizeof(buffer), stdin);
  strncpy(line, buffer, strlen(buffer) - 1);
  return line;
}

char **sh_split_args(char *input) {
  int n = strlen(input);
  char **args = malloc(sizeof(char*));
  char *p = input;
  int i = 0;
  args[i] = strsep(&p," ");

  while (args[i]) {
    i++;
    args[i] = strsep(&p," ");
  }

  return args;
}
