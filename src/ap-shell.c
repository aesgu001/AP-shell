#include "../include/ap-shell.h"

/*
*   Writes the shell's command prompt to standard output.
*   
*   @return None.
*/
void displayPrompt()
{
    /*  Current username.    */
    char *userName = NULL;
    /*  Current host (machine) name.    */
    char hostName[HOST_NAME_MAX];
    /*  Path to current directory.  */
    char *pwdDir = NULL;

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
    pwdDir = getenv(AP_PWD_ENV);
    if (!pwdDir)
    {
        perror("getenv");
    }
    else
    {
        /*  Print current path  */
        printf("%s", pwdDir);
    }

    /*  Print shell symbol  */
    printf("$ ");
}

/*
*   Generates an argument list from the command line input.
*
*   @param  line    command line input.
*   @param  argv    argument list.
*
*   @return None.
*/
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

/*
*   Clones a child process to execute the command line argument, and stores that process's
*   exit status.
*
*   @param  argv        command line argument.
*   @param  exitStatus  child process's exit status.
*
*   @return None.
*/
void executeCommand(char *const argv[], int *exitStatus)
{
    /*  Caller process ID.  */
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
    }
    else if (pid == 0)
    {
        /*  Execute command as child process    */
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
        else if (strcmp(*argv, "exit") == 0)
        {
            break;
        }
        /*  Execute command line argument   */
        executeCommand(argv, &exitStatus);
    }
    
    return 0;
}