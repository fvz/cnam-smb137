/**
 * \file cmdhandle.c
 * \brief Traitement d'une cmdline
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "cmdhandle.h"

/**
 * \fn void cmdline_handle (mysh_context_p ctx, char *cmdline)
 * \brief Traite une cmdline.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param cmdline chaine de caractères contenant la cmdline.
 */
void cmdline_handle (mysh_context_p ctx, char *cmdline) {
    cmdoper_p o;
    cmdredir_p r;

    cmdoper_p mycmdoper;
    mysh_history_add(ctx, cmdline);

    ctx_dbmyprintf(1, ctx, M_CMDHANDLE_START_PARSER);
    mycmdoper = cmdoper_parser (ctx, cmdline);
    cmdoper_parse_redir (ctx, mycmdoper);

    ctx_dbmyprintf(1, ctx, M_CMDHANDLE_START_ENGINE);

    /* for all cmdoper */
    for(o=mycmdoper; o != NULL; o=o->next) {

        cmdoper_print_one (ctx, o);
        ctx_dbmyprintf(1, ctx, M_CMDHANDLE_HANDLING_CMD, o->cmd);

        if (ctx->status == CTX_STATUS_EXIT) {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_IN_EXITING_WF, o->cmd);
            continue;
        }

        if (o->prev && (
            (o->prev->exitstatus == EXIT_FAILURE && o->prev->type == CMDOPER_AND) ||
            (o->prev->exitstatus == EXIT_SUCCESS && o->prev->type == CMDOPER_OR)
        )) {
            /* The PREVIOUS cmd has returned=EXIT_FAILURE and operator=AND
                                 or returned=EXIT_SUCCESS and operator=OR,
                so we consider this CURRENT command is virtually exiting
                with the same status of the PREVIOUS command. */
            o->exitstatus = o->prev->exitstatus;
            o->cmdexec = false; /* but keep info that cmd not executed */
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_PREVCMD_OPER_SO_NOEXEC,
                (o->prev->type == CMDOPER_AND) ? "AND" : "OR",
                (o->prev->exitstatus == EXIT_FAILURE) ? "EXIT_FAILURE" : "EXIT_SUCCESS",
                o->cmd);
            continue;
        }
        /* for(r=o->redir; r != NULL; r=r->next)  // if we want to loop in all redir */

        r = o->redir;
        ctx_dbmyprintf(1, ctx, M_CMDHANDLE_HANDLING_REDIR, r->args[0]);
        cmdalias_check_alias(ctx, r);

        switch(r->redir) {
            case CMDREDIR_EMPTY:
                cmdfork_exec(ctx, r);
                break;

            case CMDREDIR_PIPE:
                cmdfork_pipe4(ctx, r);
                break;

            case CMDREDIR_TRUNCAT:
                cmdfork_truncat(ctx, r);
                break;

            case CMDREDIR_APPEND:
                cmdfork_append(ctx, r);
                break;

            case CMDREDIR_INPUT:
            cmdfork_input(ctx, r);
                break;
            case CMDREDIR_DINPUT:
            cmdfork_dinput(ctx, r);
                break;
            default:
                break;
        }
    }

    cmdoper_free(mycmdoper);
    ctx_dbmyprintf(1, ctx, M_CMDHANDLE_END_ENGINE);
}
