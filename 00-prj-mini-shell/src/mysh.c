/**
 * \file mysh.c
 */

#include "mysh.h"

/**E*N*O*N*C*E***P*R*O*J*E*C*T************************************************
*****************************************************************************/


#define USAGE_SYNTAX "[OPTIONS]"
#define USAGE_PARAMS "OPTIONS:\n\
  -c, --command  VALUE : execute specified command.\n\
***\n\
  -h, --help        : display this help\n\
"

#define BUFFER_SIZE 1024
#define PROMPT "Prompt>" /* TODO : ability to customize ? (arg cmdline or with rc file) */

void print_usage(char* bin_name)
{
	printf("USAGE: %s %s\n\n%s\n", bin_name, USAGE_SYNTAX, USAGE_PARAMS);
}

struct option binary_opts[] =
{
	{ "command",	required_argument,	0,	'c' },
	{ "debug",	no_argument,		0,	'd' },
	{ "verbose",	no_argument,		0,	'v' },
	{ "help",	no_argument,		0,	'h' },
	{ 0,		0,			0,	0 }
};
const char* binary_optstr = "c:dvh";


void signal_handler (int signal) {

	switch (signal) {
		case SIGSEGV:
		case SIGINT:
			printf("\n"); /* for properly ending in current terminal */
			exit(EXIT_FAILURE);
	}
}

/* TODO : debug and verbose levels */
void dbprintf (/*ctx, verbose_reach, debug_reach */ const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);

	/*
	if ((ctx->verbose_level > verbose_reach) || (ctx->debug_level > debug_reach)) {
		vfprintf(stderr, fmt, va);
	}
	*/

	vfprintf(stderr, fmt, va);
	va_end(va);
}

int main(int argc, char** argv)
{
	/* TODO : store this 'context' in structure */
	char *command = NULL;
	int debug_level = 0;
	int verbose_level = 0;


	/* parsing options */
	int opt = -1;
	int opt_idx = -1;


	while ((opt = getopt_long(argc, argv, binary_optstr, binary_opts, &opt_idx)) != -1)
	{
		switch (opt)
		{
			case 'c':
				if (optarg) { command = strndup (optarg, strlen(optarg)); }
				break;
			case 'd':
				debug_level++;
				break;
			case 'v':
				verbose_level++;
				break;
			case 'h':
				print_usage(argv[0]);
				exit(EXIT_SUCCESS);
			default :
				break;
		}
	}

	signal(SIGSEGV, signal_handler);
	signal(SIGINT, signal_handler);


	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	do {
		printf("%s ", PROMPT);
		if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
			char *pos = NULL;
			if ((pos = strchr(buffer, '\n'))) { *pos = '\0'; }

			if (verbose_level > 0) {
				printf("Parsing [%s] command...\n", buffer);	
			}
		}
	} while (strcmp(buffer, "exit") != 0);



	if (command) {
		free(command);
	}

	return EXIT_SUCCESS;
}
