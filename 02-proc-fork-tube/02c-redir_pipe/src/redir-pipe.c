/**
 * \file redir-pipe.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

/*
    Écrivez un programme C équivalent au script shell suivant :
        ps eaux | grep "^root " > /dev/null && echo "root est connecté"

    Votre programme devra ainsi prendre en charge les actions suivantes :
      o Réaliser les exécutions de ps et grep en utilisant la primitive execlp dans l’ordre des priorités usuelles.
      o Mettre en place les redirections des entrées/sorties nécessaires grâce à la primitive dup (ou dup2).
      o Réaliser l’affichage final avec la primitive write.

    Vous chercherez à simplifier le code pour ne prendre en charge que cette séquence de commandes.
*/


int main(int argc, char** argv)
{
	int pipefd[2];
	pid_t parent, child1, child2;

	parent = getpid();

	printf("[PARENT-%d] Trying to create a pipe...\n", parent);
	if (pipe(pipefd) <0) {
		perror("pipe");
		dprintf(STDERR, "error: can't create a pipe.\n");
		exit(EXIT_FAILURE);
	}
	printf("[PARENT-%d] Pipe created: pipefd  = { [0]=>%d , [1]=>%d }.\n", parent, pipefd[0], pipefd[1]);

	child1 = fork();
	if (child1 == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT);
		//execlp("/bin/ls", "ls", "-l", "/tmp", (char *)NULL);
		execlp("/bin/ps", "ps", "eaux", (char *)NULL);
	}

	child2 = fork();
	if (child2 == 0) {
		close(pipefd[1]);
		dup2(pipefd[0], STDIN);
		//execlp("tr", "tr", "a-z", "A-Z", (char *)NULL);
		execlp("grep", "grep", "\"root\"", (char *)NULL);
	}

	close(pipefd[0]);
	close(pipefd[1]);

	waitpid(child2, NULL, 0);
	//close(pipefd[1]);
	//dup2(pipefd[0], STDIN);
	//close(pipefd[0]);

	return EXIT_SUCCESS;
}



/*
perror("fork"); dprintf(STDERR, "error: can't fork the program.\n"); exit(EXIT_FAILURE);
perror("fork"); dprintf(STDERR, "error: can't fork the program.\n"); exit(EXIT_FAILURE);
printf("[CHILD#1-%d] Duping [%d] to [%d] ...", selfpid, pipefd[1], STDOUT);
printf("[CHILD#2-%d] Duping [%d] to [%d] ...", selfpid, pipefd[0], STDIN);
if (dup2(pipefd[1], STDOUT) <0) {
perror("dup2");
dprintf(STDERR, "error: dup2 - can't dup [%d] to [%d]\n", pipefd[1], STDOUT);
exit(EXIT_FAILURE);
}

if (dup2(pipefd[0], STDIN) <0) {
perror("dup2");
dprintf(STDERR, "error: dup2 - can't dup [%d] to [%d]\n", pipefd[0], STDIN);
exit(EXIT_FAILURE);
}

int cr = -1;
close(pipefd[0]);
close(pipefd[1]);
printf("[PARENT-%d] Waiting...\n", parent);
wait(&cr);
printf("[PARENT-%d] Wait ends.\n", parent);
*/
