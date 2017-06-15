/**
 * \file myprintf.c
 * \brief Ensemble de fonctions d'affichage (verbose/debug).
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "myprintf.h"

/**
 * \fn void vmyprintf_stream(FILE * stream, int threshold, int level, const char *format, va_list args)
 * \brief Fonction d'affichage à paramètre variable
 *
 * \param stream stream où écrire
 * \param threshold Seuil que le level de verbosité doit atteindre pour que la chaîne soit bien affichée
 * \param level level de verbosité de la session
 * \param format paramétrage variable
 * \param args argument du paramétrage variable
 */
void vmyprintf_stream(FILE * stream, int threshold, int level, const char *format, va_list args) {
    if (level >= threshold) { vfprintf(stream, format, args); }
}

/**
 * \fn void myprintf_stream(FILE * stream, int threshold, int level, const char *format, ...)
 * \brief Fonction d'affichage à paramètre variable
 *
 * \param stream stream où écrire
 * \param threshold Seuil que le level de verbosité doit atteindre pour que la chaîne soit bien affichée
 * \param level level de verbosité de la session
 * \param format paramétrage variable
 */
void myprintf_stream(FILE * stream, int threshold, int level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vmyprintf_stream(stream, threshold, level, format, args);
    va_end(args);
}

/**
 * \fn void ctx_myprintf_stream(FILE * stream, int threshold, mysh_context_p ctx, const char *format, ...)
 * \brief Fonction d'affichage (verbose) à paramètre variable
 *
 * \param stream stream où écrire
 * \param threshold Seuil que le level de verbosité doit atteindre pour que la chaîne soit bien affichée
 * \param ctx Pointeur sur le contexte mysh_context
 * \param format paramétrage variable
 */
void ctx_myprintf_stream(FILE * stream, int threshold, mysh_context_p ctx, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vmyprintf_stream(stream, threshold, ctx->verbose_level, format, args);
    va_end(args);
}

/**
 * \fn void ctx_dbmyprintf_stream(FILE * stream, int threshold, mysh_context_p ctx, const char *format, ...)
 * \brief Fonction d'affichage (debug) à paramètre variable
 *
 * \param stream stream où écrire
 * \param threshold Seuil que le level de verbosité doit atteindre pour que la chaîne soit bien affichée
 * \param ctx Pointeur sur le contexte mysh_context
 * \param format paramétrage variable
 */
void ctx_dbmyprintf_stream(FILE * stream, int threshold, mysh_context_p ctx, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vmyprintf_stream(stream, threshold, ctx->debug_level, format, args);
    va_end(args);
}
