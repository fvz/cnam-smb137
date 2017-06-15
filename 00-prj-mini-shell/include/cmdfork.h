/**
 * \file cmdfork.h
 * \brief Traitement des forks pour execution des commandes built-in ou non
 * \author Fabien VANTARD
 * \version 0.1
 */

#ifndef CMDFORK_H_
#define CMDFORK_H_

#include "mysh.h"

void cmdfork_exec(mysh_context_p ctx, cmdredir_p r);

void cmdfork_pipe(mysh_context_p ctx, cmdredir_p r);
void cmdfork_pipe2(mysh_context_p ctx, cmdredir_p r);
void cmdfork_pipe3(mysh_context_p ctx, cmdredir_p r);
void cmdfork_pipe4(mysh_context_p ctx, cmdredir_p r);

void cmdfork_truncat(mysh_context_p ctx, cmdredir_p r);
void cmdfork_append(mysh_context_p ctx, cmdredir_p r);
void cmdfork_input(mysh_context_p ctx, cmdredir_p r);
void cmdfork_dinput(mysh_context_p ctx, cmdredir_p r);

#endif /* CMDFORK_H_ */
