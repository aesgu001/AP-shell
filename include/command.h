#ifndef __COMMAND_H
#define __COMMAND_H

#include "shell_consts.h"

/*  A single command process.   */
typedef struct command
{
    struct command *next;           /*  Next command in pipeline.   */
    char **argv;                    /*  Argument list.              */
    const char *inTok, *outTok;     /*  I/O redirector.             */
    char *in, *out;                 /*  File descriptor.            */
    int status;                     /*  Command status.             */
} command;

#endif
