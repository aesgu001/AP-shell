#include "../include/execute.h"
#include "../include/parse.h"
#include "../include/prompt.h"

/*
*   Writes the shell command prompt to standard output.
*
*   @return None.
*/
void displayCommandPrompt()
{
    /*  Shell command prompt.   */
    char commandPrompt[_INPUT_MAX];

    /*  Print command prompt  */
    if (getCommandPrompt(commandPrompt, _PROMPT_MAX) == 0)
    {
        printf("%s", commandPrompt);
    }
    /*  Print shell symbol  */
    printf("$ ");
}

int main()
{
    /*  Command line input.             */
    char input[_INPUT_MAX];
    /*  Command list.                   */
    command commands[_COMMAND_MAX];
    /*  Connector tokens.               */
    const char *toks[_ARG_MAX];

    while (1)
    {
        displayCommandPrompt();
        /*  Read command line input     */
        fgets(input, _INPUT_MAX, stdin);
        /*  Parse command line input    */
        if (parseInput(input, commands, toks) == 1)
        {
            continue;
        }
        /*  Execute command(s)  */
        else if (executeInput(commands, toks) == 1)
        {
            break;
        }
    }

    return 0;
}
