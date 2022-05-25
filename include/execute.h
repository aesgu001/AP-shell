#ifndef __EXECUTE_H
#define __EXECUTE_H

#include "command.h"
#include "shell_consts.h"

#include <fcntl.h>
#include <stdio.h>      /*  fprintf, perror, printf, stderr, stdout             */
#include <stdlib.h>     /*  EXIT_FAILURE, EXIT_SUCCESS, exit, getenv, setenv    */
#include <string.h>     /*  strcmp                                              */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>   /*  waitpid                                             */
#include <unistd.h>     /*  chdir, fork, getcwd                                 */

/*  Opens files for reading/writing on standard input/output.
*
*   @param  in  in file descriptor.
*   @param  out out file descriptor.
*   @param  cmd command to be executed.
*
*   @return 0 on success. 1 on failure.
*/
int openCommandFiles(int *, int *, command *);

/*
*   Changes the current working directory to the directory specified in path.
*
*   @param  path    path to new directory.
*   
*   @return 0 on success, 1 on error.
*/
int executeChangeDirectory(const char *);

/*  Creates a pipeline to connect all processes such that one process can write to/read from another.
*
*   @param  commands    commands to execute.
*
*   @return 0 on success. 1 if at least one command failed.
*/
int executePiped(command *);

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
