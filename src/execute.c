#include "../include/execute.h"

int openCommandFiles(int *in, int *out, command *cmd)
{
    /*  Output write flags.             */
    int oFlags = O_WRONLY | O_CREAT;
    /*  File mode flags.                */
    mode_t mFlags =  S_IRUSR | S_IWUSR;

    /*  Open in file    */
    if (cmd->in && cmd->inTok)
    {
        if ((*in = open(cmd->in, O_RDONLY)) < 0)
        {
            perror("open");
            return 1;
        }
        else if (dup2(*in, STDIN_FILENO) < 0)
        {
            perror("dup2");
            return 1;
        }
        else if (close(*in) < 0)
        {
            perror("close");
            return 1;
        }
    }
    /*  Open out file   */
    if (cmd->out && cmd->outTok)
    {
        /*  Set appropriate output flags    */
        if (strcmp(cmd->outTok, _TOK_OUTAPPEND) == 0)
        {
            oFlags |= O_APPEND;
        }
        else
        {
            oFlags |= O_TRUNC;
        }

        if ((*out = open(cmd->out, oFlags, mFlags)) < 0)
        {
            perror("open");
            return 1;
        }
        else if (dup2(*out, STDOUT_FILENO) < 0)
        {
            perror("dup2");
            return 1;
        }
        else if (close(*out) < 0)
        {
            perror("close");
            return 1;
        }       
    }

    return 0;
}

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

