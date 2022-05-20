#include "../include/prompt.h"

int getCommandPrompt(char *buf, size_t size)
{
    /*  Prompt length.                      */
    size_t length = 0;
    /*  User database entry.                */
    struct passwd *user = NULL;
    /*  Current login name.                 */
    const char *loginName = NULL;
    /*  Current host (machine) name.        */
    char hostName[_HOST_NAME_MAX];
    /*  Current working directory.          */
    const char *cwd = NULL;
    /*  Current home directory.             */
    const char *homeDir = NULL;
    /*  Path to current working directory.  */
    char pwd[_PATH_MAX];

    /*  Check if buffer is NULL     */
    if (!buf)
    {
        fprintf(stderr, "buffer is NULL\n");
        return 1;
    }

    /*  Get login name + '@'    */
    user = getpwuid(geteuid());
    if (!user)
    {
        perror("getpwuid");
        return 1;
    }
    loginName = user->pw_name;
    length += strlen(loginName) + 1;

    /*  Get host name + ':' */
    if (gethostname(hostName, _HOST_NAME_MAX) < 0)
    {
        perror("gethostname");
        return 1;
    }
    length += strlen(hostName) + 1;

    /*  Get cwd path    */
    cwd = getenv(_PWD_ENV);
    if (!cwd)
    {
        perror("getenv");
        return 1;
    }
    else if (strlen(cwd) >= _PATH_MAX)
    {
        fprintf(stderr, "cwd length exceeds path size\n");
        return 1;
    }
    homeDir = getenv(_HOME_ENV);
    if (!homeDir)
    {
        perror("getenv");
        return 1;
    }
    *pwd = '\0';
    if (strstr(cwd, homeDir) == cwd)
    {
        /*  Truncated   */
        strcat(pwd, "~");
        strcat(pwd, cwd + strlen(homeDir));
    }
    else
    {
        /*  Raw     */
        strcat(pwd, cwd);
    }
    length += strlen(pwd);

    /*  Check if prompt length exceeds buffer size  */
    if (length >= size)
    {
        fprintf(stderr, "prompt length exceeds buffer size\n");
        return 1;
    }

    /*  Store all information in buffer     */
    *buf = '\0';
    strcat(buf, loginName);
    strcat(buf, "@");
    strcat(buf, hostName);
    strcat(buf, ":");
    strcat(buf, pwd);

    return 0;
}