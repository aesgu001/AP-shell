#include "../include/execute.h"

int executeChangeDirectory(const char *path)
{
    /*  Path to previous working directory.     */
    const char *oldpwd = NULL;
    /*  Path to destination directory.          */
    const char *destDir = NULL;
    /*  Path to current working directory.      */
    char pwd[_PATH_MAX];

    /*  Get previous working directory  */
    oldpwd = getenv(_ENV_PWD);
    if (!oldpwd)
    {
        perror("getenv");
        return 1;
    }

    if (!path || strcmp(path, "") == 0 || strstr(path, "~") == path)
    {
        /*  Get path to HOME directory  */
        destDir = getenv(_ENV_HOME);
        if (!destDir)
        {
            perror("getenv");
            return 1;
        }
        else if (path)
        {
            /*  Get path to HOME subdirectory   */
            if (strlen(destDir) + strlen(path + 1) >= _PATH_MAX)
            {
                fprintf(stderr, "cd: path length exceeds buffer size\n");
                return 1;
            }
            /*  Store it in buffer  */
            *pwd = '\0'; 
            strcat(pwd, destDir);
            strcat(pwd, path + 1);
            destDir = pwd;
        }
    }
    else if (strcmp(path, "-") == 0)
    {
        /*  Get path to OLDPWD directory    */
        destDir = getenv(_ENV_OLDPWD);
        if (!destDir)
        {
            perror("getenv");
            return 1;
        }
        /*  Print it    */
        printf("%s\n", destDir);
    }
    else
    {
        /*  Get path to destination directory   */
        destDir = path;
    }

    /*  Change directory    */
    if (chdir(destDir) < 0)
    {
        perror("chdir");
        return 1;
    }
    /*  Set OLDPWD to previous env  */
    else if (setenv(_ENV_OLDPWD, oldpwd, 1) < 0)
    {
        perror("setenv");
        return 1;
    }

    /*  Get current working directory   */
    else if (!getcwd(pwd, _PATH_MAX))
    {
        perror("getcwd");
        return 1;
    }
    /*  Set PWD to new env  */
    else if (setenv(_ENV_PWD, pwd, 1) < 0)
    {
        perror("setenv");
        return 1;
    }

    return 0;
}

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
    const char *tok = _TOK_SC;

    int itArgv, itTok;
    for (itArgv = 0, itTok = 0; itArgv < argc && argv[itArgv];)
    {
        /*  Meet conditions before executing command    */
        if (strcmp(tok, _TOK_SC) == 0 || (strcmp(tok, _TOK_AND) == 0 && exitStatus == EXIT_SUCCESS) ||
            (strcmp(tok, _TOK_OR) == 0 && exitStatus == EXIT_FAILURE))
        {
            /*  Exit program    */
            if (strcmp(argv[itArgv], _CMD_EXIT) == 0)
            {
                return 1;
            }
            /*  Change directory    */
            else if (strcmp(argv[itArgv], _CMD_CD) == 0)
            {
                exitStatus = executeChangeDirectory(argv[itArgv + 1]);
            }
            else
            {
                executeCommand(&argv[itArgv], &exitStatus);
            }
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
            tok = _TOK_SC;
        }
    }

    return 0;
}
