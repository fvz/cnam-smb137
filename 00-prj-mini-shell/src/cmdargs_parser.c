/**
 * \file cmdargs_parser.c
 * \brief Parser d'arguments d'une commande
 * \author Fabien VANTARD
 * \version 0.1
 */
#include "typedef.h"
#include "cmdargs_parser.h"

/**
 * \fn void cmdargs_free (char ***array)
 * \brief Destructeur de tableau d'arguments (array = char **)
 *
 * \param array pointeur sur tableau d'arguments char* à libérer
 */
void cmdargs_free (char ***array) {
    int i;
    for (i=0; (*array)[i] != NULL; i++) {
        free((*array)[i]);
    }
    free (*array);
    (*array) = NULL;
}

/**
 * \fn int cmdargs_parser (mysh_context_p ctx, char *str, char ***array)
 * \brief Parser d'argument dans une commande ligne (séparateur = ' ')
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param str Chaine contenant la commande line à analyser
 * \param array Pointeur sur le futur tableau nouvellement alloué par la fonct.
 * \return nombre d'arguments trouvés dans la commande ligne
 *
 * Cette fonction parse une chaine de caractère contenant la commande line
 * afin de trouver l'ensemble des arguments (les single/double quotes sont
 * bien interprêtées. Exemple: "a b" vaut pour UN argument).
 *
 * Les arguments trouvés sont alloués dans un tableau de char*. Chaque argument
 * est 'strndupé' dans chq élément du tableau. A chaque nouvel argument trouvé,
 * le tableau est réalloqué (fonction realloc). La fin du tableau est indiquée
 * avec une valeur NULL pour le dernier élément.
 *
 */
int cmdargs_parser (mysh_context_p ctx, char *str, char ***array) {

    int ielt, nelt;

    int b_escape = false;
    int b_singleq = false;
    int b_doubleq = false;
    int b_in_singleq = false;
    int b_in_doubleq = false;

    int b_start = false;
    int b_end = false;

    char *bp, *cp, *ep;
    bp = ep = str;

    ctx_myprintf(1, ctx, "[cmdargs_parser] Parse arguments of command line [%s]\n", str);
    ctx_dbmyprintf(1, ctx, "[cmdargs_parser] Parse arguments of command line [%s]\n", str);

    ielt = 0; nelt = 1;
    (*array) = (char**) malloc (nelt * sizeof(char*));

    for(cp=bp; *cp != '\0'; cp++) {

        if (*cp == '\\') {
            ctx_dbmyprintf(2, ctx, "Escaping car\n", "");
            /* TODO  do no store the '\' char : use a temporary buffer */
            b_escape = true;
            continue;
        }
/*
        switch (*cp) {
            case '\'':
                if (!b_escape) {
                    if (!b_start) {
                        b_start = true;
                        bp = cp;
                    }
                    ep = cp;

                    b_singleq = !b_singleq;
                } else {
                    continue;
                }
                break;
            case '"':
                if (!b_escape) {
                    if (!b_start) {
                        b_start = true;
                        bp = cp;
                    }
                    ep = cp;

                    b_doubleq = !b_doubleq;
                } else {
                    continue;
                }
                break;
            default:
                break;
        }

        if (b_escape) { b_escape = false; continue; }
        if (b_singleq || b_doubleq) { continue; }

        switch (*cp) {
            case ' ':
                b_end = true;
                break;
            default:
                if (!b_start) {
                    b_start = true;
                    bp = cp;
                }
                ep = cp;
        }
*/

        //printf("[%c]\n", *cp);

        switch (*cp) {
            case '\'':
                if (b_escape) {
                    continue;
                } else {
                    b_singleq = true;
                }
                break;
            case '"':
                if (b_escape) {
                    continue;
                } else {
                    b_doubleq = true;
                }
                break;
            case ' ':
                if (!b_in_singleq && !b_in_doubleq) {
                    printf("#1\n");
                    b_end = true;
                }
                if (b_singleq && b_in_singleq) {
                    printf("#2\n");
                    b_singleq = false;
                    b_in_singleq = false;
                    b_end = true;
                }
                if (b_in_doubleq && b_in_doubleq) {
                    printf("#3\n");
                    b_doubleq = false;
                    b_in_doubleq = false;
                    b_end = true;
                }
                break;

            default:
                if (b_singleq) {
                    b_singleq = false;
                    b_in_singleq = !b_in_singleq;
                }
                if (b_doubleq) {
                    b_doubleq = false;
                    b_in_doubleq = !b_in_doubleq;
                }
                if (!b_start) {
                    b_start = true;
                    bp = cp;
                }

                ep = cp;
                break;
        }

        if (b_escape) { b_escape = false; continue; }
        if (b_in_singleq || b_in_doubleq) { continue; }

        if (b_end) {
            if (b_start) {
                (*array) = realloc (*array, ++nelt * sizeof(char**));
                (*array)[ielt] = strndup (bp, ep-bp+1);
                ctx_dbmyprintf(2, ctx, "[cmdargs_parser] New #%d argument [%s]\n", ielt+1, (*array)[ielt]);
                ielt++;

                bp = cp+1;
            }
            b_start = b_end = false;
            b_singleq = b_doubleq = false;
            b_in_singleq = b_in_doubleq = false;
        }
    }

    if (b_start) {
        (*array) = realloc (*array, ++nelt * sizeof(char**));
        (*array)[ielt] = strndup (bp, ep-bp+1);
        ctx_dbmyprintf(2, ctx, "[cmdargs_parser] New #%d argument [%s]\n", ielt+1, (*array)[ielt]);
        ielt++;
    }

    (*array)[ielt] = NULL;
    ctx_dbmyprintf(1, ctx, "[cmdargs_parser] End of parser : found %d arguments\n", ielt);
    return ielt;
}
