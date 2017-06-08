 /**
  * \file mysh_history.c
  * \brief Gestion de l'historique des commandes passées dans mysh.
  * \author Fabien VANTARD
  * \version 0.1
  *
  * Les fonctions mysh_history_* permettent de gérer l'historique des commandes
  * passées dans le programme mysh. Chaque instance de mysh lancée utilise son
  * fichier temporaire propre afin d'y stocker les commandes qu'elle reçoit.
  * Lorsque l'instance se termine, mysh se charge de 'flusher' la liste des
  * commandes historisées de cette instance dans le fichier historique
  * officiel (~/.mysh_history).
  * Ainsi, ce comportement est similaire à celui du shell BASH. Cela permet
  * de grouper les séquences de commandes par instance et ne pas tout mélanger.
  */

#include "mysh_history.h"

/**
 * \fn void mysh_history_open_temp_file(mysh_history_p h, mysh_context_p ctx)
 * \brief Ouverture du fichier temporaire dans le contexte mysh_context.
 *
 * \param h Pointeur sur l'objet mysh_history_p
 * \param ctx Pointeur sur le contexte mysh_context
 */
void mysh_history_open_temp_file(mysh_history_p h, mysh_context_p ctx) {
    if (h) {
        pid_t selfpid = getpid();
        char path[PATH_MAX];

        memset(path, 0, PATH_MAX);
        snprintf(path, PATH_MAX, "/tmp/%s-%d_XXXXXX", basename(ctx->argv[0]), selfpid);

        if ((h->temp_fd = mkstemp(path)) <0) {
            perror("mkstemp");
            free(h);
            exit(EXIT_FAILURE);
        }

        h->temp_path = strndup(path, strlen(path));
    }
}

/**
 * \fn void mysh_history_close_temp_file(mysh_context_p ctx)
 * \brief Fermeture du fichier temporaire propre au mysh_context.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 */
void mysh_history_close_temp_file(mysh_context_p ctx) {
    if (ctx) {
        if (ctx->history->temp_fd != -1) {
            close(ctx->history->temp_fd);
            ctx->history->temp_fd = -1;
        }
        if (ctx->history->temp_path) {
            // deletion effective when fd closed or program ends.
    		unlink(ctx->history->temp_path);
            free(ctx->history->temp_path);
            ctx->history->temp_path = NULL;
        }
    }
}

/**
 * \fn void mysh_history_init(mysh_history_p h, mysh_context_p ctx)
 * \brief Initialisation objet mysh_history
 *
 * \param h Pointeur sur l'objet mysh_history_p
 * \param ctx Pointeur sur le contexte mysh_context
 */
void mysh_history_init(mysh_history_p h, mysh_context_p ctx) {
    mysh_history_open_temp_file(h, ctx);
}

/**
 * \fn mysh_history_p mysh_history_new(mysh_context_p ctx)
 * \brief Constructeur objet mysh_history (sur la base du mysh_context)
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \return Objet mysh_history nouvellement créé
 */
mysh_history_p mysh_history_new(mysh_context_p ctx) {
    mysh_history_p h;
    h = ctx->history = (mysh_history_p) malloc (sizeof(mysh_history_t));
    mysh_history_init(ctx->history, ctx);
    return h;
}

/**
 * \fn void mysh_history_add(mysh_context_p ctx, char *cmd)
 * \brief Ajoute une commande à l'historique de session
 *
 * \param ctx Pointeur sur contexte mysh_context
 * \param cmd Chaîne de caractères à ajouter à l'historique de session
 */
void mysh_history_add(mysh_context_p ctx, char *cmd) {
    if (ctx && cmd && *cmd != '\0') {
        ssize_t n_write = 0;
        n_write += write(ctx->history->temp_fd, cmd, strlen(cmd));
        n_write += write(ctx->history->temp_fd, "\n", 1);
    }
}

/**
 * \fn void mysh_history_flush(mysh_context_p ctx)
 * \brief 'Flushe' historique de session ds historique principal/unique de mysh
 *
 * \param ctx Pointeur sur contexte mysh_context
 *
 * Cette gestion d'historique par un historique de session donne la
 * possibilité d'utiliser plusieurs instances de mysh.
 */
void mysh_history_flush(mysh_context_p ctx) {

    char path[PATH_MAX];
    int fd;
    const char *homedir;

    /* get home directory of current user :
       1/first : $HOME if exist
       2/default: official homedir if $HOME not exist */
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    snprintf(path, PATH_MAX, "%s/%s", homedir, MYSH_HISTORY_DEFAULT_FILENAME);
    if ((fd = open(path, O_RDWR|O_APPEND|O_CREAT, 00744)) != -1) {

        #define BUFFER_SIZE 1024
        char buffer[BUFFER_SIZE+1];
        ssize_t n_read;

        lseek(ctx->history->temp_fd, 0, SEEK_SET);
        while ((n_read = read(ctx->history->temp_fd, &buffer, (size_t)BUFFER_SIZE)) > 0) {
            buffer[n_read] = '\0';
            write(fd, &buffer, n_read);
        }
        close (fd);

        mysh_history_close_temp_file(ctx);

        /* opening another temporary session history file by default */
        mysh_history_init(ctx->history, ctx);
    }
}

/**
 * \fn void mysh_history_free(mysh_context_p ctx)
 * \brief Destructeur de l'objet mysh_history.
 *
 * \param ctx Pointeur sur contexte mysh_context
 *
 * Ce destructeur flushe une dernière fois l'historique de session dans
 * l'historique principal. Puis il ferme le nouveau fichier d'historique de
 * session nouvellement ouvert lors de ce dernier flush.
 */
void mysh_history_free(mysh_context_p ctx) {
    mysh_history_flush(ctx);
    mysh_history_close_temp_file(ctx);
    free(ctx->history);
}
