#ifndef AP_SHELL_H
#define AP_SHELL_H

#include <bits/local_lim.h> /*  HOST_NAME_MAX   */
#include <pwd.h>            /*  getpwuid    */
#include <stdio.h>          /*  fgets, printf, stdin    */
#include <stdlib.h>         /*  exit, getenv, setenv    */
#include <string.h>         /*  strcmp, strstr  */
#include <sys/wait.h>       /*  waitpid */
#include <unistd.h>         /*  chdir, fork, getcwd, geteuid, gethostname   */

#define AP_ARG_MAX  64
#define AP_CMD_LEN  1024

#define AP_CMD_CD   "cd"
#define AP_CMD_EXIT "exit"

#define AP_ENV_HOME     "HOME"
#define AP_ENV_OLDPWD   "OLDPWD"
#define AP_ENV_PWD      "PWD"

/*
*   Writes the shell's command prompt to standard output.
*   
*   @return None.
*/
void displayPrompt();

/*
*   Generates an argument list from the command line input.
*
*   @param  line    command line input.
*   @param  argv    argument list.
*
*   @return None.
*/
void parseLine(char *, char **);

/*
*   Changes the current working directory.
*
*   @param  path    path to new directory.
*   
*   @return 0 on success, 1 on error.
*/
int executecd(const char *);

/*
*   Clones a child process to execute the command line argument, and stores that process's
*   exit status.
*
*   @param  argv        command line argument.
*   @param  exitStatus  child process's exit status.
*
*   @return None.
*/
void executeCommand(char *const[], int *);

#endif
