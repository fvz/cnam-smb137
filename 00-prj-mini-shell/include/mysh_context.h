#ifndef MYSH_CONTEXT_H
#define MYSH_CONTEXT_H

#include "mysh.h"

void mysh_context_init (mysh_context_p ctx, int argc, char **argv);
mysh_context_p mysh_context_new (int argc, char** argv);
void mysh_context_free (mysh_context_p ctx);

#endif /* MYSH_CONTEXT_H */
