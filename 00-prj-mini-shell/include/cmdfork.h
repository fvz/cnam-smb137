#ifndef CMDFORK_H_
#define CMDFORK_H_

#include "mysh.h"

void cmdfork_exec(mysh_context_p ctx, cmdredir_p r);

void cmdfork_pipe(mysh_context_p ctx, cmdredir_p r);
void cmdfork_pipe2(mysh_context_p ctx, cmdredir_p r);
void cmdfork_pipe3(mysh_context_p ctx, cmdredir_p r);
void cmdfork_pipe5(mysh_context_p ctx, cmdredir_p r);

void cmdfork_truncat(mysh_context_p ctx, cmdredir_p r);
void cmdfork_append(mysh_context_p ctx, cmdredir_p r);

#endif /* CMDFORK_H_ */
