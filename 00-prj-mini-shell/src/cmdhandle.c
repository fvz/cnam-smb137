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

            if (ctx->status == CTX_STATUS_EXIT) {
                ctx_dbmyprintf(1, ctx, M_CMDHANDLE_IN_EXITING_WF, o->cmd);
                continue;
            }

            if (!builtin_loop_scan (ctx, r)) {
                printf("===CHILD / FORK ===\n");
                if (r->prev == NULL) {
                    int status = 0;
                    pid_t child;

                    child = fork();
                    if (child == 0) {
                        execvp(r->args[0], r->args);
                        perror("execlp");
                    } else {
                        waitpid(child, &status, 0);
                    }
                }
            }

/*
            switch(r->redir) {
                case CMDREDIR_EMPTY:
                    break;
                case CMDREDIR_PIPE:
                    break;
                case CMDREDIR_TRUNCAT:
                    break;
                case CMDREDIR_APPEND:
                    break;
                case CMDREDIR_INPUT:
                    break;
                case CMDREDIR_DINPUT:
                    break;
                default:
                    break;
            }
*/

        }
    }

    ctx_dbmyprintf(1, ctx, M_CMDHANDLE_END_ENGINE);

    //cmdoper_print(mycmdoper);
    cmdoper_free(mycmdoper);
}
