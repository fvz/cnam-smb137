/**
 * \file cmdargs_parser.c
 * \brief Parser d'arguments d'une commande
 * \author Fabien VANTARD
 * \version 0.1
 */

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
 * \fn void cmdargs_append (char ***array)
 * \brief Ajoute un argument à la fin d'un tableau d'arguments (array = char **)
 *
 * \param array pointeur sur tableau d'arguments char* modifié
 */
int cmdargs_append (char ***array, char *arg) {
    return cmdargs_append_with_new (array, strdup(arg));
}

/**
 * \fn void cmdargs_append (char ***array)
 * \brief Ajoute un argument à la fin d'un tableau d'arguments (array = char **)
 *
 * \param array pointeur sur tableau d'arguments char* modifié
 *
 * Dans ce cas, la chaine 'arg' n'est pas strdupée.
 */
int cmdargs_append_with_new (char ***array, char *arg) {

    int ielt;
    if (!array) { return -1; }
    for(ielt=0; (*array) && (*array)[ielt]; ielt++) {
        /* nothing : loop just count */
    }
    (*array) = realloc(*array, (ielt+2) * sizeof(char*));
    (*array)[ielt++] = arg;
    (*array)[ielt++] = NULL;
    return ielt;
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

    char *buff;
    int ibuff;

    char *bp, *cp, *ep;
    bp = ep = str;

    ctx_myprintf(1, ctx, M_CMDARGS_PARSER_PARSE_ARGS, str);
    ctx_dbmyprintf(1, ctx, M_CMDARGS_PARSER_PARSE_ARGS, str);

    buff = (char*) malloc (strlen(str) * sizeof(char));
    ibuff = 0;

    ielt = 0; nelt = 1;
    (*array) = (char**) malloc (nelt * sizeof(char*));

    ctx_dbmyprintf(3, ctx, M_CMDARGS_PARSER_CURSOR_IS, bp);
    for(cp=bp; *cp != '\0'; cp++) {

        ctx_dbmyprintf(3, ctx, M_CMDARGS_PARSER_ANALYZE_CHAR, *cp);

        if (*cp == '\\') {
            ctx_dbmyprintf(2, ctx, "Escaping car\n", "");
            b_escape = true;
            /* FIXME: seems to be not working :
                echo "a \"b c"  =>  arg1=[a "b c]
                echo "a \"b     =>  arg1=[a "b]
            */
            continue;
        }

        /*** Some specials cases of handling arguments ***
            echo "a b " " c d"  =>  arg1=[a b ]  arg2=[ c d]
            echo "a b ""c d"    =>  arg1=[a b c d]
            echo "a b "c        =>  arg1=[a b c]
            echo "a b"c         =>  arg1=[a bc]
            echo "a b" c        =>  arg1=[a b]  arg2=[c]
        */

        switch (*cp) {
            case '\'':
                if (b_escape) {
                    continue;
                } else {
                    if (b_singleq) { b_in_singleq = false; }
                    b_singleq = true;
                }
                break;
            case '"':
                if (b_escape) {
                    continue;
                } else {
                    if (b_doubleq) { b_in_doubleq = false; }
                    b_doubleq = true;
                }
                break;
            case ' ':
                if (!b_in_singleq && !b_in_doubleq
                    && !b_singleq && !b_doubleq) {
                        b_end = true;
                } else if (b_singleq && b_in_singleq) {
                    b_singleq = false;
                    b_in_singleq = false;
                    b_end = true;
                } else if (b_doubleq && b_in_doubleq) {
                    b_doubleq = false;
                    b_in_doubleq = false;
                    b_end = true;
                } else {
                    buff[ibuff++] = *cp;
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
                buff[ibuff++] = *cp;

                ep = cp;
                break;
        }

        if (b_escape) { b_escape = false; continue; }
        if (b_in_singleq || b_in_doubleq) { continue; }

        if (b_end) {
            if (b_start) {
                (*array) = realloc (*array, ++nelt * sizeof(char**));
                buff[ibuff++] = '\0';
                (*array)[ielt] = strndup (buff, ibuff);
                ibuff = 0;

                ctx_dbmyprintf(2, ctx, M_CMDARGS_PARSER_FOUND_ARG, ielt+1, (*array)[ielt]);
                ielt++;

                bp = cp+1;
                ctx_dbmyprintf(3, ctx, M_CMDARGS_PARSER_CURSOR_IS, bp);
            }
            b_start = b_end = false;
            b_singleq = b_doubleq = false;
            b_in_singleq = b_in_doubleq = false;
        }
    }

    if (b_start) {
        (*array) = realloc (*array, ++nelt * sizeof(char**));
        buff[ibuff++] = '\0';
        (*array)[ielt] = strndup (buff, ibuff);
        ibuff = 0;
        ctx_dbmyprintf(2, ctx, M_CMDARGS_PARSER_FOUND_ARG, ielt+1, (*array)[ielt]);
        ielt++;
    }

    free(buff);

    (*array)[ielt] = NULL;
    ctx_dbmyprintf(1, ctx, M_CMDARGS_PARSER_END_GIVE_ARG, str, ielt);
    return ielt;
}
