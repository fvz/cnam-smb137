#ifndef BUILTIN_CMD_H
#define BUILTIN_CMD_H

#include "mysh.h"


void builtin_cmd_exit (mysh_context_p ctx, cmdoper_p oper);
void builtin_cmd_echo (mysh_context_p ctx, cmdoper_p oper);


static const builtincmd_t builtin_list[] =
{
    { "exit",   builtin_cmd_exit },     /* 'exit' command */
    { "echo",   builtin_cmd_echo },     /* 'echo' command */

    { NULL,     NULL },                 /* == end of array == */
};

#endif /* BUILTIN_CMD_H */
