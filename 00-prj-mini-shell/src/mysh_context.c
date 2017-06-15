/**
 * \file mysh_context.c
 * \brief Gestion du contexte de session
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "mysh_context.h"

#define USAGE_SYNTAX "[OPTIONS]"
#define USAGE_PARAMS "OPTIONS:\n\
  -c, --command=COMMANDS   execute specified command(s).\n\
  -d, --debug              display some debug messages.\n\
  -v, --verbose            display some verbose messages.\n\
 For debug/verbose argument, you can increase the level by\n\
 repeat the arguments (Ex:  -dd, -vvv).\n\
***\n\
  -h, --help               display this help\n\
"

/**
 * \fn void print_usage(char* bin_name)
 * \brief Affichage de l'usage
 *
 * \param bin_name nom du binaire de l'executablt
 */
void print_usage(char* bin_name)
{
	printf("USAGE: %s %s\n\n%s\n", bin_name, USAGE_SYNTAX, USAGE_PARAMS);
}

struct option binary_opts[] =
{
	{ "command",   required_argument,  0,  'c' },
	{ "debug",     no_argument,        0,  'd' },
	{ "verbose",   no_argument,        0,  'v' },
	{ "help",      no_argument,        0,  'h' },
	{ 0,           0,                  0,  0 }
};
const char* binary_optstr = "c:dvh";

/**
 * \fn void mysh_context_init (mysh_context_p ctx, int argc, char **argv)
 * \brief Initialiseur d'objet mysh_context_p
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param argc argc du main
 * \param argv argv du main
 */
void mysh_context_init (mysh_context_p ctx, int argc, char **argv) {

    mysh_prompt_set(ctx, MYSH_DEFAULT_PROMPT);

    ctx->argc = argc;
    ctx->argv = argv;

    ctx->cmd = NULL;
    ctx->debug_level = 0;
    ctx->verbose_level = 0;
    ctx->history = (mysh_history_p)NULL;
    mysh_history_new(ctx);

    ctx->status = CTX_STATUS_LOOP;
    ctx->exitstatus = EXIT_SUCCESS;

    ctx->previous_dir = NULL;

    ctx->alias = NULL;
    ctx->nalias = 0;

    cmdalias_list_init(ctx);
}

/**
 * \fn mysh_context_p mysh_context_new (int argc, char** argv)
 * \brief Constructeur d'objet mysh_context_p
 *
 * \param argc argc du main
 * \param argv argv du main
 * \return mysh_context_p retourne le pointeur sur nouvel élément alloué
 */
mysh_context_p mysh_context_new (int argc, char** argv) {

    mysh_context_p ctx = (mysh_context_p) malloc (sizeof(mysh_context_t));
    mysh_context_init(ctx, argc, argv);

    /* parsing options */
	int opt = -1;
	int opt_idx = -1;

	while ((opt = getopt_long(argc, argv, binary_optstr, binary_opts, &opt_idx)) != -1)
	{
		switch (opt)
		{
			case 'c':
				if (optarg) { ctx->cmd = strndup (optarg, strlen(optarg)); }
				break;
			case 'd':
				ctx->debug_level++;
				break;
			case 'v':
				ctx->verbose_level++;
				break;
			case 'h':
				print_usage(argv[0]);
				exit(EXIT_SUCCESS);
			default :
				break;
		}
	}

    return ctx;
}

/**
 * \fn void mysh_context_free (mysh_context_p ctx)
 * \brief Destructeur d'objet mysh_context_p
 *
 * \param ctx Pointeur sur le contexte mysh_context
 */
void mysh_context_free (mysh_context_p ctx) {
    if (ctx != NULL) {
        mysh_prompt_free(ctx);
        freeif(ctx->cmd);
        freeif(ctx->previous_dir);
        mysh_history_free(ctx);
        free(ctx);
    }
}
