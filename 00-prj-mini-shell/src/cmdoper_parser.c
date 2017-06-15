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
 *    \______________/   \______________/   \__________/
 *       cmdoper-1           cmdoper-2        cmdoper-4
 *
 * Les séquences '||', '&&', '&' sont considérées comme des opérateurs (et donc
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
 *       '--[.prev] --<-- CMDOPER-3 (opér[]) -->--[.next]--->---.
 *                                                              |
 *                                           X-----------------'
 *
 * Puis, pour chaque 'cmdoper', on utilise le même principe pour éclater la
 * commande du cmdoper en une liste chainée de 'cmdredir'. Dans cette 2ème
 * phase, les séparateurs sont maintenant :  |, >, >>, <, <<.
 *
 * A la toute fin du parsing, on obtient donc une liste chaînée de 'cmdoper',
 * qui sont reliés par un opérateur ('||', '&&' ou '&'), et pour chacun de ces
 * 'cmdoper', on dispose d'une liste chaînée résultante de 'cmdredir'. Ces
 * cmdredir sont reliés entre eux par des redirecteurs ('|', '>', '<', '>>',
 * et '<<'.)
 *
 */

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
cmdoper_p cmdoper_new (char *cmd, int type, cmdoper_p prev, cmdoper_p next) {

    cmdoper_p o;
    if (!(o = (cmdoper_p) malloc (sizeof(cmdoper_t)))) {
        perror("malloc of cmdoper element.");
		return NULL;
    }

    o->cmd = cmd;
    o->type = type;
    o->redir = NULL;
    o->prev = prev;
    o->next = next;
    return o;
}

void cmdoper_free (cmdoper_p o) {
    cmdoper_p tmp;
    while (o) {
        tmp = o->next;
        freeif(o->cmd);
        if (o->redir) { cmdredir_free(o->redir); }
        free(o);
        o = tmp;
    }
}

void cmdoper_print_one (mysh_context_p ctx, cmdoper_p o) {
    char *type_str = NULL;
    if (o->type == CMDOPER_EMPTY) {
        type_str = strdup("");
    } else if (o->type == CMDOPER_SEMICOLON) {
        type_str = strdup(";");
    } else if (o->type == CMDOPER_OR) {
        type_str = strdup("||");
    } else if (o->type == CMDOPER_BACKGND) {
        type_str = strdup("&");
    } else if (o->type == CMDOPER_AND) {
        type_str = strdup("&&");
    } else {
        type_str = strdup("");
    }

    ctx_dbmyprintf(1, ctx, M_CMDOPER_PARSER_PRINT_PTR, o->cmd, type_str, o->prev, o, o->next);
    ctx_myprintf(1, ctx, M_CMDOPER_PARSER_PRINT, o->cmd, type_str);

    cmdredir_print (ctx, o->redir);

    freeif(type_str);
}

void cmdoper_print (mysh_context_p ctx, cmdoper_p o) {
    for( ; o!=NULL; o=o->next) {
        cmdoper_print_one (ctx, o);
    }
}

cmdoper_p cmdoper_parser (mysh_context_p ctx, char *str) {

    int b_escape = false;
    int b_singleq = false;
    int b_doubleq = false;
    int b_pipeline = false;
    int b_ampersand = false;

    int b_cmdstart = false;
    int b_cmdend = false;

    int type = CMDOPER_EMPTY;
    char *bp, *cp, *ep; /* begin/cur/end pointers */

    cmdoper_p cmdoper_cur, cmdoper_prev, cmdoper_first;

    if (str == NULL || *str == '\0') { return NULL; }

    ctx_myprintf(1, ctx, M_CMDOPER_PARSER_PARSE_CMDLINE, str);
    ctx_dbmyprintf(1, ctx, M_CMDOPER_PARSER_PARSE_CMDLINE, str);

    bp = ep = str;
    cmdoper_cur = cmdoper_prev = cmdoper_first = NULL;

    ctx_dbmyprintf(3, ctx, M_CMDOPER_PARSER_CURSOR_IS, bp);
    for(cp=bp; *cp != '\0'; cp++) {

        ctx_dbmyprintf(3, ctx, M_CMDOPER_PARSER_ANALYZE_CHAR, *cp);

        if (*cp == '\\') {
            ctx_dbmyprintf(2, ctx, "[cmdoper_parser] Escaping car\n", "");
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
                type = CMDOPER_SEMICOLON;
                break;
            case '|':
                if (b_pipeline) {
                    b_cmdend = true;
                    type = CMDOPER_OR;
                }
                b_pipeline = !b_pipeline;
                break;
            case '&':
                b_cmdend = true;
                type = CMDOPER_BACKGND;
                if (b_ampersand && cmdoper_cur != NULL) { cmdoper_cur->type = CMDOPER_AND; type = CMDOPER_EMPTY;}
                b_ampersand = !b_ampersand;
                break;
            case ' ':
                break;
            default:
                if (b_ampersand) {
                    if (cmdoper_cur != NULL) { cmdoper_cur->type = CMDOPER_BACKGND; }
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
                cmdoper_cur = cmdoper_new (strndup (bp, ep-bp+1), type, cmdoper_prev, NULL);
                if (cmdoper_prev != NULL) { cmdoper_prev->next = cmdoper_cur; }
                if (cmdoper_first == NULL) { cmdoper_first = cmdoper_cur; }
                bp = cp+1;
                ctx_dbmyprintf(3, ctx, M_CMDOPER_PARSER_CURSOR_IS, bp);
            }
            type = CMDOPER_EMPTY;
            b_cmdstart = b_cmdend = false;
        }
    }

    if (b_cmdstart) {
        if (cmdoper_cur) { cmdoper_prev = cmdoper_cur; }
        cmdoper_cur = cmdoper_new (strndup (bp, cp-bp+1), type, cmdoper_prev, NULL);
        if (cmdoper_prev != NULL) { cmdoper_prev->next = cmdoper_cur; }
        if (cmdoper_first == NULL) { cmdoper_first = cmdoper_cur; }
    }

    return cmdoper_first;
}

void cmdoper_parse_redir (mysh_context_p ctx, cmdoper_p o) {
    for ( ; o != NULL; o = o->next) {
        o->redir = cmdredir_parser(ctx, o);
        cmdredir_parse_args (ctx, o->redir);
    }
}
