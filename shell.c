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
  "cd",
  "ls",
  "pwd",
  "echo",
  "ps",
  "wc"
};

int (*sh_cmd[]) (char **) = {
  &sh_exit,
  &sh_cd,
  &sh_ls,
  &sh_pwd,
  &sh_echo,
  &sh_ps,
  &sh_wc
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
  int n;
  int num_cmd = sizeof(sh_cmd_names) / sizeof(sh_cmd_names[0]);

  for (n = 0; n < num_cmd; n++) {
    if (!(strcmp(input_args[0], sh_cmd_names[n]))) {
      if (n == 0 || n == 1) {
        return (*sh_cmd[n])(input_args);
      } else {
        int f = fork();

        printf("%d\n", f);

        if (f == 0) {
          (*sh_cmd[n])(input_args);
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
  }

  int f = fork();
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

  return 1;

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
              if (entry->d_type == DT_REG) {
                printf(ANSI_COLOR_GREEN"%20s ",entry->d_name);
              }
              if (entry->d_type == DT_DIR) {
                printf(ANSI_COLOR_BLUE"%20s ",entry->d_name);
              }
              printf(ANSI_COLOR_YELLOW"%s",ctime(&(file.st_mtime))) ;
          } else {
            if (entry->d_type == DT_REG) {
              printf(ANSI_COLOR_GREEN"%s\n", entry->d_name);
            }
            if (entry->d_type == DT_DIR) {
              printf(ANSI_COLOR_BLUE"%s\n", entry->d_name);
            }
          }
        } else if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            if (arg_l) {
                printf(ANSI_COLOR_RESET"%8d %8d %10ld ", file.st_uid, file.st_gid, file.st_size);
                if (entry->d_type == DT_REG) {
                  printf(ANSI_COLOR_GREEN"%20s ",entry->d_name);
                }
                if (entry->d_type == DT_DIR) {
                  printf(ANSI_COLOR_BLUE"%20s ",entry->d_name);
                }
                printf(ANSI_COLOR_YELLOW"%s",ctime(&(file.st_mtime))) ;
            } else {
              if (entry->d_type == DT_REG) {
                printf(ANSI_COLOR_GREEN"%s\n", entry->d_name);
              }
              if (entry->d_type == DT_DIR) {
                printf(ANSI_COLOR_BLUE"%s\n", entry->d_name);
              }
            }
        }
        entry = readdir(directory);
        printf(ANSI_COLOR_RESET);
    }
    closedir(directory);

  return 0;
}

int sh_pwd() {
  printf(ANSI_COLOR_RESET"%s\n", cwd);
  return 0;
}

int sh_echo(char **input_args) {
  int n = 1;
  while (input_args[n]) {
    printf("%s", input_args[n]);
    if (input_args[n+1]) {
      printf(" ");
    }
    n++;
  }
  printf("\n");
  return 0;
}

int sh_ps() {
  pid_t pid = getpid();
  pid_t ppid = getppid();
  printf("  PID CMD\n");
  printf("%5d %-6s\n",pid, "lsh");
  printf("%5d %-6s\n",pid, "ps");
}

int sh_wc(char **input_args) {
  int c;
  int w;
  int l;
  char *buffer;

  if (input_args[1]) {
    int size;

    int file = open(input_args[1], O_RDONLY);
    if (file == -1) {
      printf("wc: %s: %s\n", input_args[1], strerror(errno));
      return 0;
    }

    size = lseek(file, 0, SEEK_END);
    lseek(file, 0, SEEK_SET);

    buffer = calloc(size + 1, sizeof(char));
    int d = read(file, buffer, size);
    close(file);
  } else {
    buffer = sh_read_line();
  }

  int i;
  for (i = 0; buffer[i] != '\0'; i++) {
    if (buffer[i]) {
      c++;
    }
    if (buffer[i] == ' ' && buffer[i + 1] != ' ') {
      w++;
    }
    if (buffer[i] == '\n') {
      l++;
    }
  }
  free(buffer);
  printf("%d\t%d\t%d", l, w, c);
  if (input_args[1]) {
    printf("\t%s\n", input_args[1]);
  } else {
    printf("\n");
  }

}
