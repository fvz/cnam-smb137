/**
 * \file skeleton.c
 */

#include <stdio.h>
#include <stdlib.h>
// just a hack because mkostemp seems to be undefined in including <stdio.h>
// extern int mkostemp (char *__template, int __flags) __nonnull ((1)) __wur;

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <libgen.h>
//#include <sys/syslimits.h>
#include <limits.h>

#define STDOUT 1
#define STDERR 2

#define USAGE_SYNTAX "[arg1] [arg2] ... [argN]"

#define BUFFER_SZ 1024


/*
Realisez l'exercice suivant en realisant les etapes dans l'ordre specifie

1. Ecrivez un programme affiche qui affiche un message sur la sortie standard, dans ce message doit figurer le premier mot passe sur la ligne de commande apres le nom du programme) (argv[1]) .
On s'assurera que le bon nombre de parametres est passe sur la ligne de commande.

2. Faites evoluer le programme de sorte a ce qu'il cree un fils qui :
	. Affiche son PID.
	. Ferme le descripteur 1 (-STDOUT) (close(1)).
	. Ouvre en creation et ecriture le fichier temporaire /tmp/proc-exercise (cf mkstemp).
	. Affiche le numero du descripteur du fichier ouvert. (Possible avec la methode dup2)
	. Execute le programme affiche par son pere lors de l'etape Numero 1 (argv[1]) (cf execXXX).
	. Le pere quant a lui :
	. Affiche son PID.
	. Attend la fin du fils.
	. Affiche un message quelconque avant de terminer (ie : "That's All Folks !").

3. (*) Examinez le contenu de /tmp/proc-exercise, recommencer en fermant cette fois le descripteur 2. (Laissant le descripteur 1 ouvert)
(*)facultatif
*/


void print_usage(char* bin_name)
{
	dprintf(STDOUT, "USAGE: %s %s\n", bin_name, USAGE_SYNTAX);
}

#define __USE_GNU 1

int main(int argc, char** argv)
{
	pid_t pere, fils;

	if (argc<=1) { print_usage(argv[0]); exit(EXIT_SUCCESS); }

	printf("%s\n", argv[1]);

	pere = getpid();
	fils = fork();
	if (fils == 0) {
		int fd_temp, fd_stdout;
		char path[PATH_MAX];
		char buffer[BUFFER_SZ];

		printf("[FILS] PID=[%d]\n", getpid());

		fd_stdout = dup(STDOUT);
		close(STDOUT);

		snprintf(path, PATH_MAX, "/tmp/%s_XXXXXX", basename(argv[0]));

		if ((fd_temp = mkstemp(path)) <0) {
			perror("mkstemp");
			dprintf(STDERR, "error: mkstemp - can't create [%s] file.\n", path);
			exit(EXIT_FAILURE);
		}

		// delete file when the fd is closed or the program ends.
		unlink(path);

		if (dup2(fd_stdout, STDOUT) <0) {
			perror("dup2");
			dprintf(STDERR, "error: dup2 - can't dup [%d] to [%d]\n", fd_temp, STDOUT);
			exit(EXIT_FAILURE);
		}

		snprintf(buffer, BUFFER_SZ, "[FILS] mkstemp FD=[%d]\n", fd_temp);
		printf("%s", buffer);

		if (execvp(argv[1], argv+1) == -1) {
			perror("execv");
			exit(EXIT_FAILURE);
		}

		close(fd_temp);


	} else {
		int cr = -1;
		wait(&cr);
		printf("[PERE] PID=[%d] That's All Folks ! (son's CR=[%d])\n", pere, cr);
	}

	return EXIT_SUCCESS;
}
