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



void vmyprintf_stream(FILE * stream, int threshold, int level, const char *format, va_list args) {
    if (level >= threshold) { vfprintf(stream, format, args); }
}

void myprintf_stream(FILE * stream, int threshold, int level, const char *format, ...) {
	va_list args;
	va_start(args, format);
    vmyprintf_stream(stream, threshold, level, format, args);
	va_end(args);
}

void ctx_myprintf_stream(FILE * stream, int threshold, mysh_context_p ctx, const char *format, ...) {
    va_list args;
	va_start(args, format);
    vmyprintf_stream(stream, threshold, ctx->verbose_level, format, args);
    va_end(args);
}

void ctx_dbmyprintf_stream(FILE * stream, int threshold, mysh_context_p ctx, const char *format, ...) {
    va_list args;
	va_start(args, format);
    vmyprintf_stream(stream, threshold, ctx->debug_level, format, args);
    va_end(args);
}
