/**
 * \file forkyourself.c
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define STDOUT 1
#define STDERR 2

/*
Le processus fils doit 
	afficher son numeo (PID) celui de son pere a l'aide methodes getpid et getppid,
	puis sort exit avec un code de retour egal au dernier chiffre du PID.

Le processus pere
	affiche le PID du fils,
	puis attend sa terminaison via wait
	et affiche le code de retour son fils.
*/

int main(int argc, char** argv)
{
	pid_t pere, fils;
	int cr;

	pere = getpid();
	fils = fork();

	if (fils == 0) {
		pid_t p = getpid();
		cr = p%10;
		printf("[FILS]  PID=[%d]  PPID=[%d]  CR=[%d]\n", p, pere, cr);
		return cr;
	} else {
		cr = 0;
		printf("[PERE]  PID-fils=[%d]  --> attente terminaison (wait)...\n", fils);

		wait(&cr);
		printf("[PERE]  PID-fils=[%d] CR=[%d]\n", fils, cr>>8);
	}

	return EXIT_SUCCESS;
}
