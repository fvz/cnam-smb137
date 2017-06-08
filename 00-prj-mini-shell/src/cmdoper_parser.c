/**
 * \file cmdoper_parser.c
 * \brief Parser de 'cmdoper' (notion expliquée +bas)
 * \author Fabien VANTARD
 * \version 0.1
 *
 * Il a été choisi de parser les lignes de commandes en utilisant 2 notions :
 *  - cmdoper : un opérateur de commandes
 *  - cmdredir : une redirection de commandes
 *
 * Le parser extrait d'abord un ensemble de 'cmdoper' comme suit :
 *
 * $  command1 > file || command2 < file2 && cmd3 | cmd4
 *    \______________/   \______________/   \___/  \___/
 *       cmdoper-1           cmdoper-2        |      \_cmdoper-4
 *                                            \_cmdoper-3
 *
 * Les séquences || && | & sont considérés comme des opérateurs (et donc
 * comme des séparateurs dans la ligne de commande.)
 *
 * Ensuite, une liste chaînée est construite avec les cmdoper et leurs
 * opérateurs associés.
 *
 *        ,--->---->----X
 *       |
 *       '--[.prev] --<-- CMDOPER-1 (opér[||]) -->--[.next]-->--.
  *       ,----->---->----^                                     |
 *       |                                   v-----------------'
 *       '--[.prev] --<-- CMDOPER-2 (opér[&&]) -->--[.next]-->--.
 *        ,----->---->----^                                     |
 *       |                                   v-----------------'
 *       '--[.prev] --<-- CMDOPER-3 (opér[|] ) -->--[.next]-->--.
 *        ,----->---->----^                                     |
 *       |                                   v-----------------'
 *       '--[.prev] --<-- CMDOPER-4 (opér[]) -->--[.next]--->---.
 *                                                              |
 *                                           X-----------------'
 *
 * Puis, pour chaque 'cmdoper', on utilise le même principe pour éclater la
 * commande du cmdoper en une liste chainée de 'cmdredir'. Dans cette 2ème
 * phase, les séparateurs sont maintenant :  >, >>, <, <<.
 *
 * A la toute fin du parsing, on obtient donc une liste chaînée de 'cmdoper',
 * qui sont reliés par un opérateur ('||', '&&', '|' et '&'), et pour chacun
 * de ces 'cmdoper', on dispose d'une liste chaînée résultante de 'cmdredir'.
 * Ces cmdredir sont reliés entre eux par des redirecteurs ('>', '<', '>>',
 * et '<<'.)
 */

#include "typedef.h"
#include "cmdoper_parser.h"


/**
 * \fn cmdoper_p cmdoper_new (char *cmd, int oper, cmdoper_p prev, cmdoper_p next)
 * \brief Constructeur d'objet cmdoper
 *
 * \param cmd chaîne de caractère correspondant au cmdoper
 * \param oper Opérateur du cmdoper
 * \param prev Element cmdoper précédent (dans la liste chaînée)
 * \param next Element cmdoper suivant (dans la liste chaînée)
 * \return nouvel élément cmdoper alloué
 */
cmdoper_p cmdoper_new (char *cmd, int oper, cmdoper_p prev, cmdoper_p next) {

    cmdoper_p c;
    if (!(c = (cmdoper_p) malloc (sizeof(cmdoper_t)))) {
        perror("malloc of cmdoper element.");
		return NULL;
    }

    c->cmd = cmd;
    c->oper = oper;
    c->prev = prev;
    c->next = next;
    return c;
}

void cmdoper_free (cmdoper_p c) {
    cmdoper_p tmp;
    while (c) {
        tmp = c->next;
        freeif(c->cmd);
        if (c->redir) { cmdredir_free(c->redir); }
        free(c);
        c = tmp;
    }
}

