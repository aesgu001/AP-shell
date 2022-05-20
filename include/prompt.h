#ifndef __PROMPT_H
#define __PROMPT_H

#include <pwd.h>    /*  getpwuid                */
#include <stdlib.h> /*  getenv                  */
#include <stdio.h>  /*  fprintf, perror         */
#include <string.h> /*  strlen, strstr          */
#include <unistd.h> /*  geteuid, gethostname    */ 

#define _HOST_NAME_MAX  64
#define _PATH_MAX       1024
#define _PROMPT_MAX     1024

#define _HOME_ENV       "HOME"
#define _PWD_ENV        "PWD"

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