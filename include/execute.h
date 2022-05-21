#ifndef __EXECUTE_H
#define __EXECUTE_H

#include "parse.h"

#include <stdio.h>      /*  perror                              */
#include <stdlib.h>     /*  EXIT_FAILURE, EXIT_SUCCESS, exit    */
#include <string.h>     /*  strcmp                              */
#include <sys/wait.h>   /*  waitpid                             */
#include <unistd.h>     /*  fork                                */

/*
*   Clones a child process to execute the command line argument, and retrieves that child process's
*   exit status.
*
*   @param  argv        command line argument.
*   @param  exitStatus  child process's exit status.
*
*   @return None.
*/
void executeCommand(char *const[], int *);

/*
*   Processes the command line input. Executes each command that meets connector and exit status
*   conditions.
* 
*   @param  argc    argument count.
*   @param  argv    command line argument.
*   @param  toks    command connectors.
*
*   @return 0 on success. 1 if exit called.
*/
int executeInput(int, char *[], const char *[]);

#endif
