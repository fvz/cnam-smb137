/**
 * \file mysh.c
 * \brief mysh : développement d'un shell maison
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "mysh.h"

void signal_handler (int signal) {
	switch (signal) {
		case SIGSEGV:
		case SIGINT:
			printf("\n"); /* for properly ending in current terminal */
			exit(EXIT_FAILURE);
	}
}

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
				cmdline_handle (ctx, ctx->cmd);
				mysh_prompt_release(ctx);
			}
		}
    }

    mysh_history_flush(ctx);
    mysh_context_free(ctx);
	return EXIT_SUCCESS;
}