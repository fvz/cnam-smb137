/**
 * \file myprintf.h
 * \brief Ensemble de fonctions d'affichage (verbose/debug).
 * \author Fabien VANTARD
 * \version 0.1
 */

#ifndef MYPRINTF_H_
#define MYPRINTF_H_

#include "mysh.h"

void vmyprintf_stream(FILE * stream, int threshold, int level, const char *format, va_list args);
void myprintf_stream(FILE * stream, int threshold, int level, const char *format, ...);
#define myprintf(threshold, level, ...) \
    myprintf_stream(stdout, threshold, level, __VA_ARGS__)
#define myprintf_err(threshold, level, ...) \
    myprintf_stream(stderr, threshold, level, __VA_ARGS__)

void ctx_myprintf_stream(FILE * stream, int threshold, mysh_context_p ctx, const char *format, ...);
#define ctx_myprintf(threshold, ctx, ...) \
    ctx_myprintf_stream(stdout, threshold, ctx, __VA_ARGS__ )
#define ctx_myprintf_err(threshold, ctx, ...) \
    ctx_myprintf_stream(stderr, threshold, ctx, __VA_ARGS__ )

void ctx_dbmyprintf_stream(FILE * stream, int threshold, mysh_context_p ctx, const char *format, ...);
#define ctx_dbmyprintf(threshold, ctx, ...) \
    ctx_dbmyprintf_stream(stdout, threshold, ctx, __VA_ARGS__ )
#define ctx_dbmyprintf_err(threshold, ctx, ...) \
    ctx_dbmyprintf_stream(stderr, threshold, ctx, __VA_ARGS__ )



/* common messages */
#define M_COMMON_ANALYZE_CHAR           "Analyzing character [%c]"
#define M_COMMON_POINTER_CHAIN          "pointers : prev=[%p] cur=[%p] next[%p]"
#define M_COMMON_CURSOR_IS              "Cursor string is on [%s]"

/* cmdargs_parser messages */
#define M_CMDARGS_PARSER                "[cmdargs_parser]"
#define M_CMDARGS_PARSER_PARSE_ARGS     M_CMDARGS_PARSER" Parse args of cmdline [%s]\n"
#define M_CMDARGS_PARSER_CURSOR_IS      M_CMDARGS_PARSER" "M_COMMON_CURSOR_IS"\n"
#define M_CMDARGS_PARSER_ANALYZE_CHAR   M_CMDARGS_PARSER" "M_COMMON_ANALYZE_CHAR"\n"
#define M_CMDARGS_PARSER_FOUND_ARG      M_CMDARGS_PARSER" Found new #%d argument [%s]\n"
#define M_CMDARGS_PARSER_END_GIVE_ARG   M_CMDARGS_PARSER" End of parser. This cmd [%s] give %d argument(s).\n"

/* cmdoper_parser messages */
#define M_CMDOPER_PARSER                "[cmdoper]"
#define M_CMDOPER_PARSER_CMD            "cmd[%s] operator[%s]"
#define M_CMDOPER_PARSER_PRINT_PTR      M_CMDOPER_PARSER" "M_CMDOPER_PARSER_CMD" / "M_COMMON_POINTER_CHAIN"\n"
#define M_CMDOPER_PARSER_PRINT          M_CMDOPER_PARSER" "M_CMDOPER_PARSER_CMD"\n"
#define M_CMDOPER_PARSER_PARSE_CMDLINE  M_CMDOPER_PARSER" Parse cmdline [%s]\n"
#define M_CMDOPER_PARSER_CURSOR_IS      M_CMDOPER_PARSER" "M_COMMON_CURSOR_IS"\n"
#define M_CMDOPER_PARSER_ANALYZE_CHAR   M_CMDOPER_PARSER" "M_COMMON_ANALYZE_CHAR"\n"

