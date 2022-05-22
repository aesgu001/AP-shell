#ifndef __COMMAND_H
#define __COMMAND_H

#include "shell_consts.h"

/*  A single command process.   */
typedef struct command
{
    /*  struct command *next;             Next command in pipeline.   */
    char *argv[_ARG_MAX];           /*  Argument list.              */
    /*  int in, out;                      I/O channel.                */
    /*  const char *inTok, *outTok;       I/O redirector.             */
    /*  const char *inFD, *outFD;         File descriptor.            */
    int status;                     /*  Exit status.                */
} command;

#endif
