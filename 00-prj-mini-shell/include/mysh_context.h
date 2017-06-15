/**
 * \file mysh_context.h
 * \brief Gestion du contexte de session
 * \author Fabien VANTARD
 * \version 0.1
 */

#ifndef MYSH_CONTEXT_H_
#define MYSH_CONTEXT_H_

#include "mysh.h"

void mysh_context_init (mysh_context_p ctx, int argc, char **argv);
mysh_context_p mysh_context_new (int argc, char** argv);
void mysh_context_free (mysh_context_p ctx);

#endif /* MYSH_CONTEXT_H_ */
