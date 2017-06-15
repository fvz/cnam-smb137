/**
 * \file builtin_cmd.c
 * \brief Fonctionnalités Built-in (référencées au sein d'un tableau dans le header associé)
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "builtin_cmd.h"

/**
 * \fn int builtin_loop_scan (mysh_context_p ctx, cmdredir_p r)
 * \brief Cherche si cmd présente parmi les builtin et exécute la fonction le cas échéant.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 *
 * \return found : retourne vrai si une commande built-in a été trouvée (et exécutée) et faux sinon.
 */
int builtin_loop_scan (mysh_context_p ctx, cmdredir_p r) {
    int i;

    for(i=0; builtin_list[i].cb != NULL; i++) {
        if (strcmp(r->args[0], builtin_list[i].cmd) == 0) {
            ctx_dbmyprintf(1, ctx, M_BUILTIN_PROCESS_BUILTIN_CMD, r->args[0]);
            r->oper->exitstatus = builtin_list[i].cb(ctx, r);
            r->oper->cmdexec = true;
            return true;
        }
    }
    return false;
}

/**
 * \fn int builtin_cmd_cd (mysh_context_p ctx, cmdoper_p oper)
 * \brief Commande Built-in "cd" : elle fait changer de dossier courant.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
int builtin_cmd_cd (mysh_context_p ctx, cmdredir_p r) {

    char *homedir;
    char *to_dir;

    if (!r) {
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_UNKNOWN_ERR, "cd");
        return EXIT_FAILURE;
    }

    if ( ! r->args[0]) {
        ctx_myprintf(1, ctx, M_BUILTIN_CMD_CD_UNKNOWN_ERR);
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_CD_UNKNOWN_ERR_ARGS0_EMPTY);
        return EXIT_FAILURE;
    }

    /*
        cd "directory"      : change to directory
        cd -                : change to the previous directory
        cd ~                : change to the homedir of the current user
    */

    to_dir = NULL;
    if (r->args[1] && r->args[1][0] != '\0' && (strcmp (r->args[1], "~") != 0)) {

        if ((strcmp(r->args[1], "-") == 0) && (ctx->previous_dir != NULL)) {
            to_dir = strdup(ctx->previous_dir);
        } else {
            to_dir = strdup(r->args[1]);
        }
    } else {
        if ((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }
        if (homedir) {
            freeif(to_dir);
            to_dir = strdup(homedir);
        }
    }

    if (to_dir) {
        int result;
        char buff[BUFFER_SIZE];
        ctx_dbmyprintf(2, ctx, M_BUILTIN_CMD_CD_ATTEMPTING, to_dir);

        if (getcwd(buff, BUFFER_SIZE) != NULL) {
            freeif(ctx->previous_dir);
            ctx->previous_dir = strdup(buff);
        }

        if ((result = chdir(to_dir)) != 0) {
            ctx_myprintf(1, ctx, M_BUILTIN_CMD_CD_ERR);
            ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_CD_ERR_DETAILS, result);
            return EXIT_FAILURE;
        }

        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_CD_OK_CHANGED, to_dir);
        /* we have changed of directory :
        we can set the new directory in the shell prompt */
        mysh_prompt_set_with_new(ctx, strcat_dup(to_dir, "#"));
    }

    freeif(to_dir);
    return EXIT_SUCCESS;
}

/**
 * \fn int builtin_cmd_pwd (mysh_context_p ctx, cmdoper_p oper)
 * \brief Commande Built-in "pwd" : affiche var. contenant chemin répertoire courant
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
int builtin_cmd_pwd (mysh_context_p ctx, cmdredir_p r) {

    char buff[BUFFER_SIZE];

    if (!r) {
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_UNKNOWN_ERR, "pwd");
        return EXIT_FAILURE;
    }

    if (getcwd(buff, BUFFER_SIZE) == NULL) {
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_PWD_ERR);
        ctx_dbmyprintf(2, ctx, M_BUILTIN_CMD_PWD_ERR_DETAILS, errno, strerror(errno));
        return EXIT_FAILURE;
    }

    printf("%s\n", buff);
    return EXIT_SUCCESS;
}

/**
 * \fn int builtin_cmd_exit (mysh_context_p ctx, cmdoper_p oper)
 * \brief Commande Built-in "exit" : elle fait quitter le programme.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
int builtin_cmd_exit (mysh_context_p ctx, cmdredir_p r) {

    cmdoper_p oprev;

    if (!r) {
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_UNKNOWN_ERR, "exit");
        return EXIT_FAILURE;
    }

    if (r->oper && ((oprev = r->oper->prev) != NULL)) {
        if ((oprev->type == CMDOPER_AND) && (oprev->exitstatus == EXIT_SUCCESS)) {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_OPER_AND);
        } else if ((oprev->type == CMDOPER_OR) && (oprev->exitstatus == EXIT_FAILURE)) {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_OPER_OR);
        } else if (oprev->type == CMDOPER_SEMICOLON) {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_OPER_SCOLON);
        } else {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT);
        }
    } else {
        ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_NO_PREVCMD);
    }

    if (r->args && r->args[0] && r->args[1]) {
        int s = atoi(r->args[1]);
        ctx_dbmyprintf(1, ctx, M_CMDHANDLE_EXIT_WITH_STATUS, s);
        ctx->exitstatus = s;
    }
    ctx->status = CTX_STATUS_EXIT;
    return EXIT_SUCCESS;
}

/**
 * \fn int builtin_cmd_echo (mysh_context_p ctx, cmdoper_p oper)
 * \brief Commande Built-in "echo" : elle affiche un message sur stdin.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 *
 * Cette commande builtin 'echo' affiche sur stdin les paramètres qu'on
 * lui passe en paramètre. Si le 1er paramètre est '-n', aucune ligne n'est
 * sautée en fin d'affichage.
 */
