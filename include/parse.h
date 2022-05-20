#ifndef __PARSE_H
#define __PARSE_H

#include <stdio.h>  /*  NULL, fprintf   */
#include <string.h> /*  strcmp, strsep  */

#define _MAX_INPUT  1024
#define _ARG_MAX    64

#define _SEMICOLON  ";"
#define _AND        "&&"
#define _OR         "||"

/*
*   Parses a command string, generating arguments to be executed.
*
*   @param  cmd     command string.
*   @param  argc    argument count.
*   @param  argv    command line argument.
*
*   @return 0 on success. 1 if argc exceeds max number of arguments.
*/
int parseCommand(char *, int *, char *[]);

/*
*   Parses the command line input, extracting command strings and connectors.
*
*   @param  input   command line input.
*   @param  argc    argument count.
*   @param  argv    command line argument.
*   @param  toks    command connectors.
*
*   @return 0 on success. 1 on failure.
*/
int parseInput(char *, int *, char *[], const char *[]);

#endif