void cmdoper_print_one (cmdoper_p c) {
    char *oper = NULL;
    if (c->oper == CMDOPER_EMPTY) {
        oper = strdup("");
    } else if (c->oper == CMDOPER_SEMICOLON) {
        oper = strdup(";");
    } else if (c->oper == CMDOPER_PIPE) {
        oper = strdup("|");
    } else if (c->oper == CMDOPER_OR) {
        oper = strdup("||");
    } else if (c->oper == CMDOPER_BACKGND) {
        oper = strdup("&");
    } else if (c->oper == CMDOPER_AND) {
        oper = strdup("&&");
    } else {
        oper = strdup("");
    }

    printf ("[CMDOPER] cmd[%s] operator[%s] / pointers : prev=[%p] cur=[%p] next[%p]\n",
        c->cmd, oper, c->prev, c, c->next
    );
    cmdredir_print (c->redir);

    freeif(oper);
}

void cmdoper_print (cmdoper_p c) {
    for( ; c!=NULL; c=c->next) {
        cmdoper_print_one (c);
    }
}


cmdoper_p cmdoper_parse (char *str) {

    int b_escape = false;
    int b_singleq = false;
    int b_doubleq = false;
    int b_pipeline = false;
    int b_ampersand = false;

    int b_cmdstart = false;
    int b_cmdend = false;

    int oper = CMDOPER_EMPTY;
    char *bp, *cp, *ep; /* begin/cur/end pointers */

    cmdoper_p cmdoper_cur, cmdoper_prev, cmdoper_first;

    if (str == NULL || *str == '\0') { return NULL; }

    bp = ep = str;
    cmdoper_cur = cmdoper_prev = cmdoper_first = NULL;

    for(cp=bp; *cp != '\0'; cp++) {

        printf("[%c]\n", *cp);

        if (*cp == '\\') {
            printf("Escaping car\n");
            /* TODO  do no store the '\' char : use a temporary buffer */
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
            case ';':
                b_cmdend = true;
                oper = CMDOPER_SEMICOLON;
                break;
            case '|':
                b_cmdend = true;
                oper = CMDOPER_PIPE;
                if (b_pipeline && cmdoper_cur != NULL) { cmdoper_cur->oper = CMDOPER_OR; oper = CMDOPER_EMPTY;}
                b_pipeline = !b_pipeline;
                break;
            case '&':
                b_cmdend = true;
                oper = CMDOPER_BACKGND;
                if (b_ampersand && cmdoper_cur != NULL) { cmdoper_cur->oper = CMDOPER_AND; oper = CMDOPER_EMPTY;}
                b_ampersand = !b_ampersand;
                break;
            case ' ':
                break;
            default:
                if (b_pipeline) {
                    if (cmdoper_cur != NULL) { cmdoper_cur->oper = CMDOPER_PIPE; }
                    b_pipeline = false;
                }
                if (b_ampersand) {
                    if (cmdoper_cur != NULL) { cmdoper_cur->oper = CMDOPER_BACKGND; }
                    b_ampersand = false;
                }

                if (!b_cmdstart) {
                    b_cmdstart = true;
                    bp = cp;
                }
                ep = cp;
        }

        if (b_cmdend) {
            if (b_cmdstart) {
                if (cmdoper_cur) { cmdoper_prev = cmdoper_cur; }
                cmdoper_cur = cmdoper_new (strndup (bp, ep-bp+1), oper, cmdoper_prev, NULL);
                if (cmdoper_prev != NULL) { cmdoper_prev->next = cmdoper_cur; }
                if (cmdoper_first == NULL) { cmdoper_first = cmdoper_cur; }

                oper = CMDOPER_EMPTY;
                bp = cp+1;
            }
            b_cmdstart = b_cmdend = false;
        }
    }

    if (b_cmdstart) {
        if (cmdoper_cur) { cmdoper_prev = cmdoper_cur; }
        cmdoper_cur = cmdoper_new (strndup (bp, cp-bp+1), oper, cmdoper_prev, NULL);
        if (cmdoper_prev != NULL) { cmdoper_prev->next = cmdoper_cur; }
        if (cmdoper_first == NULL) { cmdoper_first = cmdoper_cur; }
    }

    return cmdoper_first;
}


void cmdoper_parse_redir (cmdoper_p c) {
    while(c != NULL) {
        c->redir = cmdredir_parse(c->cmd);
        c = c->next;
    }
}
