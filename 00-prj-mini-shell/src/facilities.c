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
