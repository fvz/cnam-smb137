/**
 * \file thread-sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <time.h>

/*
   Ecrivez un programme qui :
   - initialise un grand tableau d’entiers avec des valeurs aleatoires,
   - recherche ensuite le minimum et le maximum du tableau et affiche le resultat.

   Le tableau sera declare en variable globale :
   #define SIZE (int)1e8
   int tab[SIZE];

   Le programme affichera egalement le temps de la recherche du min/max (temps
   d’initialisation non compris). On utilisera la fonction gettimeofday.

   - Proposez une version threadee de ce programme. Le resultat sera ecrit dans
     une ou plusieurs variables globales (une seule variable si une structure est
     utilisee). Testez ce programme avec 2, 4, 8 threads et en augmentant la taille
     du tableau. Que constatez-vous ?

   - Ajoutez ensuite un mutex pour proteger l’acces aux variables globales resultats.

   Remarque : on utilise gettimeofday et non clock car gettimeofday rend le temps
   machine reel alors que clock donne la somme des temps d’execution des threads (principal et fils).

   Remarque 2 : pour la phase de compilation ne pas oublier d’utiliser l’option –lpthread avec gcc.

   TODO : idee de proposer la saisie (fonction scanf) de la taille tableau + nb thread
*/

#define SIZE ((int)1e8)
int tab[SIZE];

int main(int argc, char** argv)
{
	int i, len;
	int min, max;
	struct timeval t1, t2;

	len = sizeof(tab)/sizeof(int);

	printf("[notice] const: RAND_MAX=[%u] INT_MAX=[%u] UINT_MAX=[%u]\n", RAND_MAX, INT_MAX, UINT_MAX);
	printf("[notice] sizeof(tab)=[%ld]  size(tab)=[%d elements]\n", sizeof(tab), len);

	printf("initialization 'tab' (array of %d elements) ... ", len);
	srand(time(NULL));
	for (i=0; i < len; i++) {
		tab[i] = rand();
		//printf("%d ", tab[i]);
	}
	printf("[OK]\n");

	min = max = tab[0];

	printf("searching min & max ... ");
	gettimeofday(&t1, NULL);
	for (i=1; i < len; i++) {
		if (tab[i] < min) {
			min = tab[i];
		}
		if (tab[i] > max) {
			max = tab[i];
		}
	}
	gettimeofday(&t2, NULL);
	printf("[OK]\n");


	/* time duration in minutes, seconds, microseconds */
	long int diff_usec, diff_sec, diff_min;

	diff_usec = ((t2.tv_sec - t1.tv_sec) * 1e6) + t2.tv_usec - t1.tv_usec;
	diff_sec = diff_usec/(int)1e6;
	diff_usec %= (int)1e6;
	diff_min = diff_sec/60;
	diff_sec %= 60;

	printf(" |> elapsed:\t%ldm%ld.%03lds\n", diff_min, diff_sec, diff_usec/1000);

	return EXIT_SUCCESS;
}
