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
#include <getopt.h>

/**E*N*O*N*C*E***E*X*E*R*C*I*C*E**********************************************
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
*****************************************************************************/


/* dynamic array prefered for user defined parameter (via getopt / command line) */
int *tab = NULL;

/* default values */
#define DEFAULT_ARRAY_SIZE	((int)1e8)
#define DEFAULT_THREADS_NUMBER	(4)

#define BUFFER_SIZE 1024


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

		#if 0
		if (i%10000 == 0) { /* just for debug */
			printf("thread[%li] i=[%d]\n", (unsigned long int) t->th, i);
		}
		#endif
	}
	pthread_exit(NULL);
}


#define USAGE_SYNTAX "[OPTIONS]"
#define USAGE_PARAMS "OPTIONS:\n\
  -a, --array  LENGTH_OF_ARRAY    : length of random array.\n\
  -t, --thread NUMBER_OF_THREADS  : number of threads to create.\n\
***\n\
  -i, --interactive : enable *interactive* mode\n\
  -v, --verbose     : enable *verbose* mode\n\
  -h, --help        : display this help\n\
"

void print_usage(char* bin_name)
{
	printf("USAGE: %s %s\n\n%s\n", bin_name, USAGE_SYNTAX, USAGE_PARAMS);
}

struct option binary_opts[] =
{
	{ "help",	no_argument,		0,	'h' },
	{ "verbose",	no_argument,		0,	'v' },
	{ "interactive",no_argument,		0,	'i' },
	{ "array",	required_argument,	0,	'a' },
	{ "thread",	required_argument,	0,	't' },
	{ 0,		0,			0,	0 }
};
const char* binary_optstr = "hvia:t:";


int main(int argc, char** argv)
{
	int is_verbose_mode = 0;
	int is_interactive_mode = 0;
	int i, len;
	int ith, nthread;
	struct timeval t1, t2;
	resultat_t r;
	mythread_t *mythreads;

	/* use default value if not specified in args or in interactive mode */
	len = DEFAULT_ARRAY_SIZE;
	nthread = DEFAULT_THREADS_NUMBER;

	/* parsing options */
	int opt = -1;
	int opt_idx = -1;

	while ((opt = getopt_long(argc, argv, binary_optstr, binary_opts, &opt_idx)) != -1)
	{
		switch (opt)
		{
			case 'a':
				if (optarg) { len = atoi(optarg); }
				break;
			case 't':
				if (optarg) { nthread = atoi(optarg); }
				break;
			case 'i':
				is_interactive_mode = 1;
				break;
			case 'v':
				is_verbose_mode = 1;
				break;
			case 'h':
				print_usage(argv[0]);
				exit(EXIT_SUCCESS);
			default :
				break;
		}
	}

	/* interactive mode : scanning the stdin for retrieving values parameters */
	if (is_interactive_mode) {

		char buffer[BUFFER_SIZE];

		printf("Length of array [%d]: ", len);
		if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
			char *pos = NULL;
			if ((pos = strchr(buffer, '\n'))) { *pos = '\0'; }
			if (strlen(buffer) > 0) { len = atoi(buffer); }
		}

		printf("Number of threads [%d]: ", nthread);
		if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
			char *pos = NULL;
			if ((pos = strchr(buffer, '\n'))) { *pos = '\0'; }
			if (strlen(buffer) > 0) { nthread = atoi(buffer); }
		}
	}


	/* check len/nthread values (because atoi() can return 0 if string isn't a number) */
	if (len <= 0) { len = DEFAULT_ARRAY_SIZE; }
	if (nthread <= 0) { nthread = DEFAULT_THREADS_NUMBER; }


	/* allocating array of 'len' int elements */
	if (is_verbose_mode) {	printf("mallocing array of [%d] int elements... ", len); }
	if (!(tab = malloc (len * sizeof(int)))) {
		if (is_verbose_mode) { printf("[NOK]\n"); }
		perror("malloc array of int.");
		return EXIT_FAILURE;
	}
	if (is_verbose_mode) { printf("[OK]\n"); }


	/* initialization of array with random values */
	srand(time(NULL));
	if (is_verbose_mode) { printf("initialization 'tab' (array of %d elements) ... ", len); }
	for (i=0; i<len; i++) { tab[i] = rand(); }
	if (is_verbose_mode) { printf("[OK]\n"); }


	/*** WITHOUT THREAD **************************************************
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
	*********************************************************************/


	/* allocating array of 'nthread' elements for handling the N threads */
	if (!(mythreads = (mythread_t *) malloc (nthread * sizeof(mythread_t)))) {
		perror("malloc array of threads.");
		free(tab);
		return EXIT_FAILURE;
	}


	/* start time counter */
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

		if (is_verbose_mode) {
			printf("thread[%3d] id=[%li] created.\n", ith, (unsigned long int) mythreads[ith].th);
		}
	}

	/* join of all N threads */
	for(ith=0; ith < nthread; ith++) {
		if (pthread_join(mythreads[ith].th, NULL)) {
			perror("pthread_join");
			return EXIT_FAILURE;
		}
	}

	/* display of min/max found by each threads */
	if (is_verbose_mode) {
		for(ith=0; ith < nthread; ith++) {
			printf("thread[%3d]: min=[%10d] max=[%10d]\n",
				ith, mythreads[ith].r.min, mythreads[ith].r.max);
		}
	}

	/* search final min/max values between the min/max of all threads */
	r.min = mythreads[0].r.min;
	r.max = mythreads[0].r.max;
	for(ith=1; ith < nthread; ith++) {
		if (mythreads[ith].r.min < r.min) { r.min = mythreads[ith].r.min; }
		if (mythreads[ith].r.max > r.max) { r.max = mythreads[ith].r.max; }
	}

	/* stop time counter */
	gettimeofday(&t2, NULL);

	/* freeing allocated arrays */
	free (mythreads);
	free (tab);


	printf("Scanning Array[%d] / With [%d] Threads / Give min/max [%d/%d] values", len, nthread, r.min, r.max);

	/* time duration in minutes, seconds, microseconds */
	long int diff_usec, diff_sec, diff_min;

	diff_usec = ((t2.tv_sec - t1.tv_sec) * 1e6) + t2.tv_usec - t1.tv_usec;
	diff_sec = diff_usec/(int)1e6;
	diff_usec %= (int)1e6;
	diff_min = diff_sec/60;
	diff_sec %= 60;

	printf(" / In Elapsed Time [%ldm%ld.%03lds]\n", diff_min, diff_sec, diff_usec/1000);

	return EXIT_SUCCESS;
}