/* cmdredir_parser messages */
#define M_CMDREDIR_PARSER               "[cmdredir]"
#define M_CMDREDIR_PARSER_CMD           "cmd[%s] redirector[%s]"
#define M_CMDREDIR_PARSER_PRINT_PTR     M_CMDREDIR_PARSER" "M_CMDREDIR_PARSER_CMD" / "M_COMMON_POINTER_CHAIN"\n"
#define M_CMDREDIR_PARSER_PRINT         M_CMDREDIR_PARSER" "M_CMDREDIR_PARSER_CMD"\n"
#define M_CMDREDIR_ANALYZE_CHAR         M_CMDREDIR_PARSER" "M_COMMON_ANALYZE_CHAR"\n"

/* mysh_context messages */
/* --N/A-- */

/* mysh_history messages */
/* --N/A-- */

/* mysh_prompt messages */
/* --N/A-- */

/* cmdalias messages */
#define M_CMDALIAS                      "[cmdalias]"
#define M_CMDALIAS_PARSER_PARSE_CMD     M_CMDALIAS" Parse arg [%s] to get alias\n"
#define M_CMDALIAS_PARSER_CURSOR_IS     M_CMDALIAS" "M_COMMON_CURSOR_IS"\n"
#define M_CMDALIAS_PARSER_ANALYZE_CHAR  M_CMDALIAS" "M_COMMON_ANALYZE_CHAR"\n"
#define M_CMDALIAS_PARSER_NAME_FOUND    M_CMDALIAS" Found alias name [%s]\n"
#define M_CMDALIAS_PARSER_CMD_FOUND     M_CMDALIAS" Found alias cmd [%s]\n"
#define M_CMDALIAS_PARSER_END_GIVE      M_CMDALIAS" End of parser. This arg [%s] give the [%s]=[%s] alias.\n"
#define M_CMDALIAS_ALIAS_DEFINITION     "alias %s='%s'\n"
#define M_CMDALIAS_NO_ALIAS_DEFINED     "No aliases defined.\n"
#define M_CMDALIAS_ALIAS_NOT_DEFINED    "alias %s is not defined.\n"

/* cmdhandle messages */
#define M_CMDHANDLE                     "[cmdhandle]"
#define M_CMDHANDLE_START_PARSER        M_CMDHANDLE" === Starting the parser engine ============\n"
#define M_CMDHANDLE_START_ENGINE        M_CMDHANDLE" === Starting the handling engine ============\n"
#define M_CMDHANDLE_END_ENGINE          M_CMDHANDLE" === Ending the handling engine ============\n"
#define M_CMDHANDLE_HANDLING_CMD        M_CMDHANDLE" Handling command [%s]\n"
#define M_CMDHANDLE_HANDLING_REDIR      M_CMDHANDLE" Handling redirection [%s]\n"
#define M_CMDHANDLE_ALIAS_FOUND         M_CMDHANDLE" Alias found for [%s] command : "M_CMDALIAS_ALIAS_DEFINITION
#define M_CMDHANDLE_IN_EXITING_WF       M_CMDHANDLE" Currently in exiting workflow. Ignoring [%s] command.\n"
#define M_CMDHANDLE_OK4EXIT             "OK for exiting."
#define M_CMDHANDLE_OKEXIT              M_CMDHANDLE" "M_CMDHANDLE_OK4EXIT"\n"
#define M_CMDHANDLE_OKEXIT_OPER_AND     M_CMDHANDLE" Previous Operator=AND (+ReturnCode=true) : "M_CMDHANDLE_OK4EXIT"\n"
#define M_CMDHANDLE_OKEXIT_OPER_OR      M_CMDHANDLE" Previous Operator=OR (+ReturnCode=false) :"M_CMDHANDLE_OK4EXIT"\n"
#define M_CMDHANDLE_OKEXIT_OPER_SCOLON  M_CMDHANDLE" Previous Operator=SEMICOLON : "M_CMDHANDLE_OK4EXIT"\n"
#define M_CMDHANDLE_OKEXIT_NO_PREVCMD   M_CMDHANDLE" No Previous Command : "M_CMDHANDLE_OK4EXIT"\n"
#define M_CMDHANDLE_EXIT_WITH_STATUS    M_CMDHANDLE" Exiting with exitstatus [%d]\n"
#define M_CMDHANDLE_PREVCMD_OPER_SO_NOEXEC \
                                        M_CMDHANDLE" Previous Oper=%s (+%s) : don't execute [%s] cmd.\n"
