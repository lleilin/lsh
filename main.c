#include "shell.h"

#include <stdio.h>

int main(int argc, char **argv) {
  sh_init();
  sh_loop();
}
