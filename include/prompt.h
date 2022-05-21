#ifndef __PROMPT_H
#define __PROMPT_H

#include "shell_consts.h"

#include <pwd.h>    /*  getpwuid                */
#include <stdlib.h> /*  getenv                  */
#include <stdio.h>  /*  fprintf, perror         */
#include <string.h> /*  strlen, strstr          */
#include <unistd.h> /*  geteuid, gethostname    */ 

/*
*   Retrieves the shell command prompt, which contains the login name, host name, and the current
*   working directory. Stores the information in a buffer.
*
*   @param  buf     char buffer.
*   @param  size    buffer size.
*
*   @return 0 on success. 1 on failure or if size is too small.
*/
int getCommandPrompt(char *, size_t);

#endif
