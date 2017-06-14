#ifndef MYSH_TYPEDEF_H_
#define MYSH_TYPEDEF_H_



/* history
 */
typedef struct mysh_history_s mysh_history_t;
typedef mysh_history_t* mysh_history_p;
struct mysh_history_s {
    int temp_fd;
    char *temp_path;
};


/* context
*/
typedef struct mysh_context_s mysh_context_t;
typedef mysh_context_t* mysh_context_p;
struct mysh_context_s {

    int argc; char **argv;

    char *cmd;

	int debug_level;
	int verbose_level;

    mysh_history_p history;

    int status;
};

/* context status */
#define CTX_STATUS_EXIT     0x00    /* program in exiting stage */
#define CTX_STATUS_LOOP     0x01    /* program in looping stage */



/* cmdredir : a command redirection (string of command associated to a redirection operator like '>', '>>', '<', '<<')
 */
typedef struct cmdredir_s cmdredir_t;
typedef cmdredir_t* cmdredir_p;
struct cmdredir_s {
    char *cmd;
    int redir;
    cmdredir_p prev;
    cmdredir_p next;

    char **args;
    int nargs;

    int status;
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
typedef struct cmdoper_s cmdoper_t;
typedef cmdoper_t* cmdoper_p;
struct cmdoper_s {
    char *cmd;
    int oper;
    cmdredir_p redir;
    cmdoper_p prev;
    cmdoper_p next;

    int cmdstatus;
};

#define CMDOPER_EMPTY      0x00    /* empty operator */
#define CMDOPER_SEMICOLON  0x01    /* ';'  : semicolon operator */
#define CMDOPER_OR         0x02    /* '||' : logic OR operator */
#define CMDOPER_BACKGND    0x04    /* '&'  : background operator */
#define CMDOPER_AND        0x08    /* '&&' : logic AND operator */






typedef struct builtincmd_s builtincmd_t;
typedef builtincmd_t* builtincmd_p;
struct builtincmd_s {
    /* command name */
    char *cmd;

    /* callback to call if this command is found */
    void (*cb)(mysh_context_p ctx, cmdoper_p oper);
};







#endif /* MYSH_TYPEDEF_H_ */
