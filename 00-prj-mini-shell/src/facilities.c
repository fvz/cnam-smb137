/**
 * \file facilities.c
 * \brief Ensemble de fonctions diverses apportant quelques facilités de code.
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "facilities.h"


/**
 * Procedure checks if variable must be free
 * (check: ptr != NULL)
 *
 * \param void* to_free pointer to an allocated mem
 * \see man 3 free
 * \return void
 */
void freeif(void* to_free)
{
    if (to_free != NULL) free(to_free);
}

/**
 * \fn char* strcat_dup(const char *s1, const char *s2)
 * \brief concatène 2 chaines et duplique en mémoire
 *
 * \param s1 chaine n°1
 * \param s2 chaine n°1
 * \return nouvelle chaine allouée contenant la concaténation de s1 et s2.
 */
char* strcat_dup(const char *s1, const char *s2) {
    int len1, len2;
    char *new;
    len1 = str1 ? strlen(s1) : 0;
    len2 = str2 ? strlen(s2) : 0;
    new = malloc(len1 + len2 + 1); *new = '\0';
    if (s1) { memcpy(new,        s1, len1 + 1); }
    if (s2) { memcpy(new + len1, s2, len2 + 1); }
    return new;
}
