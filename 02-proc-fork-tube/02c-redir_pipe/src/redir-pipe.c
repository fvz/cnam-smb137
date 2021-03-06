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
	int devnull_fd = -1;
	int status = 0;
	pid_t parent, child1, child2;


	parent = getpid();

	printf("[PARENT-%d] Trying to create a pipe...\n", parent);
	if (pipe(pipefd) <0) {
		perror("pipe");
		dprintf(STDERR, "error: can't create a pipe.\n");
		exit(EXIT_FAILURE);
	}
	printf("[PARENT-%d] Pipe created : pipefd[0,1]=(%d,%d)\n", parent, pipefd[0], pipefd[1]);

	printf("[PARENT-%d] Open /dev/null for STDOUT redirection.\n", parent);
	if ((devnull_fd = open("/dev/null", O_WRONLY)) == -1) {
		perror("open");
		dprintf(STDERR, "error: can't open /dev/null.\n");
		exit(EXIT_FAILURE);
	}

	child1 = fork();
	if (child1 <0) {
		perror("fork");
		dprintf(STDERR, "error: can't fork the program.\n");
		exit(EXIT_FAILURE);
	}
	if (child1 == 0) {
		printf("[CHILD-%d] Closing pipefd[0] and duping STDOUT to pipefd[1].\n", getpid());
		close(pipefd[0]); // child1 (ps) : unused side (in) of pipe
		dup2(pipefd[1], STDOUT); // Connexion : child stdout -> write side of pipe
		//execlp("/bin/ls", "ls", "-l", "/tmp/", (char *)NULL);
		execlp("/bin/ps", "ps", "eaux", (char *)NULL);
		perror("execlp ps");
		exit(1);
	}

	child2 = fork();
	if (child2 <0) {
		perror("fork");
		dprintf(STDERR, "error: can't fork the program.\n");
		exit(EXIT_FAILURE);
	}
	if (child2 == 0) {
		printf("[CHILD-%d] Closing pipefd[1] and duping STDIN to pipefd[0].\n", getpid());
		close(pipefd[1]); // child2 (grep) : unused side (out) of pipe
		dup2(devnull_fd, STDOUT); // redirection of stdout to /dev/null
		dup2(pipefd[0], STDIN); // connexion : child stdin -> read side of pipe
		//execlp("tr", "tr", "a-z", "A-Z", (char *)NULL);
		execlp("grep", "grep", "\"^root \"", (char *)NULL);
		perror("execlp grep");
		exit(1);
	}
	else {
		printf("[PARENT-%d] Closing /dev/null (FD=[%d]) and pipe[0,1]=(%d,%d)\n",
			parent, devnull_fd, pipefd[0], pipefd[1]);

		close(devnull_fd);
		close(pipefd[0]);
		close(pipefd[1]);

		printf("[PARENT-%d] Waiting Child2-%d ...\n", parent, child2);
		waitpid(child2, &status, 0);

		printf("[PARENT-%d] End of [Child-%d]. Status=[%d].\n", parent, child2, status);

		/* condition '&&' => return code of child = 0 */
		if (WEXITSTATUS(status) == 0) {
			write(STDOUT, "root est connecté.\n", 20);
		}
	}

	return EXIT_SUCCESS;
}
