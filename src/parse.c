#include "../include/parse.h"

void parseCommand(char *cmd, char *argv[])
{
    while (*cmd != '\0')
    {
        /*  Replace newlines and whitespaces '\0'   */
        while (*cmd == ' ' || *cmd == '\n')
        {
            *cmd++ = '\0';
        }

        if (*cmd == '\0')
        {
            /*  Exit loop   */
            break;
        }

        /*  Store argument position     */
        *argv++ = cmd++;
        while (*cmd != '\0' && *cmd != '\n' && *cmd != ' ')
        {
            ++cmd;
        }
    }

    /*  Set end of argument list to NULL    */
    *argv = NULL;
}

int parseInput(char *input, command *commands, const char *toks[])
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
            else if (strstr(tokP, _TOK_OR) == tokP)
            {
                tok = _TOK_OR;
                break;
            }   
        }

        /*  Parse command string    */
        parseCommand(strsep(&input, tok), commands->argv);
        if (strcmp(tok, _TOK_EMPTY) != 0)
        {
            if (!(*commands->argv))
            {
                /*  Syntax error    */
                fprintf(stderr, "no command found before token `%s'\n", tok);
                return 1;
            }

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

        if (*commands->argv)
        {
            /*  Initialize command  */
            commands++->status = -1;
            prevTok = tok;
        }
    }
    /*  Check if input ends with connector AND or OR   */
    if (strcmp(prevTok, _TOK_AND) == 0 || strcmp(prevTok, _TOK_OR) == 0)
    {
        fprintf(stderr, "no command found after token `%s'\n", prevTok);
        return 1;
    }

    /*  Set end of command list to NULL     */
    *commands->argv = NULL;
    /*  Set end of connector list to NULL   */
    *toks = NULL;

    return 0;
}
