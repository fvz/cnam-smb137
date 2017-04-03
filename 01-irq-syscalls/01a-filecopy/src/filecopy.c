/**
 * \file filecopy.c
 *
 * Copie de fichier par bloc de 1024 octets.
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include<getopt.h>


#define STDOUT 1
#define STDERR 2

#define MAX_PATH_LENGTH 4096


#define USAGE_SYNTAX "[OPTIONS] -i INPUT -o OUTPUT"
#define USAGE_PARAMS "OPTIONS:\n\
  -i, --input  INPUT_FILE  : input file\n\
  -o, --output OUTPUT_FILE : output file\n\
***\n\
  -v, --verbose : enable *verbose* mode\n\
  -h, --help    : display this help\n\
"

/**
 * Procedure which displays binary usage
 * by printing on stdout all available options
 *
 * \return void
 */
void print_usage(char* bin_name)
{
  dprintf(1, "USAGE: %s %s\n\n%s\n", bin_name, USAGE_SYNTAX, USAGE_PARAMS);
}


/**
 * Procedure checks if variable must be free
 * (check: ptr != NULL)
 *
 * \param void* to_free pointer to an allocated mem
 * \see man 3 free
 * \return void
 */
void free_if_needed(void* to_free)
{
  if (to_free != NULL) free(to_free);
}


/**
 *
 * \see man 3 strndup
 * \see man 3 perror
 * \return
 */
char* dup_optarg_str()
{
  char* str = NULL;

  if (optarg != NULL)
  {
    str = strndup(optarg, MAX_PATH_LENGTH);

    // Checking if ERRNO is set
    if (str == NULL)
      perror(strerror(errno));
  }

  return str;
}


/**
 * Binary options declaration
 * (must end with {0,0,0,0})
 *
 * \see man 3 getopt_long or getopt
 * \see struct option definition
 */
static struct option binary_opts[] =
{
  { "help",    no_argument,       0, 'h' },
  { "verbose", no_argument,       0, 'v' },
  { "input",   required_argument, 0, 'i' },
  { "output",  required_argument, 0, 'o' },
  { 0,         0,                 0,  0  }
};

/**
 * Binary options string
 * (linked to optionn declaration)
 *
 * \see man 3 getopt_long or getopt
 */
const char* binary_optstr = "hvi:o:";



/**
 * Binary main loop
 *
 * \return 1 if it exit successfully
 */
int main(int argc, char** argv)
{
  /**
   * Binary variables
   * (could be defined in a structure)
   */
  short int is_verbose_mode = 0;
  char* bin_input_param = NULL;
  char* bin_output_param = NULL;


  int fd_input, fd_output;
  #define BUFFER_SIZE 1024
  char buffer[BUFFER_SIZE+1];
  ssize_t n_read, n_write, sum_read, sum_write;

  // Parsing options
  int opt = -1;
  int opt_idx = -1;

  while ((opt = getopt_long(argc, argv, binary_optstr, binary_opts, &opt_idx)) != -1)
  {
    switch (opt)
    {
      case 'i':
        //input param
        if (optarg)
        {
          bin_input_param = dup_optarg_str();
        }
        break;
      case 'o':
        //output param
        if (optarg)
        {
          bin_output_param = dup_optarg_str();
        }
        break;
      case 'v':
        //verbose mode
        is_verbose_mode = 1;
        break;
      case 'h':
        print_usage(argv[0]);

        free_if_needed(bin_input_param);
        free_if_needed(bin_output_param);

        exit(EXIT_SUCCESS);
      default :
        break;
    }
  }

  /**
   * Checking binary requirements
   * (could defined in a separate function)
   */
  if (bin_input_param == NULL || bin_output_param == NULL)
  {
    dprintf(STDERR, "Bad usage! See HELP [--help|-h]\n");

    // Freeing allocated data
    free_if_needed(bin_input_param);
    free_if_needed(bin_output_param);
    // Exiting with a failure ERROR CODE (== 1)
    exit(EXIT_FAILURE);
  }

  // Printing params
  if (is_verbose_mode == 1) {
    dprintf(STDOUT, "** PARAMS **\n%-8s: %s\n%-8s: %s\n%-8s: %d\n",
            "input",   bin_input_param,
            "output",  bin_output_param,
            "verbose", is_verbose_mode);
  }

  if ((strcmp(bin_input_param, bin_output_param)) == 0) {
    dprintf(STDERR, "Attention, les parametres input et output doivent être différents.\n");
    free_if_needed(bin_input_param);
    free_if_needed(bin_output_param);
    return EXIT_FAILURE;
  }

  if ((fd_input = open(bin_input_param, O_RDONLY)) == -1) {
    dprintf(STDERR, "Erreur Ouverture fichier input [%s]\n", bin_input_param);
    free_if_needed(bin_input_param);
    free_if_needed(bin_output_param);
    return EXIT_FAILURE;
  }

  if ((fd_output = open(bin_output_param, O_RDWR|O_CREAT, 00744)) == -1) {
    dprintf(STDERR, "Erreur Ouverture fichier output [%s]\n", bin_output_param);
    free_if_needed(bin_input_param);
    free_if_needed(bin_output_param);
    close(fd_input);
    return EXIT_FAILURE;
  }

  if (is_verbose_mode == 1)
    dprintf(STDOUT, "** Ouverture Fichiers / Lecture [fd=%d] Ecriture [fd=%d]\n", fd_input, fd_output);


  sum_read = sum_write = 0;
  while ((n_read = read(fd_input, &buffer, (size_t)BUFFER_SIZE)) > 0) {

    buffer[n_read] = '\0';
    sum_read += n_read;

    if (is_verbose_mode == 1)
      dprintf(STDOUT, "** Lecture [%ld] octets\n", n_read);

    n_write = write(fd_output, &buffer, n_read);
    sum_write += n_write;

    if (is_verbose_mode == 1)
      dprintf(STDOUT, "** Ecriture [%ld] octets\n", n_write);
  }


  dprintf(STDOUT, "OK - Octets lus [%ld] / écrits [%ld]\n", sum_read, sum_write);

  if (is_verbose_mode == 1)
    dprintf(STDOUT, "** Fermeture Fichiers **\n");
  if (fd_input != -1) close(fd_input);
  if (fd_output != -1) close(fd_output);

  // Freeing allocated data
  free_if_needed(bin_input_param);
  free_if_needed(bin_output_param);

  return EXIT_SUCCESS;
}
