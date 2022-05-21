#include "../include/execute.h"

void executeCommand(char *const argv[], int *exitStatus)
{
    /*  Child process status.   */
    int status = 0;
    /*  Forked process ID.      */
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
    }
    else if (pid == 0)
    {
        /*  Execute command line argument   */
        if (execvp(*argv, argv) < 0)
        {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        /*  Wait for child process to finish    */
        if (waitpid(-1, &status, 0) < 0)
        {
            perror("waitpid");
        }
        /*  Extract exit status     */
        else if (WIFEXITED(status))
        {
            *exitStatus = WEXITSTATUS(status);
        }
    }
}

int executeInput(int argc, char *argv[], const char *toks[])
{
    /*  Command exit status.    */
    int exitStatus = 0;
    /*  Command connector.      */
    const char *tok = _SEMICOLON;

    int itArgv, itTok;
    for (itArgv = 0, itTok = 0; itArgv < argc && argv[itArgv];)
    {
        /*  Meet conditions before executing command    */
        if (strcmp(tok, _SEMICOLON) == 0 || (strcmp(tok, _AND) == 0 && exitStatus == EXIT_SUCCESS) ||
            (strcmp(tok, _OR) == 0 && exitStatus == EXIT_FAILURE))
        {
            /*  Exit program    */
            if (strcmp(argv[itArgv], "exit") == 0)
            {
                return 1;
            }

            executeCommand(&argv[itArgv], &exitStatus);
        }

        /*  Get next command    */
        while (argv[itArgv++]);
        /*  Get next connector  */
        if ((tok = toks[itTok]) != NULL)
        {
            ++itTok;
        }
        else
        {
            tok = _SEMICOLON;
        }
    }

    return 0;
}