#define M_CMDHANDLE_REDIR_TRUNCAT_TO    M_CMDHANDLE" Stdout redirection to [%s] (truncating).\n"
#define M_CMDHANDLE_REDIR_APPEND_TO     M_CMDHANDLE" Stdout redirection to [%s] (appending).\n"


/* builtin messages */
#define M_BUILTIN                       "[builtin]"
#define M_BUILTIN_PROCESS_BUILTIN_CMD   M_BUILTIN" Processing Built-in [%s] command\n"
#define M_BUILTIN_CMD_UNKNOWN_ERR       M_BUILTIN" Built-in [%s] command : unknown error.\n"
#define M_BUILTIN_CMD_CD_UNKNOWN_ERR    M_BUILTIN" Built-in 'cd' command : unknown error.\n"
#define M_BUILTIN_CMD_CD_UNKNOWN_ERR_ARGS0_EMPTY \
                                        M_BUILTIN" Built-in 'cd' command : unknown error (arg[0] empty).\n"
#define M_BUILTIN_CMD_CD_ATTEMPTING     M_BUILTIN" Attempting to change directory to [%s]\n"
#define M_BUILTIN_CMD_CD_OK_CHANGED     M_BUILTIN" Current directory is now [%s]\n"
#define M_BUILTIN_CMD_CD_ERR            M_BUILTIN" Error in changing directory.\n"
#define M_BUILTIN_CMD_CD_ERR_DETAILS    M_BUILTIN" Error in changing directory (result=[%d]).\n"
#define M_BUILTIN_CMD_PWD_ERR           M_BUILTIN" Error in retrieving current directory.\n"
#define M_BUILTIN_CMD_PWD_ERR_DETAILS   M_BUILTIN" Error in retrieving current directory [errno#%d='%s'].\n"
#define M_BUILTIN_CMD_ALIAS_UNKNOWN_ERR M_BUILTIN" Built-in 'alias' command : unknown error.\n"
#define M_BUILTIN_CMD_ALIAS_UNKNOWN_ERR_ARGS0_EMPTY \
                                        M_BUILTIN" Built-in 'alias' command : unknown error (arg[0] empty).\n"
#define M_BUILTIN_CMD_ALIAS_SEARCH      M_BUILTIN" Searching the alias [%s] in list\n"
#define M_BUILTIN_CMD_ALIAS_FOUND       M_BUILTIN" Alias found [%s]=[%s]\n"
#define M_BUILTIN_CMD_ALIAS_REDEFINE    M_BUILTIN" Redefining alias [%s] to [%s] command\n"
#define M_BUILTIN_CMD_ALIAS_PRINT_ONE   M_BUILTIN" Printing alias [%s]=[%s]\n"
#define M_BUILTIN_CMD_ALIAS_ADD         M_BUILTIN" Adding alias [%s]=[%s] to list\n"
#define M_BUILTIN_CMD_ALIAS_PRINT_ALL   M_BUILTIN" Printing all [%d] alias list\n"
#define M_BUILTIN_CMD_HISTORY_UNKNOWN_ERR \
                                        M_BUILTIN" Built-in 'history' command : unknown error.\n"
#define M_BUILTIN_CMD_HISTORY_UNKNOWN_ERR_ARGS0_EMPTY \
                                        M_BUILTIN" Built-in 'history' command : unknown error (arg[0] empty).\n"

/* cmdfork messages */
#define M_CMDFORK                       "[cmdfork]"
#define M_CMDFORK_FORKING_CMD           M_CMDFORK" We have to forking for execute [%s] command.\n"
#define M_CMDFORK_FORK_TO_CHILD_WAIT    M_CMDFORK" PPID[%d] Fork to child [%d] and wait/waitpid.\n"
#define M_CMDFORK_INCHILD_WORK_EXEC     M_CMDFORK" PID [%d](Child) Working and executing [%s] command.\n"
#define M_CMDFORK_END_OF_WAIT           M_CMDFORK" PPID[%d] End of waitpid [%d] + Exitstatus=[%d].\n"


#endif /* MYPRINTF_H_ */
