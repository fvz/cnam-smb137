/**
 * \file cmdfork.c
 * \brief Traitement des forks pour execution des commandes built-in ou non
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "cmdfork.h"

/**
 * \fn void cmdfork_exec(mysh_context_p ctx, cmdredir_p r)
 * \brief Fork le programme pour exécuter une commande builtin ou non.
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
void cmdfork_exec(mysh_context_p ctx, cmdredir_p r) {

    cmdoper_p o = r->oper;
    ctx_dbmyprintf(1, ctx, M_CMDFORK_FORKING_CMD, o->cmd);

    int status;
    pid_t child, ppid;

    ppid = getpid();
    child = fork();

    if (child == 0) { /* child */
        ctx_dbmyprintf(1, ctx, M_CMDFORK_INCHILD_WORK_EXEC, getpid(), r->cmd);
        if (builtin_loop_scan (ctx, r)) {
            /* a builtin cmd has been found and executed. */
        } else {
            execvp(r->args[0], r->args);
            printf("execlp failed %s\n", strerror(errno));
        }
    } else { /* parent */
        ctx_dbmyprintf(1, ctx, M_CMDFORK_FORK_TO_CHILD_WAIT, ppid, child);
        waitpid(child, &status, 0);
        ctx_dbmyprintf(1, ctx, M_CMDFORK_END_OF_WAIT, ppid, child, WEXITSTATUS(status));
        o->exitstatus = WEXITSTATUS(status);
        o->cmdexec = true;
    }
}







/**
 * \fn void cmdfork_pipe(mysh_context_p ctx, cmdredir_p r)
 * \brief
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
void cmdfork_pipe(mysh_context_p ctx, cmdredir_p r) {

    int pfd[2];

    printf("=== [%s] | [%s] ===\n", r->args[0], r->next->args[0]);

    /* create the pipe */
    if (pipe(pfd) == -1) {
        printf("pipe failed\n");
        //return 1;
    }

    /* create the child */
    int  pid;
    if ((pid = fork()) < 0) {
        printf("fork failed\n");
        //return 2;
    }

    if (pid == 0) {
        /* child */
        close(pfd[1]); /* close the unused write side */
        dup2(pfd[0], 0); /* connect the read side with stdin */
        close(pfd[0]); /* close the read side */
        /* execute the process (wc command) */
        execvp(r->next->args[0], r->args);
        printf("%s failed", r->next->args[0]); /* if execlp returns, it's an error */
    } else {
        /* parent */
        close(pfd[0]); /* close the unused read side */
        dup2(pfd[1], 1); /* connect the write side with stdout */
        close(pfd[1]); /* close the write side */
        /* execute the process (ls command) */
        execvp(r->args[0], r->args);
        printf("%s failed", r->args[0]); /* if execlp returns, it's an error */
    }
}






void cmdfork_pipe_run1(int pfd[], char **args) {
    int pid;	/* we don't use the process ID here, but you may wnat to print it for debugging */
	switch (pid = fork()) {
	case 0: /* child */
		dup2(pfd[1], 1);	/* this end of the pipe becomes the standard output */
		close(pfd[0]); 		/* this process don't need the other end */
		execvp(args[0], args);	/* run the command */
		perror(args[0]);	/* it failed! */
	default: /* parent does nothing */
		break;
	case -1:
		perror("fork");
		exit(1);
	}
}

void cmdfork_pipe_run2(int pfd[], char **args) {
    int pid;	/* we don't use the process ID here, but you may wnat to print it for debugging */
	switch (pid = fork()) {
	case 0: /* child */
		dup2(pfd[1], 1);	/* this end of the pipe becomes the standard output */
		close(pfd[0]); 		/* this process don't need the other end */
		execvp(args[0], args);	/* run the command */
		perror(args[0]);	/* it failed! */
	default: /* parent does nothing */
		break;
	case -1:
		perror("fork");
		exit(1);
	}
}

void cmdfork_pipe2(mysh_context_p ctx, cmdredir_p r) {

    int pid, status;
	int fd[2];

	pipe(fd);

	cmdfork_pipe_run1(fd, r->args);
	cmdfork_pipe_run2(fd, r->next->args);
	close(fd[0]); close(fd[1]); 	/* this is important! close both file descriptors on the pipe */

	while ((pid = wait(&status)) != -1)	/* pick up all the dead children */
		fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
	exit(0);
}







