#ifndef MYSH_FACILITIES_H_
#define MYSH_FACILITIES_H_

#include "mysh.h"

#define BUFFER_SIZE 1024


void freeif(void* to_free);
char* strcat_dup(const char *s1, const char *s2);

#endif /* MYSH_FACILITIES_H_ */
