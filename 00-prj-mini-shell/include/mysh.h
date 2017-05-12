/**
 * \file mysh.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
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
