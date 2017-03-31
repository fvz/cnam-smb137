/**
 * \file skeleton.c
 */

#include<stdio.h>
#include<stdlib.h>
// just a hack because mkostemp seems to be undefined in including <stdio.h>
extern int mkostemp (char *__template, int __flags) __nonnull ((1)) __wur;

#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>

#define STDOUT 1
#define STDERR 2

#define USAGE_SYNTAX "[arg1] [arg2] ... [argN]"

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

	if (argc<=1) {
		print_usage(argv[0]);
		exit(EXIT_SUCCESS);
	}

	printf("%s\n", argv[1]);


	pere = getpid();
	fils = fork();
	if (fils == 0) {
		int fd_temp;
		pid_t p;
		char *path = "/tmp/XXXXXX-exercice";

		p = getpid();
		printf("[FILS] PID=[%d]\n", p);
		//close(STDOUT);

		if ((fd_temp = mkostemp(path, O_CREAT|O_RDWR)) < 0)
			exit(EXIT_FAILURE);


		write(fd_temp, "coucou", 6);

		if (dup2(fd_temp, STDOUT) < 0)
			exit(EXIT_FAILURE);

		printf("fd_temp=[%d]\n", fd_temp);
		close(fd_temp);

	} else {
		cr = 0;
		wait(&cr);
		printf("[PERE] PID=[%d] That's All Folks !\n", pere);
	}

	return EXIT_SUCCESS;
}
