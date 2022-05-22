#ifndef __SHELL_CONSTS_H
#define __SHELL_CONSTS_H

#define _ARG_MAX        64
#define _COMMAND_MAX    64
#define _HOST_NAME_MAX  64
#define _INPUT_MAX      1024
#define _PATH_MAX       1024
#define _PROMPT_MAX     1024

#define _CMD_EXIT       "exit"
#define _CMD_CD         "cd"

#define _ENV_HOME       "HOME"
#define _ENV_OLDPWD     "OLDPWD"
#define _ENV_PWD        "PWD"

#define _TOK_AND        "&&"
#define _TOK_COMMENT    "#"
#define _TOK_EMPTY      ""
/*  #define _TOK_IN         "<"     */
#define _TOK_OR         "||"
/*  #define _TOK_OUT        ">"     */
/*  #define _TOK_OUTAPPEND  ">>"    */
#define _TOK_SEMICOLON  ";"

#endif
