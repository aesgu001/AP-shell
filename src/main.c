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
    char commandPrompt[_PROMPT_MAX];

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
    /*  Command line input.     */
    char input[_MAX_INPUT];
    /*  Argument count.         */
    int argc = 0;
    /*  Command line argument.  */
    char *argv[_ARG_MAX];
    /*  Command connectors.     */
    const char *toks[_ARG_MAX];

    while (1)
    {
        displayCommandPrompt();
        /*  Read command line input     */
        fgets(input, _MAX_INPUT, stdin);
        /*  Parse command line input    */
        if (parseInput(input, &argc, argv, toks) == 1 || !(*argv))
        {
            continue;
        }
        else if (strcmp(*argv, "exit") == 0)
        {
            break;
        }
    }

    return 0;
}