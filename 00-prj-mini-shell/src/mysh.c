/**
 * \file mysh.c
 * \brief mysh : développement d'un shell maison
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "mysh.h"

/**
 * \fn void signal_handler (int signal)
 * \brief Pour catcher les signaux SEGV et INT
 *
 * \param signal signal
 */
void signal_handler (int signal) {
	switch (signal) {
		case SIGSEGV:
		case SIGINT:
			printf("\n"); /* for properly ending in current terminal */
			exit(EXIT_FAILURE);
	}
}

/**
 * \fn int main(int argc, char** argv)
 * \brief Fonction principale
 *
 * \param argc argc
 * \param argv argv
 * \return int exit status
 */
int main(int argc, char** argv)
{
    mysh_context_p ctx = mysh_context_new (argc, argv);

    if (ctx->cmd) {
		cmdline_handle(ctx, ctx->cmd);
    } else {
    	signal(SIGSEGV, signal_handler);
    	signal(SIGINT, signal_handler);

		while (ctx->status != CTX_STATUS_EXIT) {
			mysh_prompt_print(ctx);
			if (mysh_prompt_catch(ctx)) {
				cmdline_handle(ctx, ctx->cmd);
				mysh_prompt_release(ctx);
			}
		}
		ctx_dbmyprintf(1, ctx, "[main] End of main loop (ctx = CTX_STATUS_EXIT).\n");
    }

    mysh_history_flush(ctx);
    mysh_context_free(ctx);
	return ctx->exitstatus;
}
