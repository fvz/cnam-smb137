/**
 * \file builtin_cmd.c
 * \brief Fonctionnalités Built-in
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "builtin_cmd.h"

void builtin_cmd_exit (mysh_context_p ctx, cmdoper_p oper) {

    ctx_dbmyprintf(1, ctx, M_CMDHANDLE_PROCESS_CMD_EXIT);

    if (oper->prev) {
        if ((oper->prev->oper == CMDOPER_AND) && (oper->prev->cmdstatus == true)) {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_OPER_AND);
            ctx->status = CTX_STATUS_EXIT;
        } else if ((oper->prev->oper == CMDOPER_OR) && (oper->prev->cmdstatus == false)) {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_OPER_OR);
            ctx->status = CTX_STATUS_EXIT;
        } else if (oper->prev->oper == CMDOPER_SEMICOLON) {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_OPER_SCOLON);
            ctx->status = CTX_STATUS_EXIT;
        } else {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT);
            ctx->status = CTX_STATUS_EXIT;
        }
    } else {
        ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_NO_PREVCMD);
        ctx->status = CTX_STATUS_EXIT;
    }
}

void builtin_cmd_echo (mysh_context_p ctx, cmdoper_p oper) {

    ctx_dbmyprintf(1, ctx, M_CMDHANDLE_PROCESS_CMD_ECHO);

    /* TODO */
}
