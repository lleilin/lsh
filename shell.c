#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>



char pwd[256];
char cwd[256];

char *sh_cmd_names[] = {
  "exit",
  "cd"
};

int (*sh_cmd[]) (char **) = {
  &sh_exit,
  &sh_cd
};

void sh_init() {
  printf("Welcome to lsh \n");
  char *user = getenv("USER");
  getcwd(pwd, sizeof(pwd));
  getcwd(cwd, sizeof(cwd));
  printf("USER is: @%s\n", user);
}

void sh_loop() {
  char *line;
  char **cmds;
  int status = 1;

  while(status) {
    print_cwd();
    line = sh_read_line();
    cmds = sh_split_line(line);
    // args = sh_parse_line(cmds);
    status = sh_run_all(cmds);

    free(line);
    free(cmds);
    // free(args);

  }
}

void print_cwd() {
  char *user = getenv("USER");
  getcwd(cwd, sizeof(cwd));

  printf(ANSI_COLOR_GREEN"%s:", user);
  printf(ANSI_COLOR_RED"%s$ ", cwd);
  printf(ANSI_COLOR_RESET);

}

char *sh_read_line() {
  char buffer[512];
  char *line = malloc(sizeof(buffer));
  fgets(buffer, sizeof(buffer), stdin);
  strncpy(line, buffer, strlen(buffer) - 1);
  return line;
}

char **sh_split_line(char *input) {
  char **cmds = malloc(8 * sizeof(char*));
  char *p = input;
  int i = 0;

  cmds[i]=strsep(&p,";");
  while (cmds[i]) {
    i++;
    cmds[i] = strsep(&p,";");
  }

  return cmds;
}

char **sh_parse_line(char *input) {
  char **args = malloc(8 * sizeof(char*));
  char *p = input;
  int i = 0;
  args[i]=strsep(&p," ");
  while (args[i]) {
    if (args[i][0] != 0) {
      i++;
    }
    args[i] = strsep(&p," ");
  }

  return args;
}

int sh_run(char **input_args) {

  if (!(strcmp(input_args[0], "exit"))) {
    return sh_exit();
  } else if (!(strcmp(input_args[0], "cd"))) {
    return sh_cd(input_args);
  } else {

    int f = fork();
    printf("pid %d\n", f);
    if (f == 0) {
      execvp(input_args[0], input_args);
      exit(0);
    } else if (f < 0) {
      printf("%s: error forking", input_args[0]);
      return 0;
    } else {
      wait(NULL);
      return 1;
    }
  }
}

int sh_run_all(char **input_lines) {
  char **args;
  int i = 0;
  int status = 1;
  while (input_lines[i]) {
    args = sh_parse_line(input_lines[i]);
    status = sh_run(args);
    free(args);
    i++;
  }
  return status;
}

int sh_exit() {
  printf("lsh exited successfully\n");
  return 0;
}

int sh_cd(char **input_args){
  char *dir_name;

  if (input_args[1] == NULL) {
    return 1;
  }

  if (!strcmp(input_args[1], "-")) {
    strncpy(dir_name, pwd, sizeof(pwd));
    printf("%s\n", pwd);
  } else {
    dir_name = input_args[1];
  }

  getcwd(pwd, sizeof(pwd));

  int directory = chdir(dir_name);
  if (directory) {
    printf("cd: %s: %s\n", dir_name, strerror(errno));
  }
  getcwd(cwd, sizeof(cwd));

  return 1;
}
