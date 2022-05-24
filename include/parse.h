#ifndef __PARSE_H
#define __PARSE_H

#include "command.h"
#include "shell_consts.h"

#include <stdio.h>  /*  NULL, fprintf   */
#include <string.h> /*  strcmp, strsep  */

/*
*   Parses a command string, generating the command line argument.
*
*   @param  cmd     command string.
*   @param  argv    command line argument.
*   @param  argvC   command argument list.
*
*   @return None.
*/
void parseCommand(char *, char *[], char *[]);

/*
*   Parses the command line input, extracting commands and connector tokens.
*
*   @param  input       command line input.
*   @param  argv        command line argument.
*   @param  commands    command list.
*   @param  toks        connector tokens.
*
*   @return 0 on success. 1 on failure.
*/
int parseInput(char *, char *[], command *, const char *[]);

#endif