void cmdfork_pipe3(mysh_context_p ctx, cmdredir_p r) {

    int p[2];
    pipe(p); // Creates a pipe with file descriptors Eg. input = 3 and output = 4 (Since, 0,1 and 2 are not available)

    if (fork() == 0) {
    // Child process
        close(0); // Release fd no - 0
        close(p[0]); // Close pipe fds since useful one is duplicated
        close(p[1]);
        //dup(p[0]); // Create duplicate of fd - 3 (pipe read end) with fd 0.
        dup2(p[0], 0); // Create duplicate of fd - 3 (pipe read end) with fd 0.
        //exec("cmd2");
        execvp(r->next->args[0], r->next->args);
    } else {
        //Parent process
        close(1); // Release fd no - 1
        close(p[0]); // Close pipe fds since useful one is duplicated
        close(p[1]);
        //dup(p[1]); // Create duplicate of fd - 4 (pipe write end) with fd 1.
        dup2(p[1], 1); // Create duplicate of fd - 4 (pipe write end) with fd 1.
        //exec("cmd1");
        execvp(r->args[0], r->args);
    }
}






void cmdfork_pipe4(mysh_context_p ctx, cmdredir_p r) {

    printf("=== [%s] | [%s] ===\n", r->args[0], r->next->args[0]);


    //open a pipe, check that it was successfully created
    int pip[2];
    if(pipe(pip)<0){
      perror("pipe failure");
      //return -1;
    }

    //attempt to fork
    int parentstatus=0;
    int childstatus=0;
    switch(fork()){

      //fork failed, close the pipe and return -1 (failure)
      case -1:
        {
          close(pip[0]);
          close(pip[1]);
          perror("fork failure");
          //return -1;
        }

      //pid of 0 indicates this is the child process
      case 0:
        {
          //close the write end of the pipe and redirect stdin to the read end
          close(pip[0]);
          int stdIn=dup(0);
          dup2(pip[1],0);

          //once redirect is done, can close the other end of the pipe
          close(pip[1]);

          //parse the second line as a list of commands (not important for question)
          //then restore stdin, close the old file descriptor pointing to the pipe
          //execute_line(secondline,builtins);
          execvp(r->next->args[0], r->next->args);
          dup2(stdIn,0);
          close(stdIn);
          break;
        }

      //pid other than 0 indicates this is the parent process
      default:
        {
          //close the read end of the pipe
          close(pip[1]);

          //redirect stdout to the write end of the pipe
          int stdOut=dup(1);
          dup2(pip[0],1);
          close(pip[0]);

          //execute this line (not important for question)
          //parentstatus=execute_line(firstline,builtins);
          execvp(r->args[0], r->args);

          //restore stdout and close the temporary fd pointing to the pipe
          dup2(stdOut,1);
          close(stdOut);

          //wait for child process to exit and store it's return value in 'status' (then childstatus)
          int status;
          wait(&status);
          childstatus=status;
          break;
        }
    }

    printf ("%d %d\n", parentstatus, childstatus);
}









/**
 * \fn void cmdfork_truncat(mysh_context_p ctx, cmdredir_p r)
 * \brief
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
void cmdfork_truncat(mysh_context_p ctx, cmdredir_p r) {
    int fd;
    //int fd_stdout;
    if (r->next && r->next->args) {
        ctx_dbmyprintf(1, ctx, M_CMDHANDLE_REDIR_TRUNCAT_TO, r->next->args[0]);
        fd = open(r->next->args[0], O_TRUNC | O_WRONLY);

        //fd_stdout = dup(fileno(stdout));
        dup2(fd, fileno(stdout));
        close(fd);
    }
}

/**
 * \fn void cmdfork_append(mysh_context_p ctx, cmdredir_p r)
 * \brief
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
void cmdfork_append(mysh_context_p ctx, cmdredir_p r) {
    int fd;
    //int fd_stdout;
    if (r->next && r->next->args) {
        ctx_dbmyprintf(1, ctx, M_CMDHANDLE_REDIR_TRUNCAT_TO, r->next->args[0]);
        fd = open(r->next->args[0], O_RDWR|O_APPEND|O_CREAT, 00744);
        //fd_stdout = dup(fileno(stdout));
        dup2(fd, fileno(stdout));
        close(fd);
    }
}



/**
 * \fn void cmdfork_input(mysh_context_p ctx, cmdredir_p r)
 * \brief
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
void cmdfork_input(mysh_context_p ctx, cmdredir_p r) {
    /* TODO */
}

/**
 * \fn void cmdfork_dinput(mysh_context_p ctx, cmdredir_p r)
 * \brief
 *
 * \param ctx Pointeur sur le contexte mysh_context
 * \param r Pointeur sur le CmdRedir en cours
 */
void cmdfork_dinput(mysh_context_p ctx, cmdredir_p r) {
    /* TODO */
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
