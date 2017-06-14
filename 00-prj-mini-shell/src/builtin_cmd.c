/**
 * \file builtin_cmd.c
 * \brief Fonctionnalités Built-in (référencées au sein d'un tableau dans le header associé)
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "builtin_cmd.h"


/**
 * \fn int builtin_loop (mysh_context_p ctx, cmdoper_p o)
 * \brief Cherche si cmd présente parmi les builtin et exécute la fonction le cas échéant.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param o Pointeur sur le CmdOper en cours
 * \param r Pointeur sur le CmdRedir en cours
 *
 * \return found : retourne vrai si une commande built-in a été trouvée (et exécutée) et faux sinon.
 */
int builtin_loop_scan (mysh_context_p ctx, cmdoper_p o, cmdredir_p r) {
    /* TODO :
    In the cmdredir_p object, add 'parent' property for pointing to
    it's parent (a cmdoper object) */
    int i;
    int found = false;

    for(i=0; builtin_list[i].cb != NULL; i++) {
        if (strcmp(r->args[0], builtin_list[i].cmd) == 0) {
            ctx_dbmyprintf(1, ctx, M_BUILTIN_PROCESS_BUILTIN_CMD, r->args[0]);
            builtin_list[i].cb(ctx, o);
            found = true;
            break;
        }
    }
    return found;
}

/**
 * \fn void builtin_cmd_exit (mysh_context_p ctx, cmdoper_p oper)
 * \brief Commande Built-in "exit" : elle fait quitter le programme.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param o Pointeur sur le CmdOper en cours
 */
void builtin_cmd_exit (mysh_context_p ctx, cmdoper_p oper) {

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

/**
 * \fn void builtin_cmd_echo (mysh_context_p ctx, cmdoper_p oper)
 * \brief Commande Built-in "echo" : elle affiche un message sur stdin.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param o Pointeur sur le CmdOper en cours
 */
void builtin_cmd_echo (mysh_context_p ctx, cmdoper_p oper) {

    /* TODO */
}
