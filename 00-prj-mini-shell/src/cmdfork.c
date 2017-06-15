/**
 * \file cmdhandle.c
 * \brief Traitement des forks pour execution des commandes non built-in
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "cmdfork.h"

/**
 * \fn int cmdfork_do(mysh_context_p ctx, cmdredir_p r)
 * \brief Fork le programme pour exécuter une commande non builtin.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 * \return true si OK
 */
int cmdfork_do(mysh_context_p ctx, cmdredir_p r) {

    cmdoper_p o = r->oper;
    ctx_dbmyprintf(1, ctx, M_CMDFORK_FORKING_CMD, o->cmd);

    int status;
    pid_t child, ppid;

    ppid = getpid();
    child = fork();

    if (child == 0) { /* executed by child */
        ctx_dbmyprintf(1, ctx, M_CMDFORK_INCHILD_WORK_EXEC, getpid(), r->cmd);
        execvp(r->args[0], r->args);
        //perror("execlp");

    } else { /* executed by parent */
        ctx_dbmyprintf(1, ctx, M_CMDFORK_FORK_TO_CHILD_WAIT, ppid, child);
        waitpid(child, &status, 0);
        ctx_dbmyprintf(1, ctx, M_CMDFORK_END_OF_WAIT, ppid, child, WEXITSTATUS(status));
        o->exitstatus = WEXITSTATUS(status);
        o->cmdexec = true;
    }

    return true;
}



#if 0
int
main(int argc, char *argv[])
{
    pid_t cpid, w;
    int status;

   cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

   if (cpid == 0) {            /* Code executed by child */
        printf("Child PID is %ld\n", (long) getpid());
        if (argc == 1)
            pause();                    /* Wait for signals */
        _exit(atoi(argv[1]));

   } else {                    /* Code executed by parent */
        do {
            w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
            if (w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

           if (WIFEXITED(status)) {
                printf("exited, status=%d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("killed by signal %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("stopped by signal %d\n", WSTOPSIG(status));
            } else if (WIFCONTINUED(status)) {
                printf("continued\n");
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        exit(EXIT_SUCCESS);
    }
}
#endif












/*
            switch(r->redir) {
                case CMDREDIR_EMPTY:
                    break;
                case CMDREDIR_PIPE:
                    break;
                case CMDREDIR_TRUNCAT:
                    break;
                case CMDREDIR_APPEND:
                    break;
                case CMDREDIR_INPUT:
                    break;
                case CMDREDIR_DINPUT:
                    break;
                default:
                    break;
            }
*/



#if 0
int main(void)
{
    pid_t p = fork();
    if ( p == -1 ) {
        perror("fork failed");
        return EXIT_FAILURE;
    }
    else if ( p == 0 ) {
        execl("/bin/sh", "bin/sh", "-c", "./failprog", "NULL");
        return EXIT_FAILURE;
    }

    int status;
    if ( waitpid(p, &status, 0) == -1 ) {
        perror("waitpid failed");
        return EXIT_FAILURE;
    }

    if ( WIFEXITED(status) ) {
        const int es = WEXITSTATUS(status);
        printf("exit status was %d\n", es);
    }

    return EXIT_SUCCESS;
}
#endif






#if 0
int main(int argc, char** argv)
{
	pid_t pere, fils;
	int cr;

	pere = getpid();
	fils = fork();

	if (fils == 0) {
		pid_t p = getpid();
		cr = p%10;
		printf("[FILS]  PID=[%d]  PPID=[%d]  CR=[%d]\n", p, pere, cr);
		return cr;
	} else {
		cr = 0;
		printf("[PERE]  PID-fils=[%d]  --> attente terminaison (wait)...\n", fils);

		wait(&cr);
		printf("[PERE]  PID-fils=[%d] CR=[%d]\n", fils, cr>>8);
	}

	return EXIT_SUCCESS;
}
#endif
