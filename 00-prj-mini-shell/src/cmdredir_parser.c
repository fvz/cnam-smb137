/**
 * \file cmdredir_parser.c
 * \brief Parser de 'cmdredir'
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "cmdredir_parser.h"

/**
 * \fn cmdredir_p cmdredir_new (cmdoper_p oper, char *cmd, int redir, cmdredir_p prev, cmdredir_p next)
 * \brief Constructeur d'objet cmdredir
 *
 * \param oper cmdoper parent
 * \param cmd Commande correspondante
 * \param redir Type de redirection
 * \param prev Element cmdredir précédent (dans la liste chaînée)
 * \param next Element cmdredir suivant (dans la liste chaînée)
 * \return nouvel élément cmdoper alloué
 */
cmdredir_p cmdredir_new (cmdoper_p oper, char *cmd, int redir, cmdredir_p prev, cmdredir_p next) {

    cmdredir_p r;
    if (!(r = (cmdredir_p) malloc (sizeof(cmdredir_t)))) {
        perror("malloc of cmdredir element.");
		return NULL;
    }

    r->oper = oper;

    r->cmd = cmd;
    r->redir = redir;
    r->prev = prev;
    r->next = next;

    r->status = CMDREDIR_STATUS_INIT;
    return r;
}

/**
 * \fn void cmdredir_free (cmdredir_p r)
 * \brief Destructeur d'objet cmdredir
 *
 * \param r Pointeur sur objet cmdredir
 */
void cmdredir_free (cmdredir_p r) {
    cmdredir_p tmp;
    while (r) {
        tmp = r->next;
        freeif(r->cmd);
        cmdargs_free(&(r->args));
        free(r);
        r = tmp;
    }
}

/**
 * \fn void cmdredir_print (mysh_context_p ctx, cmdredir_p r)
 * \brief Affiche un cmdredir
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur objet cmdredir
 */
void cmdredir_print (mysh_context_p ctx, cmdredir_p r) {
    for ( ; r != NULL; r = r->next) {

        char *redir = NULL;
        if (r->redir == CMDREDIR_EMPTY) {
            redir = strdup("");
        } else if (r->redir == CMDREDIR_PIPE) {
            redir = strdup("|");
        } else if (r->redir == CMDREDIR_TRUNCAT) {
            redir = strdup(">");
        } else if (r->redir == CMDREDIR_APPEND) {
            redir = strdup(">>");
        } else if (r->redir == CMDREDIR_INPUT) {
            redir = strdup("<");
        } else if (r->redir == CMDREDIR_DINPUT) {
            redir = strdup("<<");
        } else {
            redir = strdup("");
        }

        ctx_dbmyprintf(1, ctx, M_CMDREDIR_PARSER_PRINT_PTR, r->cmd, redir, r->prev, r, r->next);
        ctx_myprintf(1, ctx, M_CMDREDIR_PARSER_PRINT, r->cmd, redir);

        freeif(redir);
    }
}

/**
 * \fn cmdredir_p cmdredir_parser (mysh_context_p ctx, cmdoper_p oper)
 * \brief Parser de cmdredir (en se basant sur cmd de cmdoper)
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param oper cmdoper parent
 * \return cmdredir_p retourne le pointeur sur le 1er élément de la liste chainée
 */
