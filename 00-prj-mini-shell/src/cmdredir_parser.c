#include "typedef.h"
#include "cmdredir_parser.h"


cmdredir_p cmdredir_new (char *cmd, int redir, cmdredir_p prev, cmdredir_p next) {

    cmdredir_p c;
    if (!(c = (cmdredir_p) malloc (sizeof(cmdredir_t)))) {
        perror("malloc of cmdredir element.");
		return NULL;
    }

    c->cmd = cmd;
    c->redir = redir;
    c->prev = prev;
    c->next = next;
    return c;
}

void cmdredir_free (cmdredir_p c) {
    cmdredir_p tmp;
    while (c) {
        tmp = c->next;
        freeif(c->cmd);
        free(c);
        c = tmp;
    }
}

void cmdredir_print (cmdredir_p c) {
    while(c != NULL) {
        char *redir = NULL;
        if (c->redir == CMDREDIR_EMPTY) {
            redir = strdup("");
        } else if (c->redir == CMDREDIR_TRUNCAT) {
            redir = strdup(">");
        } else if (c->redir == CMDREDIR_APPEND) {
            redir = strdup(">>");
        } else if (c->redir == CMDREDIR_INPUT) {
            redir = strdup("<");
        } else if (c->redir == CMDREDIR_DINPUT) {
            redir = strdup("<<");
        } else {
            redir = strdup("");
        }

        printf ("[CMDREDIR] cmd[%s] redirection[%s] / pointers : prev=[%p] cur=[%p] next[%p]\n",
            c->cmd, redir, c->prev, c, c->next
        );

        freeif(redir);
        c = c->next;
    }
}

cmdredir_p cmdredir_parse (char *str) {

    int b_escape = false;
    int b_singleq = false;
    int b_doubleq = false;
    int b_gt = false;
    int b_lt = false;

    int b_cmdstart = false;
    int b_cmdend = false;

    int redir = CMDREDIR_EMPTY;
    char *bp, *cp, *ep; /* begin/cur/end pointers */

    cmdredir_p cmdredir_cur, cmdredir_prev, cmdredir_first;

    if (str == NULL || *str == '\0') { return NULL; }

    bp = ep = str;
    cmdredir_cur = cmdredir_prev = cmdredir_first = NULL;

    for(cp=bp; *cp != '\0'; cp++) {

        //printf("[%c]\n", *cp);

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
                cmdredir_cur = cmdredir_new (strndup (bp, ep-bp+1), redir, cmdredir_prev, NULL);
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
        cmdredir_cur = cmdredir_new (strndup (bp, cp-bp+1), redir, cmdredir_prev, NULL);
        if (cmdredir_prev != NULL) { cmdredir_prev->next = cmdredir_cur; }
        if (cmdredir_first == NULL) { cmdredir_first = cmdredir_cur; }
    }

    return cmdredir_first;
}
