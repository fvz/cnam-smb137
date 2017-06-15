#ifndef MYSH_MYSH_H_
#define MYSH_MYSH_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
#include <getopt.h>


/* for catching CTRL+C */
#include <signal.h>

/* va_list / va_start / va_end */
#include <stdarg.h>

/* true and false constants */
#include <stdbool.h>

/* getpwuid/getuid */
#include <pwd.h>


/* mysh project */
#include "typedef.h"
#include "cmdoper_parser.h"
#include "cmdredir_parser.h"
#include "mysh_context.h"
#include "mysh_history.h"
#include "facilities.h"
#include "mysh_prompt.h"
#include "cmdhandle.h"
#include "cmdargs_parser.h"
#include "myprintf.h"
#include "builtin_cmd.h"
#include "cmdfork.h"
#include "cmdalias.h"

#define BUFFER_SIZE 1024

#endif /* MYSH_MYSH_H_ */