cmdredir_p cmdredir_parser (mysh_context_p ctx, cmdoper_p oper) {

    int b_escape = false;
    int b_singleq = false;
    int b_doubleq = false;
    int b_pipe = false;
    int b_gt = false;
    int b_lt = false;

    int b_cmdstart = false;
    int b_cmdend = false;

    int redir = CMDREDIR_EMPTY;
    char *bp, *cp, *ep; /* begin/cur/end pointers */

    cmdredir_p cmdredir_cur, cmdredir_prev, cmdredir_first;

    if ((oper->cmd == NULL) || ((oper->cmd)[0] == '\0')) { return NULL; }

    bp = ep = oper->cmd;
    cmdredir_cur = cmdredir_prev = cmdredir_first = NULL;

    for(cp=bp; *cp != '\0'; cp++) {

        ctx_dbmyprintf(2, ctx, M_CMDREDIR_ANALYZE_CHAR, *cp);

        if (*cp == '\\') {
            ctx_dbmyprintf(2, ctx, "Escaping car\n", "");
            b_escape = true;
            continue;
        }

        switch (*cp) {
            case '\'':
                if (!b_escape) {
                    if (!b_cmdstart) {
                        b_cmdstart = true;
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
                    if (!b_cmdstart) {
                        b_cmdstart = true;
                        bp = cp;
                    }
                    ep = cp;

                    b_doubleq = !b_doubleq;
                } else {
                    continue;
                }
                break;
        }

        if (b_escape) { b_escape = false; continue; }
        if (b_singleq || b_doubleq) { continue; }

        switch (*cp) {
            case '|':
                b_cmdend = true;
                redir = CMDREDIR_PIPE;
                if (b_pipe && cmdredir_cur != NULL) { redir = CMDREDIR_EMPTY;}
                b_pipe = !b_pipe;
                break;
            case '>':
                b_cmdend = true;
                redir = CMDREDIR_TRUNCAT;
                if (b_gt && cmdredir_cur != NULL) { cmdredir_cur->redir = CMDREDIR_APPEND; redir = CMDREDIR_EMPTY;}
                b_gt = !b_gt;
                break;
            case '<':
                b_cmdend = true;
                redir = CMDREDIR_INPUT;
                if (b_lt && cmdredir_cur != NULL) { cmdredir_cur->redir = CMDREDIR_DINPUT; redir = CMDREDIR_EMPTY;}
                b_lt = !b_lt;
                break;
            case ' ':
                break;
            default:
                if (b_pipe) {
                    if (cmdredir_cur != NULL) { cmdredir_cur->redir = CMDREDIR_PIPE; }
                    b_pipe = false;
                }
                if (b_gt) {
                    if (cmdredir_cur != NULL) { cmdredir_cur->redir = CMDREDIR_TRUNCAT; }
                    b_gt = false;
                }
                if (b_lt) {
                    if (cmdredir_cur != NULL) { cmdredir_cur->redir = CMDREDIR_INPUT; }
                    b_lt = false;
                }

                if (!b_cmdstart) {
                    b_cmdstart = true;
                    bp = cp;
                }
                ep = cp;
        }

        if (b_cmdend) {
            if (b_cmdstart) {
                if (cmdredir_cur) { cmdredir_prev = cmdredir_cur; }
                cmdredir_cur = cmdredir_new (oper, strndup (bp, ep-bp+1), redir, cmdredir_prev, NULL);
                if (cmdredir_prev != NULL) { cmdredir_prev->next = cmdredir_cur; }
                if (cmdredir_first == NULL) { cmdredir_first = cmdredir_cur; }

                redir = CMDREDIR_EMPTY;
                bp = cp+1;
            }
            b_cmdstart = b_cmdend = false;
        }
    }

    if (b_cmdstart) {
        if (cmdredir_cur) { cmdredir_prev = cmdredir_cur; }
        cmdredir_cur = cmdredir_new (oper, strndup (bp, cp-bp+1), redir, cmdredir_prev, NULL);
        if (cmdredir_prev != NULL) { cmdredir_prev->next = cmdredir_cur; }
        if (cmdredir_first == NULL) { cmdredir_first = cmdredir_cur; }
    }

    return cmdredir_first;
}

/**
 * \fn void cmdredir_parse_args (mysh_context_p ctx, cmdredir_p r)
 * \brief Fonction de facilité pour Parser de cmdargs
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur liste chainée de cmdredir
 */
void cmdredir_parse_args (mysh_context_p ctx, cmdredir_p r) {
    for ( ; r != NULL; r = r->next) {
        r->nargs = cmdargs_parser(ctx, r->cmd, &(r->args));
    }
}
