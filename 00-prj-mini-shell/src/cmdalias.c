/**
 * \file cmdalias.c
 * \brief Mécanisme d'alias de commande
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "cmdalias.h"

/**
 * \fn cmdalias_p cmdalias_new(char *name, char *cmd)
 * \brief Constructeur d'objet cmdalias_p
 *
 * \param name nom de l'alias
 * \param cmd commande correspondante
 * \return objet cmdalias_p nouvellement créé
 */
cmdalias_p cmdalias_new(char *name, char *cmd) {
    cmdalias_p alias = (cmdalias_p) malloc (sizeof(cmdalias_t));
    cmdalias_set(alias, name, cmd);
    return alias;
}

/**
 * \fn void cmdalias_free(cmdalias_p alias)
 * \brief Libère un objet cmdalias_p passé en paramètre
 *
 * \param alias Pointeur sur l'objet cmdalias_p à libérer
 */
void cmdalias_free(cmdalias_p alias) {

    if (alias) {
        free(alias->name);
        free(alias->cmd);
        free(alias);
    }
}

/**
 * \fn void cmdalias_free_array_elt(cmdalias_p *alias)
 * \brief Libère l'ensemble des éléments cmdalias_p du tableau passé en paramètre
 *
 * \param alias Pointeur sur le tableau d'objet cmdalias_p
 */
void cmdalias_free_array_elt(cmdalias_p *alias) {
    if (alias) {
        int i;
        for (i=0; alias[i]; i++) {
            cmdalias_free(alias[i]);
        }
    }
}

/**
 * \fn void cmdalias_free_array(cmdalias_p **alias)
 * \brief Libère le tableau d'alias de la session courante
 *
 * \param alias Pointeur sur le tableau d'objet cmdalias_p
 */
void cmdalias_free_array(cmdalias_p **alias) {
    if (alias && *alias) {
        cmdalias_free_array_elt(*alias);
        freeif(*alias);
    }
}

/**
 * \fn cmdalias_p cmdalias_search(mysh_context_p ctx, char *name)
 * \brief Cherche un alias dans la liste d'alias définis dans la session.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param name nom de l'alias à chercher
 *
 * \return NULL si non alias non trouvé. ou pointeur sur l'alias si trouvé.
 */
cmdalias_p cmdalias_search(mysh_context_p ctx, char *name) {

    int i;
    if (!ctx || !ctx->alias) {
        return NULL;
    }

    for(i=0; ctx->alias[i]; i++) {
        if (strcmp(name, ctx->alias[i]->name) == 0) {
            return ctx->alias[i];
        }
    }
    return NULL;
}

/**
 * \fn void cmdalias_set(cmdalias_p alias, char *name, char *cmd)
 * \brief Définit un objet cmdalias avec les name/cmd passés en paramètre.
 *
 * \param alias Pointeur sur l'alias à redéfinir
 * \param name nom de l'alias
 * \param cmd nouvelle commande
 *
 * Cette commande duplique les chaines name/cmd passées en paramètres.
 */
void cmdalias_set(cmdalias_p alias, char *name, char *cmd) {

    if (alias) {
        cmdalias_set_with_new(alias, strdup(name), strdup(cmd));
    }
}

/**
 * \fn void cmdalias_set_with_new(cmdalias_p alias, char *name, char *cmd)
 * \brief Définit un objet cmdalias avec les name/cmd passés en paramètre.
 *
 * \param alias Pointeur sur l'alias à redéfinir
 * \param name nom de l'alias
 * \param cmd nouvelle commande
 *
 * Cette commande NE duplique PAS les chaines name/cmd passées en paramètres.
 * Elle assigne uniquement leur pointeur.
 */
void cmdalias_set_with_new(cmdalias_p alias, char *name, char *cmd) {

    if (alias) {
        freeif(alias->name); alias->name = name;
        freeif(alias->cmd); alias->cmd = cmd;
    }
}

/**
 * \fn void cmdalias_redefine_cmd_with_new(cmdalias_p alias, char *cmd)
 * \brief Redéfinit la commande d'un alias existant
 *
 * \param alias Pointeur sur l'alias à redéfinir
 * \param cmd nouvelle commande
 *
 * Cette fonction duplique la chaine 'cmd' passée en paramètre.
 */
void cmdalias_redefine_cmd(cmdalias_p alias, char *cmd) {
    cmdalias_redefine_cmd_with_new(alias, strdup(cmd));
}

/**
 * \fn void cmdalias_redefine_cmd_with_new(cmdalias_p alias, char *cmd)
 * \brief Redéfinit la commande d'un alias existant
 *
 * \param alias Pointeur sur l'alias à redéfinir
 * \param cmd nouvelle commande
 *
 * Cette fonction NE duplique PAS la chaine 'cmd' passée en paramètre.
 * Elle assigne uniquement son pointeur.
 */
void cmdalias_redefine_cmd_with_new(cmdalias_p alias, char *cmd) {
    if (alias) {
        freeif(alias->cmd); alias->cmd = cmd;
    }
}

/**
 * \fn cmdalias_add(mysh_context_p ctx, char *name, char *cmd)
 * \brief Ajoute un alias à la liste courante (dans l'instance courant de mysh)
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param name nom de l'alias
 * \param cmd commande correspondante
 */
