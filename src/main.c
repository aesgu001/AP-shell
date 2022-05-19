#include "../include/prompt.h"

#include <stdio.h>  /*  fgets, printf       */

#define _MAX_INPUT  1024

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

    displayCommandPrompt();
    /*  Read command line input     */
    fgets(input, _MAX_INPUT, stdin);

    return 0;
}