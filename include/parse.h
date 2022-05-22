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
*
*   @return None.
*/
void parseCommand(char *, char *[]);

/*
*   Parses the command line input, extracting commands and connector tokens.
*
*   @param  input       command line input.
*   @param  commands    command list.
*   @param  toks        connector tokens.
*
*   @return 0 on success. 1 on failure.
*/
int parseInput(char *, command *, const char *[]);

#endif
