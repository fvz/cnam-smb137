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
    if (execlp("ps", "eaux", (char *)NULL) == -1) {
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    if (execlp("grep", "\"^root \"", (char *)NULL) == -1) {
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    // FIXME


    return EXIT_SUCCESS;
}
