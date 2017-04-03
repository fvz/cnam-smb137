/**
 * \file printreverse.c
 *
 * Utilitaire ls "like"
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include<getopt.h>


#define STDOUT 1
#define STDERR 2

#define MAX_PATH_LENGTH 4096


#define USAGE_SYNTAX "[DIR_or_FILE1] [DIR_or_FILE2] ... [DIR_or_FILE-N]"


/**
 * Procedure which displays binary usage
 * by printing on stdout all available options
 *
 * \return void
 */
void print_usage(char* bin_name)
{
  dprintf(1, "USAGE: %s %s\n", bin_name, USAGE_SYNTAX);
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





int stat_and_print(char *fpath, char *name)
{
  struct stat s;
  char c = '?';
  char perm[10];
  struct group *grp;
  struct passwd *pwd;
  struct tm *t;

  if (stat(fpath, &s) == -1)
    return EXIT_FAILURE;

  switch(s.st_mode & S_IFMT) {
    case S_IFBLK:  c='b'; break;
    case S_IFCHR:  c='c'; break;
    case S_IFDIR:  c='d'; break;
    case S_IFIFO:  c='f'; break;
    case S_IFLNK:  c='l'; break;
    case S_IFREG:  c='-'; break;
    case S_IFSOCK: c='s'; break;
    default:       c='?'; break;
  }

  sprintf(perm, "%c%c%c%c%c%c%c%c%c", 
    (s.st_mode & S_IRUSR) ? 'r' : '-',
    (s.st_mode & S_IWUSR) ? 'w' : '-',
    (s.st_mode & S_IXUSR) ? 'x' : '-',
    (s.st_mode & S_IRGRP) ? 'r' : '-',
    (s.st_mode & S_IWGRP) ? 'w' : '-',
    (s.st_mode & S_IXGRP) ? 'x' : '-',
    (s.st_mode & S_IROTH) ? 'r' : '-',
    (s.st_mode & S_IWOTH) ? 'w' : '-',
    (s.st_mode & S_IXOTH) ? 'x' : '-'          
    );
  perm[9] = '\0';

  grp = getgrgid(s.st_gid);
  pwd = getpwuid(s.st_uid);

  t = localtime(&(s.st_mtime));

  dprintf(STDOUT, "%c%s %-10s %-10s %10ld %2d/%02d/%4d %02d:%02d %-20s\n",
    c, perm,
    pwd->pw_name, grp->gr_name,
    (long)s.st_size,
    t->tm_mday, t->tm_mon, t->tm_year-100+2000,
    t->tm_hour, t->tm_min,
    name
  );

  return 1;
}





/**
 * Binary main loop
 *
 * \return 1 if it exit successfully
 */
int main(int argc, char** argv)
{
  int i;
  int len_path, len_dname;

  if (argc <= 1) {
    print_usage(argv[0]);
    exit(EXIT_SUCCESS);
  }


  for (i=1; i < argc; i++)
  {
    struct stat s;
    char *path = argv[i];

    if (stat(path, &s) == -1) {
      perror(path);
      exit(EXIT_SUCCESS);
    }

    switch (s.st_mode & S_IFMT) {

      case S_IFREG:
        printf("[%s]\n", path);
        stat_and_print(".", &(argv[i]));
        break;

      case S_IFDIR:
        printf("[%s]\n", path);

        DIR *dir;
        struct dirent *dp;

        len_path = strlen(path);
        dir = opendir(path);
        while ((dp = readdir(dir)) != NULL) {

          // Attention, dp->d_name ne contient pas le chemin absolu.
          // Pour utiliser stat(), il faut concaténer le path + '/' + d_name

          char *fpath;
          len_dname = strlen(dp->d_name);
          fpath = (char*)malloc((len_path + len_dname + 1 + 1) * sizeof(char));   // path + '/' + dname + '\0'
          strncpy(fpath, path, len_path);
          *(fpath + len_path) = '/';
          strncpy(fpath + len_path + 1, dp->d_name, len_dname);
          *(fpath + len_path + 1 + len_dname + 1) = '\0';

          stat_and_print(fpath, &dp->d_name);

          free(fpath);
        }
        closedir(dir);
        dprintf(STDOUT, "\n");
        break;
    }
  }

  return EXIT_SUCCESS;
}
