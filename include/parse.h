#ifndef __PARSE_H
#define __PARSE_H

#include "command.h"
#include "shell_consts.h"

#include <ctype.h>  /*  isalnum         */
#include <stdio.h>  /*  NULL, fprintf   */
#include <string.h> /*  strcmp, strsep  */

/*
*   Parses a command string, generating the command line argument.
*
*   @param  cmdStr  command string.
*   @param  argv    command line argument.
*   @param  cmd     command object.
*
*   @return 0 on success. 1 on failure.
*/
int parseCommand(char *, char *[], command *);

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
