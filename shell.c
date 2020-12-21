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

void sh_init() {
  printf("Welcome to lsh \n");
  char *user = getenv("USER");
  getcwd(pwd, sizeof(pwd));
  getcwd(cwd, sizeof(cwd));
  printf("USER is: @%s\n", user);
}

void sh_loop() {
  char *line;
  char *fixedline;
  int status = 1;

  while(status) {
    print_cwd();
    line = sh_read_line();
    fixedline = fix_command(line);
    status = run_command(fixedline);
    free(line);
    free(fixedline);
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

char *fix_command(char *line_input) {
  char *new_line = malloc(sizeof(line_input) * sizeof(char));
  int i = 0;
  int j = 0;
  int wantspace = 0;

  while (line_input[i]) {
    if (line_input[i] == ' ') {
        if (wantspace) {
          new_line[j] = line_input[i];
          j++;
          i++;
          wantspace = !wantspace;
        } else {
          i++;
        }
    } else {
      if (!wantspace) {
        wantspace = !wantspace;
      }
        new_line[j] = line_input[i];
        j++;
        i++;
    }

  }

  if (new_line[j - 1] == ' ') {
    new_line[j - 1] = '\0';
  }

  return new_line;
}

int run_command(char *command_input) {
  int backupstdin = dup(0);
  int backupstdout = dup(1);
  if (!command_input) {
    return 1;
  }

  char **args = malloc(64 * sizeof(char*));
  char **arg_to_run = malloc(16 * sizeof(char*));
  // MAX NUMBER OF ARGS: 64

  char *token;
  char *p = command_input;
  int i = 0;

  char *arg = strsep(&p, " ");
  while (arg) {
      if (!strcmp(arg, "<")) {
          redirin(strsep(&p, " "));
      } else if (!strcmp(arg, ">>")) {
        redirapp(strsep(&p, " "));
      } else if (!strcmp(arg, ">")) {
        redirout(strsep(&p, " "));
      } else if(!strcmp(arg, "|")) {
        args[i] = NULL;
        piping(args);
        i = 0;
      } else if (!strcmp(arg, ";")) {
        args[i] = NULL;
        run_args(args);
        i = 0;
      } else{
          args[i] = arg;
          i++;
      }
      arg = strsep(&p, " ");
  }
  args[i] = NULL;

  int k = run_args(args);
  dup2(backupstdout, 0);
  dup2(backupstdin, 1);

  return k;
}

int run_args(char **args) {
  if (!strcmp(args[0], "exit")) {
    return sh_exit();
  } else if (!strcmp(args[0], "cd")) {
    return sh_cd(args);
  } else {
    int f = fork();
    if (f) {
      wait(NULL);
      return 1;
    } else {
      if (execvp(args[0], args) == -1) {
          printf("%s: %s\n",args[0], strerror(errno));
      }
      exit(0);
    }
  }
}

void piping(char **args) {
  int backupstdin = dup(0);
  int backupstdout = dup(1);

  int fd[2];
  pipe(fd);

  dup2(fd[1], 1);
  close(fd[1]);

  run_args(args);

  dup2(backupstdout, 0);
  dup2(backupstdin, 1);

  dup2(fd[0], 0);
  close(fd[0]);
}

void redirin(char *fd) {
  int in = open(fd, O_RDONLY);
  dup2(in, 0);
  close(in);
}

void redirout(char *fd) {
  int out = open(fd, O_WRONLY | O_TRUNC | O_CREAT, 0600);
  dup2(out, 1);
  close(out);
}

void redirapp(char *fd) {
  int out = open(fd, O_WRONLY | O_APPEND | O_CREAT, 0600);
  dup2(out, 1);
  close(out);
}

int sh_exit() {
  printf("lsh exited successfully\n");
  return 0;
}

int sh_cd(char **input_args){
  char dir_name[256];

  if (input_args[1] == NULL) {
    return 1;
  }

  if (!strcmp(input_args[1], "-")) {
    strcpy(dir_name, pwd);
    printf("%s\n", pwd);
  } else {
    strcpy(dir_name, input_args[1]);
  }

  getcwd(pwd, sizeof(pwd));

  int directory = chdir(dir_name);
  if (directory) {
    printf("cd: %s: %s\n", dir_name, strerror(errno));
  }
  getcwd(cwd, sizeof(cwd));

  return 1;
}
