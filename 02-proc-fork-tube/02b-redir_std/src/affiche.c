/**
 * \file skeleton.c
 */

#include<stdio.h>
#include<stdlib.h>
// just a hack because mkostemp seems to be undefined in including <stdio.h>
// extern int mkostemp (char *__template, int __flags) __nonnull ((1)) __wur;

#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<libgen.h>

#define STDOUT 1
#define STDERR 2

#define USAGE_SYNTAX "[arg1] [arg2] ... [argN]"

#define PATH_SZ 1024
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
	int cr;

	if (argc<=1) { print_usage(argv[0]); exit(EXIT_SUCCESS); }

	printf("%s\n", argv[1]);

	pere = getpid();
	fils = fork();
	if (fils == 0) {
		int fd_temp, stdout;
		pid_t p;
		char path[PATH_SZ];
		char buffer[BUFFER_SZ];

		p = getpid();
		printf("[FILS] PID=[%d]\n", p);

		stdout = dup(STDOUT);
		close(STDOUT);

		snprintf(path, PATH_SZ, "/tmp/%s_XXXXXX", basename(argv[0]));

		if ((fd_temp = mkstemp(path)) <0) {
			dprintf(STDERR, "error: mkstemp - can't create [%s] file.\n", path);
			exit(EXIT_FAILURE);
		}

		if (dup2(stdout, STDOUT) <0) {
			printf("error: dup2 - can't dup [%d] to [%d]\n", fd_temp, STDOUT);
			exit(EXIT_FAILURE);
		}
		snprintf(buffer, BUFFER_SZ, "[FILS] mkstemp FD=[%d]\n", fd_temp);
		printf("%s", buffer);
		//write(fd_temp, buffer, strlen(buffer));

		close(fd_temp);

	} else {
		cr = 0;
		wait(&cr);
		printf("[PERE] PID=[%d] That's All Folks !\n", pere);
	}

	return EXIT_SUCCESS;
}
