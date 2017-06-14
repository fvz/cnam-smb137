#ifndef BUILTIN_CMD_H_
#define BUILTIN_CMD_H_

#include "mysh.h"

int builtin_loop_scan (mysh_context_p ctx, cmdredir_p r);

void builtin_cmd_exit (mysh_context_p ctx, cmdredir_p r);
void builtin_cmd_echo (mysh_context_p ctx, cmdredir_p r);


static const builtincmd_t builtin_list[] =
{
    { "exit",   builtin_cmd_exit },     /* 'exit' command */
    { "echo",   builtin_cmd_echo },     /* 'echo' command */

    { NULL,     NULL },                 /* == end of array == */
};

#endif /* BUILTIN_CMD_H_ */