void cmdalias_add(mysh_context_p ctx, char *name, char *cmd) {

    if (ctx && name && cmd) {
        if (ctx->alias == NULL) {
            ctx->nalias = 1;
            ctx->alias = malloc (2 * sizeof(cmdalias_p));
            ctx->alias[0] = cmdalias_new(name, cmd);
        } else {
            ctx->alias[ctx->nalias] = cmdalias_new(name, cmd);
            ctx->nalias++;
            ctx->alias = realloc(ctx->alias, ctx->nalias * sizeof(cmdalias_p));
        }
        ctx->alias[ctx->nalias] = NULL;
    }
}

/**
 * \fn int cmdalias_parse_cmd(mysh_context_p ctx, char *cmd, char **newalias, char **newcmd)
 * \brief Parser d'alias. Trouve le nom et la commande de l'alias  nom='commande'
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param cmd Argument à parser
 * \param newalias pointeur où assigner une chaine allouée contenant le nom de l'alias trouvé
 * \param newcmd pointeur où assigner une chaine allouée contenant la commande de l'alias trouvée
 *
 * \return true si parsing ok
 */
int cmdalias_parse_cmd(mysh_context_p ctx, char *cmd, char **newalias, char **newcmd) {

    char *bp, *cp, *ep;
    int in_alias = false;
    int in_cmd = false;
    int alias_ok = false;
    int b_start = false;
    int b_end = false;

    if (newalias) { *newalias = NULL; }
    if (newcmd) { *newcmd = NULL; }

    if (!cmd || *cmd == '\0') {
        return false;
    }

    ctx_myprintf(1, ctx, M_CMDALIAS_PARSER_PARSE_CMD, cmd);
    ctx_dbmyprintf(1, ctx, M_CMDALIAS_PARSER_PARSE_CMD, cmd);

    bp = ep = cmd;
    ctx_dbmyprintf(3, ctx, M_CMDALIAS_PARSER_CURSOR_IS, bp);
    for(cp=bp; *cp != '\0'; cp++) {

        ctx_dbmyprintf(3, ctx, M_CMDALIAS_PARSER_ANALYZE_CHAR, *cp);

        switch(*cp) {
            case ' ':
            case '=':
                if (in_alias) {
                    b_end = true;
                    ep = cp;
                }
                break;
            default:
                if(alias_ok) {
                    if (!in_cmd) { in_cmd = true; }
                } else {
                    if(!in_alias) { in_alias = true; }
                }
                if (!b_start) {
                    b_start = true;
                    bp = cp;
                }
                break;
        }

        if (b_end) {
            if (b_start) {
                if (newalias) {
                    (*newalias) = strndup (bp, ep - bp);
                    ctx_dbmyprintf(2, ctx, M_CMDALIAS_PARSER_NAME_FOUND, *newalias);
                }
                bp = cp+1;
                ctx_dbmyprintf(3, ctx, M_CMDALIAS_PARSER_CURSOR_IS, bp);
                alias_ok = true;
                in_alias = false;
            }
            b_start = b_end = false;
        }
    }

    if (b_start) {
        if (alias_ok) {
            if (newcmd) {
                (*newcmd) = strndup (bp, cp - bp);
                ctx_dbmyprintf(2, ctx, M_CMDALIAS_PARSER_CMD_FOUND, *newcmd);
            }
        }
        else {
            if (newalias) {
                (*newalias) = strndup (bp, cp - bp);
                ctx_dbmyprintf(2, ctx, M_CMDALIAS_PARSER_NAME_FOUND, *newalias);
            }
        }
    }

    ctx_dbmyprintf(1, ctx, M_CMDALIAS_PARSER_END_GIVE, cmd, *newalias, *newcmd);
    return true;
}

/**
 * \fn void cmdalias_print_one(cmdalias_p alias)
 * \brief Affiche un alias passé en paramètre
 *
 * \param alias Pointeur sur un objet cmdalias_p
 */
void cmdalias_print_one(cmdalias_p alias) {
    if (alias) {
        printf(M_CMDALIAS_ALIAS_DEFINITION,
            alias->name ? alias->name : "",
            alias->cmd ? alias->cmd : ""
        );
    }
}

/**
 * \fn void cmdalias_print(mysh_context_p ctx)
 * \brief Affiche la liste des alias du contexte courant
 *
 * \param ctx Pointeur sur le contexte mysh_context
 */
void cmdalias_print(mysh_context_p ctx) {
    if (ctx && ctx->alias) {
        int i;
        for (i=0; ctx->alias[i]; i++) {
            cmdalias_print_one(ctx->alias[i]);
        }
    }
    else {
        printf(M_CMDALIAS_NO_ALIAS_DEFINED);
    }
}

/**
 * \fn void cmdalias_check_alias(mysh_context_p ctx, cmdredir_p r)
 * \brief Vérifie si 1er argument est un alias et le remplace si besoin
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur l'objet cmdredir_p courant
 */
void cmdalias_check_alias(mysh_context_p ctx, cmdredir_p r) {

    cmdalias_p alias;
    if ((alias = cmdalias_search(ctx, r->args[0])) != NULL) {
        char **array;
        int i;
        ctx_dbmyprintf(1, ctx, M_CMDHANDLE_ALIAS_FOUND, r->args[0], alias->name, alias->cmd);

        /* replace r->args[0] by cmdargs_parse(alias->cmd) */
        cmdargs_parser(ctx, alias->cmd, &array);
        for (i=1; r->args[i]; i++) {
            cmdargs_append_with_new(&array, r->args[i]);
            r->args[i] = NULL;
        }
        cmdargs_free(&(r->args));
        r->args = array;

        /* Perhaps we can redefine r->cmd (by joining r->args with ' ' separators), but
           the handling mecanism is working with r->args only. So it is a bit useless. */
    }
}
