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

        /* for all cmdredir of each cmdoper */
        for(r=o->redir; r != NULL; r=r->next) {

            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_HANDLING_REDIR, r->args[0]);

            cmdalias_p alias;
            if ((alias = cmdalias_search(ctx, r->args[0])) != NULL) {
                char **array;
                int i;
                ctx_dbmyprintf(1, ctx, M_CMDHANDLE_ALIAS_FOUND, r->args[0], alias->name, alias->cmd);

                /* replace r->args[0] by cmdargs_parse(alias->cmd) */ 
                cmdargs_parser(ctx, alias->cmd, &array);
                for (i=1; r->args[i]; i++) {
                    cmdargs_append_with_new(&array, r->args[i]);
                    r->args[i] = NULL;
                }
                cmdargs_free(&(r->args));
                r->args = array;

                /* Perhaps we can redefine r->cmd (by joining r->args with ' ' separators), but
                   the handling mecanism is working with r->args only. So it is a bit useless. */
            }

            if (ctx->status == CTX_STATUS_EXIT) {
                ctx_dbmyprintf(1, ctx, M_CMDHANDLE_IN_EXITING_WF, o->cmd);
                continue;
            }

            if (o->prev && (
                (o->prev->type == CMDOPER_AND && o->prev->exitstatus == EXIT_FAILURE) ||
                (o->prev->type == CMDOPER_OR && o->prev->exitstatus == EXIT_SUCCESS)
            )) {
                /*Don't exec the CURRENT command (because the PREVIOUS command
                returned an EXIT_FAILURE status and the operator was a 'AND'.
                But we consider that the CURRENT command is virtually exiting
                with an EXIT_SUCCESS status.
                --OR--
                Don't exec the CURRENT command (because the PREVIOUS command
                returned an EXIT_SUCCESS status and the operator was an 'OR'.
                But we consider that the CURRENT command is virtually exiting
                with an EXIT_SUCCESS status.*/
                o->exitstatus = EXIT_SUCCESS;
                o->cmdexec = false; /* but keep info that cmd not executed */

                ctx_dbmyprintf(1, ctx, M_CMDHANDLE_PREVCMD_OPER_SO_NOEXEC,
                    (o->prev->type == CMDOPER_AND) ? "AND" : "OR",
                    (o->prev->exitstatus == EXIT_FAILURE) ? "EXIT_FAILURE" : "EXIT_SUCCESS",
                    o->cmd);

                continue;
            }

            if (builtin_loop_scan (ctx, r)) {
                /* a builtin cmd has been found and executed. */
                continue;
            }

            /* we process a fork if cmd isn't a built-in cmd */
            cmdfork_do(ctx, r);
        }
    }

    ctx_dbmyprintf(1, ctx, M_CMDHANDLE_END_ENGINE);

    //cmdoper_print(mycmdoper);
    cmdoper_free(mycmdoper);
}
