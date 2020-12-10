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
  int status;

  while(status) {
    print_cwd();
    line = read_line();
    args = split_line();
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
