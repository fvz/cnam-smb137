/**
 * \file myprintf.c
 * \brief Ensemble de fonctions d'affichage (verbose/debug).
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "myprintf.h"

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

