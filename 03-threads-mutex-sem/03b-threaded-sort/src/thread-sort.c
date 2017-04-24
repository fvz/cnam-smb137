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
#include <pthread.h>

/*
   Ecrivez un programme qui :
   - initialise un grand tableau d’entiers avec des valeurs aleatoires,
   - recherche ensuite le minimum et le maximum du tableau et affiche le resultat.

   Le tableau sera declare en variable globale :
   #define SIZE (int)1e8
   int tab[SIZE];

   Le programme affichera egalement le temps de la recherche du min/max (temps
   d’initialisation non compris). On utilisera la fonction gettimeofday.

   - Proposez une version threadee de ce programme. Le resultat sera ecrit dans
     une ou plusieurs variables globales (une seule variable si une structure est
     utilisee). Testez ce programme avec 2, 4, 8 threads et en augmentant la taille
     du tableau. Que constatez-vous ?

   - Ajoutez ensuite un mutex pour proteger l'acces aux variables globales resultats.

   Remarque : on utilise gettimeofday et non clock car gettimeofday rend le temps
   machine reel alors que clock donne la somme des temps d’execution des threads (principal et fils).

   Remarque 2 : pour la phase de compilation ne pas oublier d'utiliser l'option -lpthread avec gcc.

   TODO : proposer la saisie (fonction scanf) de la taille tableau + nb thread
*/

int *tab = NULL;


typedef struct resultat_s {
	int min;
	int max;
	int b, e; /* begin/end (array range to scan) */
} resultat_t;
typedef resultat_t* resultat_p;


typedef struct mythread_s {
	pthread_t th;
	resultat_t r;
} mythread_t;
typedef mythread_t* mythread_p;


void *search_min_max(void *arg)
{
	mythread_p t = (mythread_p) arg;
	int i;
	for (i=t->r.b; i<t->r.e; i++) {
		if (tab[i] < t->r.min) { t->r.min = tab[i]; }
		if (tab[i] > t->r.max) { t->r.max = tab[i]; }

		/* just for debug */
		#if 0
		if (i%10000 == 0) {
			printf("thread[%li] i=[%d]\n", (unsigned long int) t->th, i);
		}
		#endif
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	int i, len;
	struct timeval t1, t2;
	resultat_t r;
	int ith, nthread = 4;
	mythread_t *mythreads;

	#define SIZE (5*((int)1e8))

	len = SIZE;

	printf("mallocing array of [%d] int elements... ", len);
	if (!(tab = malloc (len * sizeof(int)))) {
		printf("[NOK]\n");
		perror("malloc array of int.");
		return EXIT_SUCCESS;
	}
	printf("[OK]\n");

	printf("Number of threads [%d]: ", nthread);
	// FIXME : si vide (enter), prendre valeur par defaut
	scanf("%d", &nthread);

	//printf("[notice] const: RAND_MAX=[%u] INT_MAX=[%u] UINT_MAX=[%u]\n", RAND_MAX, INT_MAX, UINT_MAX);
	//printf("[notice] sizeof(tab)=[%ld]  size(tab)=[%d elements]\n", sizeof(tab), len);

	srand(time(NULL));
	printf("initialization 'tab' (array of %d elements) ... ", len);
	for (i=0; i<len; i++) { tab[i] = rand(); }
	printf("[OK]\n");


/*** WITHOUT THREAD ****
	r.min = r.max = tab[0];
	r.b = 0; r.e = len;

	printf("searching min & max ... ");
	gettimeofday(&t1, NULL);
	search_min_max(&r);
	for (i=r.b; i<r.e; i++) {
		if (tab[i] < r.min) { r.min = tab[i]; }
		if (tab[i] > r.max) { r.max = tab[i]; }
	}
	gettimeofday(&t2, NULL);
	printf("[OK]\n");
***/


	mythreads = (mythread_t *) malloc (nthread * sizeof(mythread_t));
	gettimeofday(&t1, NULL);

	i=0;
	int len_by_thread = len / nthread;

	/* creation of N threads */
	for(ith=0; ith < nthread; ith++) {
		mythreads[ith].r.b = i;
		i += len_by_thread;
		if (i > len) i = len;
		mythreads[ith].r.e = i;
		mythreads[ith].r.min = mythreads[ith].r.max = tab[mythreads[ith].r.b];

		if (pthread_create(&(mythreads[ith].th), NULL, search_min_max, (void*)&(mythreads[ith])) == -1) {
			perror("pthread_create");
			return EXIT_FAILURE;
		}

		printf("thread[%3d] id=[%li] created.\n", ith, (unsigned long int) mythreads[ith].th);
	}

	/* join of all N threads */
	for(ith=0; ith < nthread; ith++) {
		if (pthread_join(mythreads[ith].th, NULL)) {
			perror("pthread_join");
			return EXIT_FAILURE;
		}
	}

	/* display of min/max found by each threads */
	for(ith=0; ith < nthread; ith++) {
		printf("thread[%3d]: min=[%10d] max=[%10d]\n",
			ith, mythreads[ith].r.min, mythreads[ith].r.max);
	}


	/* search final min/max between the min/max of all threads */
	r.min = mythreads[0].r.min;
	r.max = mythreads[0].r.max;
	for(ith=1; ith < nthread; ith++) {
		if (mythreads[ith].r.min < r.min) { r.min = mythreads[ith].r.min; }
		if (mythreads[ith].r.max > r.max) { r.max = mythreads[ith].r.max; }
	}

	gettimeofday(&t2, NULL);
	free (mythreads);
	free (tab);

	printf("END: min=[%d] max=[%d]\n", r.min, r.max);


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
