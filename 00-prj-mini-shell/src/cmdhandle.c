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
	cmdoper_p mycmdoper;
	mysh_history_add(ctx, cmdline);
	mycmdoper = cmdoper_parse (cmdline);
	cmdoper_parse_redir (mycmdoper);

	/* for all cmdoper */
	cmdoper_p o;
	for(o=mycmdoper; o != NULL; o=o->next) {

		printf("cmd[%s]\n", o->cmd);

		/* for all cmdredir of one cmdoper */
		cmdredir_p r;
		for(r=o->redir; r != NULL; r=r->next) {
			printf("red[%s]\n", o->cmd);

			if (strcmp(cmdline, "exit") == 0) {
				ctx->status = CTX_STATUS_EXIT;
			}
		}
	}

	//cmdoper_print(mycmdoper);
	cmdoper_free(mycmdoper);
}
