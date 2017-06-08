#ifndef MYSH_FACILITIES_H
#define MYSH_FACILITIES_H

#include "mysh.h"

#define BUFFER_SIZE 1024

void freeif(void* to_free);
void dbprintf (/*ctx, verbose_reach, debug_reach */ const char *fmt, ...);



#endif /* MYSH_FACILITIES_H */
