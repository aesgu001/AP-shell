#include "../include/ap-shell.h"

void displayPrompt()
{
    /*  Current username.    */
    char *userName = NULL;
    /*  Current host (machine) name.    */
    char hostName[HOST_NAME_MAX];
    /*  Current working directory.  */
    char *cwd = NULL;
    /*  Current home directory.     */
    char *homeDir = NULL;

    /*  Get username    */
    userName = getpwuid(geteuid())->pw_name;
    if (!userName)
    {
        perror("getpwuid");
    }
    /*  Get hostname    */
    else if (gethostname(hostName, HOST_NAME_MAX) < 0)
    {
        perror("gethostname");
    }
    else
    {
        /*  Print them  */
        printf("%s@%s:", userName, hostName);
    }

    /*  Get "PWD" env value     */
    cwd = getenv(AP_ENV_PWD);
    if (!cwd)
    {
        perror("getenv");
    }
    /*  Get "HOME" env value    */
    homeDir = getenv(AP_ENV_HOME);
    if (!homeDir)
    {
        perror("getenv");
    }
    else if (cwd)
    {
        if (strstr(cwd, homeDir) == cwd)
        {
            /*  Print cwd path truncated   */
            printf("~%s", cwd + strlen(homeDir));
        }
        else
        {
            /*  Print cwd path raw  */
            printf("%s", cwd);
        }
    }

    /*  Print shell symbol  */
    printf("$ ");
}

void parseLine(char *line, char **argv)
{
    /*  Parse command line until null character found   */
    while (*line != '\0')
    {
        /*  Replace newline and whitespace characters with '\0'  */
        while (*line == ' ' || *line == '\n')
        {
            *line++ = '\0';
        }

        /*  Break from loop     */
        if (*line == '\0')
        {
            break;
        }

        /*  Store argument position  */
        *argv++ = line++;
        while (*line != '\0' && *line != '\n' && *line != ' ')
        {
            ++line;
        }
    }

    /*  Set end of argument list to NULL    */
    *argv = NULL;
}

int executecd(const char *path)
{
    /*  Current working directory.  */
    const char *cwd = NULL;
    /*  Path to destination directory.  */
    const char *destDir = NULL;
    /*  New working directory.  */
    char nwd[PATH_MAX];

    /*  Get current env value   */
    cwd = getenv(AP_ENV_PWD);
    if (!cwd)
    {
        perror("getenv");
        return EXIT_FAILURE;
    }

    if (!path || strcmp(path, "~") == 0)
    {
        /*  Get path to HOME directory  */
        destDir = getenv(AP_ENV_HOME);
        if (!destDir)
        {
            perror("getenv");
            return EXIT_FAILURE;
        }
    }
    else if (strcmp(path, "-") == 0)
    {
        /*  Get path to OLDPWD directory    */
        destDir = getenv(AP_ENV_OLDPWD);
        if (!destDir)
        {
            perror("getenv");
            return EXIT_FAILURE;
        }
        else
        {
            /*  Print it    */
            printf("%s\n", destDir);
        }
    }
    else
    {
        /*  Get path to destination directory   */
        destDir = path;
    }

    /*  Change current working directory    */
    if (chdir(destDir) < 0)
    {
        perror("chdir");
        return EXIT_FAILURE;
    }
    /*  Set OLDPWD to previous env  */
    else if (setenv(AP_ENV_OLDPWD, cwd, 1) < 0)
    {
        perror("setenv");
        return EXIT_FAILURE;
    }

    /*  Get new working directory   */
    if (!getcwd(nwd, PATH_MAX))
    {
        perror("getcwd");
        return EXIT_FAILURE;
    }
    /*  Set PWD to new env  */
    else if (setenv(AP_ENV_PWD, nwd, 1) < 0)
    {
        perror("setenv");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void executeCommand(char *const argv[], int *exitStatus)
{
    /*  Forked process ID.  */
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
    }
    else if (pid == 0)
    {
        /*  Execute in child process    */
        if (execvp(*argv, argv) < 0)
        {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        /*  Wait for child process to finish    */
        if (waitpid(-1, exitStatus, 0) < 0)
        {
            perror("waitpid");
        }
    }
}

/*      DRIVER CODE     */
int main()
{
    /*  Command line input.     */
    char line[AP_CMD_LEN];
    /*  Command line argument.  */
    char *argv[AP_ARG_MAX];
    /*  Command exit status.    */
    int exitStatus = 0;

    while (1)
    {
        /*  Display command prompt    */
        displayPrompt();
        /*  Read command line   */
        fgets(line, AP_ARG_MAX, stdin);
        /*  Parse command line  */
        parseLine(line, argv);
        /*  Continue if argument NULL  */
        if (!(*argv))
        {
            continue;
        }
        /*  Break from loop if exit called  */
        else if (strcmp(*argv, AP_CMD_EXIT) == 0)
        {
            break;
        }
        /*  Handle in program if cd called  */
        else if (strcmp(*argv, AP_CMD_CD) == 0)
        {
            exitStatus = executecd(*(argv + 1));
            continue;
        }
        /*  Execute command line argument   */
        executeCommand(argv, &exitStatus);
    }
    
    return 0;
}
