#ifndef AP_SHELL_H
#define AP_SHELL_H

#include <bits/local_lim.h> /*  HOST_NAME_MAX   */
#include <pwd.h>            /*  getpwuid    */
#include <stdio.h>          /*  fgets, printf, stdin    */
#include <stdlib.h>         /*  exit, getenv    */
#include <string.h>         /*  strcmp  */
#include <sys/wait.h>       /*  waitpid */
#include <unistd.h>         /*  fork, geteuid, gethostname  */

#define AP_ARG_MAX  64
#define AP_CMD_LEN  1024

#define AP_PWD_ENV  "PWD"

#endif