int builtin_cmd_echo (mysh_context_p ctx, cmdredir_p r) {

    int line_feed = true;
    int i;

    if (!r) {
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_UNKNOWN_ERR, "echo");
        return EXIT_FAILURE;
    }

    /* start at 1 because '0' arg is the 'echo' command. */
    for (i=1; r->args[i] != NULL; i++) {
        if ((i == 1) && (strcmp (r->args[i], "-n") == 0)) {
            line_feed = false;
            continue;
        }
        printf("%s", r->args[i]);
        if (r->args[i+1] != NULL) { printf(" "); }
    }

    if (line_feed) {
        printf("\n");
    }

    return EXIT_SUCCESS;
}

/**
 * \fn int builtin_cmd_alias (mysh_context_p ctx, cmdoper_p oper)
 * \brief Commande Built-in "alias" : gestion des alias
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 *
 * Cette commande alias permet :
 *  - de définir ou redéfinir un alias,
 *  - d'afficher la valeur d'un alias (la commande correspondante),
 *  - d'afficher l'ensemble des alias définis.
 */
int builtin_cmd_alias (mysh_context_p ctx, cmdredir_p r) {

    if (!r) {
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_UNKNOWN_ERR, "alias");
        return EXIT_FAILURE;
    }

    if ( ! r->args[0]) {
        ctx_myprintf(1, ctx, M_BUILTIN_CMD_CD_UNKNOWN_ERR);
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_CD_UNKNOWN_ERR_ARGS0_EMPTY);
        return EXIT_FAILURE;
    }

    if (r->args[1] && r->args[1][0] != '\0') {

        char *newalias, *newcmd;
        newalias = newcmd = NULL;
        if (cmdalias_parse_cmd(ctx, r->args[1], &newalias, &newcmd)) {

            if (newalias) {
                cmdalias_p found;
                ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_ALIAS_SEARCH, newalias);
                if ((found = cmdalias_search(ctx, newalias)) != NULL) {
                    ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_ALIAS_FOUND, found->name, found->cmd);
                    if (newcmd) {
                        /* redefine an existing alias */
                        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_ALIAS_REDEFINE, found->name, newcmd);
                        cmdalias_redefine_cmd(found, newcmd);
                    } else {
                        /* if no new value, print the current value of alias */
                        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_ALIAS_PRINT_ONE, found->name, found->cmd);
                        cmdalias_print_one(found);
                    }
                } else {
                    if (newcmd) {
                        /* define a new alias */
                        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_ALIAS_ADD, newalias, newcmd);
                        cmdalias_add(ctx, newalias, newcmd);
                    } else {
                        printf(M_CMDALIAS_ALIAS_NOT_DEFINED, newalias);
                    }
                }
            }
            freeif(newalias);
            freeif(newcmd);
        }
    } else {
        /* print all alias */
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_ALIAS_PRINT_ALL, ctx->nalias);
        cmdalias_print(ctx);
    }

    return EXIT_SUCCESS;
}

/**
 * \fn int builtin_cmd_history (mysh_context_p ctx, cmdredir_p r)
 * \brief Commande Built-in "history" : affichage de l'historique
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
int builtin_cmd_history (mysh_context_p ctx, cmdredir_p r) {

    if (!r) {
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_UNKNOWN_ERR, "history");
        return EXIT_FAILURE;
    }

    if ( ! r->args[0]) {
        ctx_myprintf(1, ctx, M_BUILTIN_CMD_HISTORY_UNKNOWN_ERR);
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_HISTORY_UNKNOWN_ERR_ARGS0_EMPTY);
        return EXIT_FAILURE;
    }

    mysh_history_print(ctx);

    return EXIT_SUCCESS;
}
