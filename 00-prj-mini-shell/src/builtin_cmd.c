/**
 * \file builtin_cmd.c
 * \brief Fonctionnalités Built-in (référencées au sein d'un tableau dans le header associé)
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "builtin_cmd.h"


/**
 * \fn int builtin_loop (mysh_context_p ctx, cmdoper_p o)
 * \brief Cherche si cmd présente parmi les builtin et exécute la fonction le cas échéant.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 *
 * \return found : retourne vrai si une commande built-in a été trouvée (et exécutée) et faux sinon.
 */
int builtin_loop_scan (mysh_context_p ctx, cmdredir_p r) {
    int i;
    int found = false;

    for(i=0; builtin_list[i].cb != NULL; i++) {
        if (strcmp(r->args[0], builtin_list[i].cmd) == 0) {
            ctx_dbmyprintf(1, ctx, M_BUILTIN_PROCESS_BUILTIN_CMD, r->args[0]);
            //r->oper->exitstatus = EXIT_FAILURE; /* EXIT_FAILURE by default before executing cmd */
            r->oper->exitstatus = builtin_list[i].cb(ctx, r);
            r->oper->cmdexec = true;
            found = true;
            break;
        }
    }
    return found;
}

/**
 * \fn int builtin_cmd_cd (mysh_context_p ctx, cmdoper_p oper)
 * \brief Commande Built-in "cd" : elle fait changer de dossier courant.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
int builtin_cmd_cd (mysh_context_p ctx, cmdredir_p r) {

    if (!r) {
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_UNKNOWN_ERR, "cd");
        return EXIT_FAILURE;
    }

    if ( ! r->args[0]) {
        ctx_myprintf(1, ctx, M_BUILTIN_CMD_CD_UNKNOWN_ERR);
        ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_CD_UNKNOWN_ERR_ARGS0_EMPTY);
        return EXIT_FAILURE;
    }

    if (!r->args[1] || (r->args[1][0] == '\0')) {
        /* TODO : simple 'cd' command is like 'cd ~' */
    } else {
        int result;
        ctx_dbmyprintf(2, ctx, M_BUILTIN_CMD_CD_ATTEMPTING, r->args[1]);
        if ((result = chdir(r->args[1])) == 0) {
            //r->oper->exitstatus = EXIT_SUCCESS;
            ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_CD_OK_CHANGED, r->args[1]);
            /* we have changed of directory :
            we can set the new directory in the shell prompt */
            mysh_prompt_set_with_new(ctx, strcat_dup(r->args[1], "#"));
        } else {
            /* TODO : errno + details */
            ctx_myprintf(1, ctx, M_BUILTIN_CMD_CD_ERR);
            ctx_dbmyprintf(1, ctx, M_BUILTIN_CMD_CD_ERR_DETAILS, result);
            return EXIT_FAILURE;
        }
    }

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
        /*
        if ((oprev->type == CMDOPER_AND) && (oprev->cmdstatus == true)) {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_OPER_AND);
            ctx->status = CTX_STATUS_EXIT;
        } else if ((oprev->type == CMDOPER_OR) && (oprev->cmdstatus == false)) {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_OPER_OR);
            ctx->status = CTX_STATUS_EXIT;
        } else if (oprev->type == CMDOPER_SEMICOLON) {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_OPER_SCOLON);
            ctx->status = CTX_STATUS_EXIT;
        } else {
            ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT);
            ctx->status = CTX_STATUS_EXIT;
        }
        */
    } else {
        ctx_dbmyprintf(1, ctx, M_CMDHANDLE_OKEXIT_NO_PREVCMD);
        ctx->status = CTX_STATUS_EXIT;
    }

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

    /* TODO :   for better perf, we can build the string in a buffer
                and print on the stdin by using 'printf' only one time */
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
