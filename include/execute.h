#ifndef __EXECUTE_H
#define __EXECUTE_H

#include "command.h"
#include "shell_consts.h"

#include <stdio.h>      /*  fprintf, perror, printf, stderr, stdout             */
#include <stdlib.h>     /*  EXIT_FAILURE, EXIT_SUCCESS, exit, getenv, setenv    */
#include <string.h>     /*  strcmp                                              */
#include <sys/wait.h>   /*  waitpid                                             */
#include <unistd.h>     /*  chdir, fork, getcwd                                 */

/*
*   Changes the current working directory to the directory specified in path.
*
*   @param  path    path to new directory.
*   
*   @return 0 on success, 1 on error.
*/
int executeChangeDirectory(const char *);

/*
*   Clones a child process to execute the command line argument, and retrieves that child process's
*   exit status.
*
*   @param  cmd         executing command.
*
*   @return None.
*/
void executeCommand(command *);

/*
*   Executes each command in the list that meets connector and exit status conditions.
* 
*   @param  commands    command list.
*   @param  toks        connector tokens.
*
*   @return 0 on success. 1 if exit called.
*/
int executeInput(command *, const char *[]);

#endif
