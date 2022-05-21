#include "../include/parse.h"

int parseCommand(char *cmd, int *argc, char *argv[])
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

        /*  Check if argc exceeds args  */
        if (++(*argc) >= _ARG_MAX)
        {
            fprintf(stderr, "too many arguments\n");
            *argv = NULL;
            return 1;
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

    return 0;
}

int parseInput(char *input, int *argc, char *argv[], const char *toks[])
{
    /*  Command line argument parser.   */
    char **parArgv = argv;
    /*  Connector string.               */
    const char *tok = "";
    /*  Connector parser.               */
    char *parTok = NULL;

    /*  Remove input preceded by '#'    */
    input = strsep(&input, "#");
    /*  Set argument count to 0     */
    *argc = 0;

    while (input && strcmp(input, "") != 0)
    {
        /*  Get nearest connector   */
        tok = "";
        for (parTok = input; *parTok != '\0'; ++parTok)
        {
            if (strstr(parTok, _TOK_SC) == parTok)
            {
                tok = _TOK_SC;
                break;
            }
            else if (strstr(parTok, _TOK_AND) == parTok)
            {
                tok = _TOK_AND;
                break;
            }
            else if (strstr(parTok, _TOK_OR) == parTok)
            {
                tok = _TOK_OR;
                break;
            }
        }

        /*  Parse command string  */
        if (parseCommand(strsep(&input, tok), argc, parArgv) == 1)
        {
            return 1;
        }
        else if (strcmp(tok, "") != 0)
        {
            /*  Check if argv is NULL   */
            if (!(*parArgv))
            {
                fprintf(stderr, "syntax error near unexpected token `%s'\n", tok);
                return 1;
            }

            /*  Add connector string    */
            *toks++ = tok;

            if (strcmp(tok, _TOK_AND) == 0 || strcmp(tok, _TOK_OR) == 0)
            {
                /*  Remove trailing char from input   */
                *input++ = '\0';
            }
        }

        /*  Update argc and argv    */
        parArgv = argv + ++(*argc);
    }
    /*  Check if input ends with connectors AND or OR   */
    if (strcmp(tok, _TOK_AND) == 0 || strcmp(tok, _TOK_OR) == 0)
    {
        fprintf(stderr, "syntax error near unexpected token `%s'\n", tok);
        return 1;
    }

    /*  Set end of connector list to NULL   */
    *toks = NULL;

    return 0;
}
