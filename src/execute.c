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
                fprintf(stderr, "path length exceeds buffer size\n");
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

int executePiped(command *commands)
{
    /*  I/O state backup.       */
    int stIn = 0, stOut = 0;
    /*  Command pipeline.       */
    int *pipefd = NULL;
    /*  Number of commands.     */
    int nCommands = 0;
    /*  Number of pipes.        */
    int nPipes = 0;
    /*  Current command.        */
    command *cmd;
    /*  Forked process id.      */
    pid_t pid;
    /*  Child process status.   */
    int status = 0;
    /*  Return status.          */
    int exitStatus = 0;

    int i;  /*  Pipe open iterator.     */
    int j;  /*  Command iterator.       */
    int k;  /*  Pipe close iterator.    */


    /*  Back up I/O states  */
    if ((stIn = dup(STDIN_FILENO)) < 0)
    {
        perror("dup");
        return 1;
    }
    else if ((stOut = dup(STDOUT_FILENO)) < 0)
    {
        perror("dup");
        return 1;
    }

    /*  Get number of commands and pipes    */
    for(cmd = commands; cmd; cmd = cmd->next)
    {
        ++nCommands;
    }
    nPipes = nCommands - 1;
    cmd = commands;

    /*  Allocate memory for pipeline   */
    pipefd = (int*)malloc(nPipes * 2 * sizeof(int));
    for (i = 0; i < nPipes; ++i)
    {
        if (pipe(pipefd + i * 2) < 0)
        {
            perror("pipe");
            free(pipefd);
            return 1;
        }
    }

    for (j = 0; j < nCommands; ++j)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            free(pipefd);
            return 1;
        }
        /*  Child process   */
        else if (pid == 0)
        {
            /*  1st pipe  */
            if (j == 0)
            {
                if (dup2(pipefd[_PIPE_WRITE], STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    free(pipefd);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                /*  2nd to nth pipes    */
                if (dup2((pipefd + (j - 1) * 2)[_PIPE_READ], STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    free(pipefd);
                    exit(EXIT_FAILURE);
                }
                /*  2nd to n-1th pipes  */
                else if (j + 1 < nCommands)
                {
                    if (dup2((pipefd + j * 2)[_PIPE_WRITE], STDOUT_FILENO) < 0)
                    {
                        perror("dup2");
                        free(pipefd);
                        exit(EXIT_FAILURE);
                    }
                }
            }

            /*  Close remaining pipes  */
            k = (j < 2)? 0 : j - 1;
            for (; k < nPipes; ++k)
            {
                if (close((pipefd + k * 2)[_PIPE_READ]) < 0)
                {
                    perror("close");
                    free(pipefd);
                    exit(EXIT_FAILURE);
                }
                else if (close((pipefd + k * 2)[_PIPE_WRITE]) < 0)
                {
                    perror("close");
                    free(pipefd);
                    exit(EXIT_FAILURE);
                }
            }

            /*  Execute command     */
            if (execvp(*cmd->argv, cmd->argv) < 0)
            {
                perror("execvp");
                free(pipefd);
                exit(EXIT_FAILURE);
            }
        }

        /*  Parent process  */
        if (j > 0)
        {
            /*  Close pipes no longer needed    */
            if (close((pipefd + (j - 1) * 2)[_PIPE_READ]) < 0)
            {
                perror("close");
                free(pipefd);
                return 1;
            }
            else if (close((pipefd + (j - 1) * 2)[_PIPE_WRITE]) < 0)
            {
                perror("close");
                free(pipefd);
                return 1;
            }
        }

        if (waitpid(-1, &status, 0) < 0)
        {
            perror("waitpid");
            free(pipefd);
            return 1;
        }
        /*  Extract exit status     */
        else if (WIFEXITED(status))
        {
            cmd->status = WEXITSTATUS(status);
            if (exitStatus == 0)
            {
                exitStatus = cmd->status;
            }
        }

        /*  Get next command    */
        cmd = cmd->next;
    }

    /*  Restore I/O states  */
    if (dup2(stIn, STDIN_FILENO) < 0)
    {
        perror("dup2");
        free(pipefd);
        return 1;
    }
    else if (dup2(stOut, STDOUT_FILENO) < 0)
    {
        perror("dup2");
        free(pipefd);
        return 1;
    }

    /*  Free memory from pipeline   */
    free(pipefd);

    return exitStatus;
}

void executeCommand(command *cmd)
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
        if (execvp(*cmd->argv, cmd->argv) < 0)
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
            cmd->status = WEXITSTATUS(status);
        }
    }
}

int executeInput(command *commands, const char *toks[])
{
    /*  Command exit status.        */
    int exitStatus = 0;
    /*  Connector token.            */
    const char *tok = _TOK_EMPTY;
    /*  Command iterator            */
    command *cmd;

    for (cmd = commands; cmd->argv; ++cmd)
    {
        if (cmd->status < 0)
        {
            /*  Meet conditions before executing command    */
            if (strcmp(tok, _TOK_EMPTY) == 0 || strcmp(tok, _TOK_SEMICOLON) == 0 ||
                (strcmp(tok, _TOK_AND) == 0 && exitStatus == EXIT_SUCCESS) ||
                (strcmp(tok, _TOK_OR) == 0 && exitStatus == EXIT_FAILURE))
            {
                /*  Exit program    */
                if (strcmp(*cmd->argv, _CMD_EXIT) == 0)
                {
                    return 1;
                }
                /*  Change directory    */
                else if (strcmp(*cmd->argv, _CMD_CD) == 0)
                {
                    exitStatus = executeChangeDirectory(*(cmd->argv + 1));
                }
                /*  Execute piped   */
                else if (cmd->next)
                {
                    exitStatus = executePiped(cmd);
                }
                else
                {
                    executeCommand(cmd);
                    exitStatus = cmd->status;
                }
            }
            /*  Get next connector token    */
            if (*toks)
            {
                tok = *toks++;
            }
            else
            {
                tok = _TOK_EMPTY;
            }
        }
    }

    return 0;
}
