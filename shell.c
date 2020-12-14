#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

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
  char *user = getenv("USER");
  getcwd(cwd, sizeof(cwd));

  printf(ANSI_COLOR_GREEN"%s:", user);
  printf(ANSI_COLOR_BLUE"%s$ ", cwd);
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
  "exit",
  "ls",
  "cd"
};

int (*sh_cmd[]) (char **) = {
  &sh_exit,
  &sh_ls,
  &sh_cd
};

int sh_run(char **input_args) {
  int n;
  for (n = 0; n < sizeof(sh_cmd_names); n++) {
    if (!(strcmp(input_args[0], sh_cmd_names[n]))) {
      return (*sh_cmd[n])(input_args);
    }
  }
  printf("%s: command not found", input_args[0]);
  return 1;
}

int sh_exit() {
  printf("lsh exited successfully\n");
  return 0;
}

int option_switch(char **input_args, char *arg) {
  int n = 0;
  while (input_args[n]) {
    if (!strcmp(input_args[n], arg)) {
      return 1;
    }
    n++;
  }
  return 0;
}

int sh_ls(char **input_args) {
  DIR *directory;
  char *dir_name;
  int size = 0;

  int arg_l = option_switch(input_args, "-l");
  int arg_a = option_switch(input_args, "-a");

  int n = 0;
  while (input_args[n] != NULL) {
    n++;
  }

  if ((strcmp(input_args[n - 1], "-l")) && (strcmp(input_args[n - 1], "-a")) && (strcmp(input_args[n - 1], "ls"))) {
    dir_name = input_args[n - 1];
  } else {
    dir_name = ".";
  }

  directory = opendir(dir_name);

    if (!directory) {
      printf("ls: cannot access'%s': %s\n",dir_name , strerror(errno));
      return 1;
    }

    struct dirent *entry;
    struct stat file;
    entry = readdir(directory);

    if (!arg_a) {
      size = -1024;
    }

    while (entry)
    {
        stat(entry->d_name, &file);
        size += file.st_size;
        entry = readdir(directory);
    }

    rewinddir(directory);
    entry = readdir(directory);

    if (arg_l) {
        printf("total: %d\n", size);
        printf("%8s %8s %10s %20s %24s\n", "UID", "GID", "SIZE", "NAME", "LAST MODIFIED");
    }

    while (entry)
    {
        stat(entry->d_name, &file);

        if (arg_a) {
          if (arg_l) {
              printf(ANSI_COLOR_RESET"%8d %8d %10ld ", file.st_uid, file.st_gid, file.st_size);
              printf(ANSI_COLOR_GREEN"%20s ",entry->d_name);
              printf(ANSI_COLOR_YELLOW"%s",ctime(&(file.st_mtime))) ;
          } else {
            printf(ANSI_COLOR_GREEN"%s\n", entry->d_name);
          }
        } else if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            if (arg_l) {
                printf(ANSI_COLOR_RESET"%8d %8d %10ld ", file.st_uid, file.st_gid, file.st_size);
                printf(ANSI_COLOR_GREEN"%20s ",entry->d_name);
                printf(ANSI_COLOR_YELLOW"%s",ctime(&(file.st_mtime))) ;
            } else {
              printf(ANSI_COLOR_GREEN"%s\n", entry->d_name);
            }
        }
        entry = readdir(directory);
        printf(ANSI_COLOR_RESET);
    }
  return 1;
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
