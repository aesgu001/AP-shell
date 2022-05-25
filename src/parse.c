#include "../include/parse.h"

int parseCommand(char *cmdStr, char *argv[], command *cmd)
{
    /*  Redirector token.           */
    const char *ioTok = _TOK_EMPTY;
    /*  Command argument parser.    */
    char **argvC = cmd->argv;

    while (*cmdStr != '\0')
    {
        /*  Replace newlines and whitespaces with '\0'  */
        while (*cmdStr == ' ' || *cmdStr == '\n')
        {
            *cmdStr++ = '\0';
        }

        if (*cmdStr == '\0')
        {
            /*  Exit loop   */
            break;
        }

        /*  Get redirection token   */
        if (strstr(cmdStr, _TOK_IN) == cmdStr)
        {
            ioTok = _TOK_IN;
            *cmdStr++ = '\0';
        }
        else if (strstr(cmdStr, _TOK_OUT) == cmdStr)
        {
            if (strstr(cmdStr, _TOK_OUTAPPEND) == cmdStr)
            {
                ioTok = _TOK_OUTAPPEND;
                *cmdStr++ = '\0';
                *cmdStr++ = '\0';
            }
            else
            {
                ioTok = _TOK_OUT;
                *cmdStr++ = '\0';
            }
        }
        else
        {
            ioTok = _TOK_EMPTY;
        }

        if (strcmp(ioTok, _TOK_EMPTY) == 0)
        {
            /*  Store argument position     */
            *argvC++ = *argv++ = cmdStr++;
            while (*cmdStr != '\0' && *cmdStr != '\n' && *cmdStr != ' ' && *cmdStr != '>' &&
                *cmdStr != '<')
            {
                ++cmdStr;
            }
        }
        else
        {
            /*  Replace more newlines and whitespaces   */
            while (*cmdStr == ' ' || *cmdStr == '\n')
            {
                *cmdStr++ = '\0';
            }

            /*  Syntax error    */
            if (*cmdStr == '\0')
            {
                fprintf(stderr, "no file found after token `%s'\n", ioTok);
                return 1;
            }
            /*  Store redirection token and file name     */
            else if (isalnum(*cmdStr))
            {
                if (strcmp(ioTok, _TOK_IN) == 0)
                {
                    cmd->in = cmdStr++;
                    cmd->inTok = ioTok;
                }
                else
                {
                    cmd->out = cmdStr++;
                    cmd->outTok = ioTok;
                }

                while (*cmdStr != '\0' && *cmdStr != '\n' && *cmdStr != ' ' && *cmdStr != '>' &&
                    *cmdStr != '<')
                {
                    ++cmdStr;
                }
            }
        }
    }

    /*  Set end of argument list to NULL    */
    *argvC = *argv = NULL;

    return 0;
}

int parseInput(char *input, char *argv[], command *commands, const char *toks[])
{
    /*  Connector token.                */
    const char *tok = _TOK_EMPTY;
    /*  Previous connector token.       */
    const char *prevTok = _TOK_EMPTY;
    /*  Connector parser.               */
    const char *tokP = NULL;

    /*  Remove input preceded by '#'    */
    input = strsep(&input, _TOK_COMMENT);
    while (input && strcmp(input, _TOK_EMPTY) != 0)
    {
        /*  Find nearest connector token.   */
        tok = _TOK_EMPTY;
        for (tokP = input; *tokP != '\0'; ++tokP)
        {
            if (strstr(tokP, _TOK_SEMICOLON) == tokP)
            {
                tok = _TOK_SEMICOLON;
                break;
            }
            else if (strstr(tokP, _TOK_AND) == tokP)
            {
                tok = _TOK_AND;
                break;
            }
            else if (strstr(tokP, _TOK_PIPE) == tokP)
            {
                if (strstr(tokP, _TOK_OR) == tokP)
                {
                    tok = _TOK_OR;
                }
                else
                {
                    tok = _TOK_PIPE;
                }
                break;
            }   
        }

        /*  Initialize command  */
        commands->next = NULL;
        commands->argv = argv;
        commands->inTok = commands->outTok = NULL;
        commands->in = commands->out = NULL;
        commands->status = -1;

        /*  Parse command string    */
        if (parseCommand(strsep(&input, tok), argv, commands) == 1)
        {
            return 1;
        }
        else if (strcmp(tok, _TOK_EMPTY) != 0)
        {
            if (!(*argv))
            {
                /*  Syntax error    */
                fprintf(stderr, "no command found before token `%s'\n", tok);
                return 1;
            }
            else if (strcmp(tok, _TOK_PIPE) == 0)
            {
                /*  Add next command to current pipeline    */
                commands->next = commands + 1;
            }
            else
            {
                /*  Add connector token     */
                *toks++ = tok;

                if (strlen(tok) > 1)
                {
                    int i;
                    for (i = 0; i < strlen(tok) - 1; ++i)
                    {
                        /*  Remove trailing token char from input   */
                        *input++ = '\0';
                    }
                }
            }
        }

        if (*argv)
        {
            /*  Update commands, argv, and previous token   */
            ++commands;
            while (*argv++);
            prevTok = tok;
        }
    }
    /*  Check if input ends with connector AND or OR or PIPE   */
    if (strcmp(prevTok, _TOK_AND) == 0 || strcmp(prevTok, _TOK_OR) == 0 ||
        strcmp(prevTok, _TOK_PIPE) == 0)
    {
        fprintf(stderr, "no command found after token `%s'\n", prevTok);
        return 1;
    }

    /*  Set end of command list to NULL     */
    commands->argv = NULL;
    /*  Set end of connector list to NULL   */
    *toks = NULL;

    return 0;
}
