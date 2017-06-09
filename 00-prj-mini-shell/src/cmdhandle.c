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

	mycmdoper = cmdoper_parser (ctx, cmdline);
	cmdoper_parse_redir (ctx, mycmdoper);


	/* for all cmdoper */
	for(o=mycmdoper; o != NULL; o=o->next) {

		cmdoper_print_one (ctx, o);
		ctx_dbmyprintf(1, ctx, "[cmdhandle] Handling command [%s]\n", o->cmd);

		/* for all cmdredir of each cmdoper */
		for(r=o->redir; r != NULL; r=r->next) {

			char *a = r->args[0]; /* just a shortcut */
			ctx_dbmyprintf(1, ctx, "[cmdhandle] Handling redirection [%s]\n", a);


			if (strcmp(a, "exit") == 0) {
				ctx_dbmyprintf(1, ctx, "[cmdhandle] 'exit' command found.\n");

				if (o->prev) {
					if ((o->prev->oper == CMDOPER_AND) && (o->prev->cmdstatus == true)) {
						ctx_dbmyprintf(1, ctx, "[cmdhandle] Prev Operator=AND and ReturnCode=true. OK for exiting.\n", r->cmd);
						ctx->status = CTX_STATUS_EXIT;
					} else if ((o->prev->oper == CMDOPER_OR) && (o->prev->cmdstatus == false)) {
						ctx_dbmyprintf(1, ctx, "[cmdhandle] Prev Operator=OR and ReturnCode=false. OK for exiting.\n", r->cmd);
						ctx->status = CTX_STATUS_EXIT;
					} else if (o->prev->oper == CMDOPER_SEMICOLON) {
						ctx_dbmyprintf(1, ctx, "[cmdhandle] Prev Operator=SEMICOLON. OK for exiting.\n", r->cmd);
						ctx->status = CTX_STATUS_EXIT;
					} else {
						ctx_dbmyprintf(1, ctx, "[cmdhandle] OK for exiting.\n", r->cmd);
						ctx->status = CTX_STATUS_EXIT;
					}
				} else {
					ctx_dbmyprintf(1, ctx, "[cmdhandle] No Previous Command. OK for exiting.\n", r->cmd);
					ctx->status = CTX_STATUS_EXIT;
				}
			} else {


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


//printf("OK\n");


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

	//cmdoper_print(mycmdoper);
	cmdoper_free(mycmdoper);
}
