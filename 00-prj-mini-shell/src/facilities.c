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



/* TODO : debug and verbose levels */
void dbprintf (/*ctx, verbose_reach, debug_reach */ const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	/*
	if ((ctx->verbose_level > verbose_reach) || (ctx->debug_level > debug_reach)) {
		vfprintf(stderr, fmt, va);
	}
	*/
	vfprintf(stderr, fmt, va);
	va_end(va);
}