int executePiped(command *cmdLink)
{
    /*  I/O state backup.       */
    int stIn, stOut;
    /*  I/O file descriptor.    */
    int in, out;

    /*  Signal mask backup.                 */
    sigset_t stBlock;
    /*  Interrupt signal action backup.     */
    struct sigaction stSigInt;
    /*  Interrupt signal action override.   */
    struct sigaction saSigInt;

    /*  Command pipeline.       */
    int pipefd[_PIPE_MAX];
    /*  Number of commands.     */
    int nCommands;
    /*  Number of pipes.        */
    int nPipes;
    /*  Pipe iterator.          */
    int i;
    /*  Command iterator.       */
    int j;
    /*  Current command.        */
    command *cmd;

    /*  Forked process ID.      */
    pid_t pid;
    /*  Wait status.            */
    int wStatus;
    /*  Execute status.         */
    int status;


    /*  Update command link status  */
    nCommands = 0;
    for(cmd = cmdLink; cmd; cmd = cmd->next)
    {
        cmd->status = 0;
        ++nCommands;
    }
    cmd = cmdLink;
    nPipes = nCommands - 1;

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

    /*  Set up and backup interrupt signal action   */
    if (sigemptyset(&saSigInt.sa_mask) < 0)
    {
        perror("sigemptyset");
        return 1;
    }
    saSigInt.sa_flags = SA_RESTART;
    saSigInt.sa_handler = SIG_IGN;
    if (sigaction(SIGINT, &saSigInt, &stSigInt) < 0)
    {
        perror("sigaction");
        return 1;
    }
    else if (sigaddset(&saSigInt.sa_mask, SIGCHLD))
    {
        perror("sigaddset");
        return 1;
    }
    else if (sigprocmask(SIG_BLOCK, &saSigInt.sa_mask, &stBlock))
    {
        perror("sigprocmask");
        return 1;
    }

    /*  Create unidirectional pipe  */
    if (nPipes > _PIPE_MAX)
    {
        fprintf(stderr, "Too many commands\n");
        return 1;
    }
    for (i = 0; i < nPipes; ++i)
    {
        if (pipe(pipefd + i * 2) < 0)
        {
            perror("pipe");
            return 1;
        }
    }

    /*  Fork process    */
    for (j = 0, status = 0; cmd && j < nCommands; ++j)
    {
        if ((pid = fork()) < 0)
        {
            perror("fork");
            return 1;
        }
        /*  Child process   */
        else if (pid == 0)
        {
            /*  Restore interrupt signal action     */
            if (sigaction(SIGINT, &stSigInt, NULL) < 0)
            {
                perror("sigaction");
                exit(EXIT_FAILURE);
            }
            else if (sigprocmask(SIG_SETMASK, &stBlock, NULL) < 0)
            {
                perror("sigprocmask");
                exit(EXIT_FAILURE);
            }

            /*  1st to n-1th pipes  */
            if (j <= 0 || j + 1 < nCommands)
            {
                if (dup2((pipefd + j * 2)[_PIPE_WRITE], STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            /*  2nd to nth pipes    */
            if (j > 0)
            {
                if (dup2((pipefd + (j - 1) * 2)[_PIPE_READ], STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            /*  Close remaining pipes  */
            i = (j < 2)? 0 : j - 1;
            for (; i < nPipes; ++i)
            {
                if (close((pipefd + i * 2)[_PIPE_READ]) < 0)
                {
                    perror("close");
                    exit(EXIT_FAILURE);
                }
                else if (close((pipefd + i * 2)[_PIPE_WRITE]) < 0)
                {
                    perror("close");
                    exit(EXIT_FAILURE);
                }
            }

            /*  Open I/O files  */
            if (openCommandFiles(&in, &out, cmd) == 1)
            {
                exit(EXIT_FAILURE);
            }

            /*  Execute command     */
            else if (execvp(*cmd->argv, cmd->argv) < 0)
            {
                perror("execvp");
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
                return 1;
            }
            else if (close((pipefd + (j - 1) * 2)[_PIPE_WRITE]) < 0)
            {
                perror("close");
                return 1;
            }
        }

        /*  Wait for child process to finish    */
        if (waitpid(pid, &wStatus, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }
        else if (WIFEXITED(wStatus))
        {
            if (status == 0)
            {
                status = WEXITSTATUS(wStatus);
            }
        }
        else if (WIFSIGNALED(wStatus))
        {
            status = _EXIT_SIG + WTERMSIG(wStatus);
            putchar('\n');

            /*  Close remaining pipes   */
            for (i = j; i < nPipes; ++i)
            {
                if (close((pipefd + i * 2)[_PIPE_READ]) < 0)
                {
                    perror("close");
                    return 1;
                }
                else if (close((pipefd + i * 2)[_PIPE_WRITE]) < 0)
                {
                    perror("close");
                    return 1;
                }
            }

            break;
        }
        cmd = cmd->next;
    }

    /*  Restore interrupt signal action     */
    if (sigaction(SIGINT, &stSigInt, NULL) < 0)
    {
        perror("sigaction");
        return 1;
    }
    else if (sigprocmask(SIG_SETMASK, &stBlock, NULL) < 0)
    {
        perror("sigprocmask");
        return 1;
    }

    /*  Restore I/O states  */
    if (dup2(stIn, STDIN_FILENO) < 0)
    {
        perror("dup2");
        return 1;
    }
    else if (dup2(stOut, STDOUT_FILENO) < 0)
    {
        perror("dup2");
        return 1;
    }


    return status;
}

int executeCommand(command *cmd)
{
    /*  I/O state backup.       */
    int stIn, stOut;
    /*  I/O file descriptor.    */
    int in, out;

    /*  Signal mask backup.                 */
    sigset_t stBlock;
    /*  Interrupt signal action backup.     */
    struct sigaction stSigInt;
    /*  Interrupt signal action override.   */
    struct sigaction saSigInt;

    /*  Forked process ID.      */
    pid_t pid;
    /*  Wait status.            */
    int wStatus;
    /*  Execute status.         */
    int status;


    /*  Update command status   */
    cmd->status = 0;

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

    /*  Set up and backup interrupt signal action   */
    if (sigemptyset(&saSigInt.sa_mask) < 0)
    {
        perror("sigemptyset");
        return 1;
    }
    saSigInt.sa_flags = SA_RESTART;
    saSigInt.sa_handler = SIG_IGN;
    if (sigaction(SIGINT, &saSigInt, &stSigInt) < 0)
    {
        perror("sigaction");
        return 1;
    }
    else if (sigaddset(&saSigInt.sa_mask, SIGCHLD))
    {
        perror("sigaddset");
        return 1;
    }
    else if (sigprocmask(SIG_BLOCK, &saSigInt.sa_mask, &stBlock))
    {
        perror("sigprocmask");
        return 1;
    }

    /*  Fork process    */
    if ((pid = fork()) < 0)
    {
        perror("fork");
        return 1;
    }
    else if (pid == 0)
    {
        /*  Restore interrupt signal action     */
        if (sigaction(SIGINT, &stSigInt, NULL) < 0)
        {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
        else if (sigprocmask(SIG_SETMASK, &stBlock, NULL) < 0)
        {
            perror("sigprocmask");
            exit(EXIT_FAILURE);
        }

        /*  Open I/O files      */
        else if (openCommandFiles(&in, &out, cmd) == 1)
        {
            exit(EXIT_FAILURE);
        }

        /*  Execute command     */
        else if (execvp(*cmd->argv, cmd->argv) < 0)
        {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    /*  Wait for child process to finish    */
    if (waitpid(pid, &wStatus, 0) < 0)
    {
        perror("waitpid");
        return 1;
    }
    else if (WIFEXITED(wStatus))
    {
        status = WEXITSTATUS(wStatus);
    }
    else if (WIFSIGNALED(wStatus))
    {
        status = _EXIT_SIG + WTERMSIG(wStatus);
        putchar('\n');
    }

    /*  Restore interrupt signal action     */
    if (sigaction(SIGINT, &stSigInt, NULL) < 0)
    {
        perror("sigaction");
        return 1;
    }
    else if (sigprocmask(SIG_SETMASK, &stBlock, NULL) < 0)
    {
        perror("sigprocmask");
        return 1;
    }

    /*  Restore I/O states  */
    if (dup2(stIn, STDIN_FILENO) < 0)
    {
        perror("dup2");
        return 1;
    }
    else if (dup2(stOut, STDOUT_FILENO) < 0)
    {
        perror("dup2");
        return 1;
    }

    
    return status;
}

int executeInput(command *commands, const char *toks[])
{
    /*  Command exit status.        */
    int exitStatus = 0;
    /*  Connector token.            */
    const char *tok = _TOK_EMPTY;
    /*  Command iterator            */
    command *cmd;

    for (cmd = commands; exitStatus <= 127 && cmd->argv; ++cmd)
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
                    exitStatus = executeCommand(cmd);
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
