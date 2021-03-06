/**
 * \file typedef.h
 * \brief l'ensemble des définitions de types
 * \author Fabien VANTARD
 * \version 0.1
 */

#ifndef MYSH_TYPEDEF_H_
#define MYSH_TYPEDEF_H_



/* typedef */

typedef struct mysh_history_s mysh_history_t;
typedef mysh_history_t* mysh_history_p;

typedef struct mysh_context_s mysh_context_t;
typedef mysh_context_t* mysh_context_p;

typedef struct cmdredir_s cmdredir_t;
typedef cmdredir_t* cmdredir_p;

typedef struct cmdoper_s cmdoper_t;
typedef cmdoper_t* cmdoper_p;

typedef struct builtincmd_s builtincmd_t;
typedef builtincmd_t* builtincmd_p;

typedef struct cmdalias_s cmdalias_t;
typedef cmdalias_t* cmdalias_p;




/* history
 */
struct mysh_history_s {
    int temp_fd;            /* file descriptor of history file */
    char *temp_path;        /* filepath of this open history file */
};


/* context
*/
struct mysh_context_s {

    char *prompt;           /* prompt to display after each executed cmdline */

    int argc; char **argv;  /* argc/argv of main */

    char *cmd;              /* command line currently in analyze/execution */

    int debug_level;        /* debug level selected in options */
    int verbose_level;      /* verbose level selected in options */

    mysh_history_p history; /* history file currently open */

    int status;             /* general status of main loop */
    int exitstatus;         /* exit status for exiting program */

    char *previous_dir;     /* previous dir if 'cd' command is used */

    cmdalias_p *alias;      /* array of alias (in this session) */
    int nalias;             /* number of alias in this array */
};

/* context status */
#define CTX_STATUS_EXIT     0x00    /* program in exiting stage */
#define CTX_STATUS_LOOP     0x01    /* program in looping stage */



/* cmdredir : a command redirection (string of command associated to a redirection operator like '>', '>>', '<', '<<')
 */
struct cmdredir_s {

    cmdoper_p oper;     /* cmdoper 'parent' of the cmdredir chain */

    char *cmd;          /* command of this cmdredir */
    int redir;          /* type of redirector */
    cmdredir_p prev;    /* previous cmdredir in the 'redir' chain */
    cmdredir_p next;    /* and next ... */

    char **args;        /* array of arguments (1st (command) is also an argument) */
    int nargs;          /* number of arguments */

    int status;         /* return code of executed command */
};

#define CMDREDIR_EMPTY      0x00    /* empty redirection operator */
#define CMDREDIR_PIPE       0x01    /* |  : pipe redirection operator */
#define CMDREDIR_TRUNCAT    0x02    /* >  : truncat redirection operator */
#define CMDREDIR_APPEND     0x04    /* >> : append redirection operator */
#define CMDREDIR_INPUT      0x08    /* <  : input redirection operator */
#define CMDREDIR_DINPUT     0x10    /* << : ?? */

#define CMDREDIR_STATUS_INIT        0x00    /* object has been initialized */
#define CMDREDIR_STATUS_PENDING     0x01    /* cmd has not been executed yet */
#define CMDREDIR_STATUS_EXEC_OK     0x02    /* cmd has been executed */



/* cmdoper : a command operator  (string of command associated to an operator like '&&', '||', '|', '&')
 */
struct cmdoper_s {
    char *cmd;          /* command of this cmdoper */
    int type;           /* type of cmdoper */
    cmdredir_p redir;   /* chainlist of cmdredir */
    cmdoper_p prev;     /* previous cmdoper in the 'oper' chain */
    cmdoper_p next;     /* and next ... */

    int cmdexec;        /* cmd has been executed (true) or not (false) */
    int exitstatus;     /* return code of command */
};

#define CMDOPER_EMPTY      0x00    /* empty operator */
#define CMDOPER_SEMICOLON  0x01    /* ';'  : semicolon operator */
#define CMDOPER_OR         0x02    /* '||' : logic OR operator */
#define CMDOPER_BACKGND    0x04    /* '&'  : background operator */
#define CMDOPER_AND        0x08    /* '&&' : logic AND operator */






struct builtincmd_s {
    char *cmd; /* command name */
    int (*cb)(mysh_context_p ctx, cmdredir_p r); /* callback to call if this command is found */
};





struct cmdalias_s {
    char *name;     /* alias name */
    char *cmd;      /* and it's commande */
};




#endif /* MYSH_TYPEDEF_H_ */
