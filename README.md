## lsh / LSH
by Lei Lin

### Features:
- Forks and executes commands
- Parses multiple commands on one line
- Redirects using >, <
- Pipes using |
- COLORS
- "cd -" can be used to return to the previous working directory

---

### Attempted:
- The following did not end up working, but have been left in the code, commented out.
    - Initially did not utilize execvp and instead  coded all of the functions, this took a lot of time and was eventually abandoned
    - show DESKTOP NAME in pwd
---

### Bugs:
- ";" used for multiple commands must be spaced appropriately with at least 1 space on both sides


---

### Files & Function Headers:
#### shell.c
* Contain all of the functions for this shell
  ```
  /*======== void sh_init() ==========
  Inputs:
  Returns:

  Performs one time functions like printing the welcome message and initiaties the shell.

  ====================*/

  /*======== void sh_loop() ==========
  Inputs:
  Returns:

  Loop that allows the shell to continue instead of exiting.
  ====================*/


  /*======== void print_cwd() ==========
  Inputs:
  Returns:

  Prints the current working directory in nice colors.
  ====================*/

  /*======== char *sh_read_line() ==========
  Inputs:
  Returns: char* that is equivalent to the parsed input

  Parses stdin and returns a string
  ====================*/

    /*======== char *fix_command() ==========
  Inputs: char *line_input
  Returns: char* with extra spaces removed

  Removes all cases in the string where a space appears more than once at a time.
  Also removes any leading and trailing spaces.
  ====================*/

  /*======== run_command() ==========
  Inputs: char *command_input
  Returns: int to show if the shell should continue

  Takes in a command line and runs the corressponding program.
  ====================*/

  /*======== run_args() ==========
  Inputs: char **args
  Returns: int to show if the shell should continue

  Takes in an array of arguments and runs execvp and fork where necessary.
  ====================*/

  /*======== piping() ==========
  Inputs: char **args
  Returns:

  Takes in an array of arguments and calls run_args on it. Additionally, it opens a pipe
  to be used for the following command.
  ====================*/

  /*======== redirin() ==========
  Inputs: char *fd
  Returns:

  Takes in a character string representing the name of the file descriptor we want to redirect from.
  ====================*/

  /*======== redirout() ==========
  Inputs: char *fd
  Returns:

  Takes in a character string representing the name of the file descriptor we want to redirect out to.
  ====================*/

  /*======== redirapp() ==========
  Inputs: char *fd
  Returns:

  Takes in a character string representing the name of the file descriptor we want to redirect out and append to.
  ====================*/

  /*======== sh_exit() ==========
  Inputs:
  Returns:

  prints "lsh exited successfully" and returns 0 to be used to set the status to 0.
  ====================*/

  /*======== sh_cd() ==========
  Inputs: char **input_args
  Returns:

  Takes in the string array representing the command cd and its arguments. It changes the current working directory
  to the one inputted in its argument. In addition, you can also use "-" as an argument to return to the previous
  working directory.
  ====================*/
  ```

### main.c
* Acts as the driver for the shell
