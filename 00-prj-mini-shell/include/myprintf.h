#ifndef MYPRINTF_H
#define MYPRINTF_H

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
#define M_CMDARGS_PARSER_END_GIVE_ARG   M_CMDARGS_PARSER" End of parser. This cmd [%s] give %d arguments.\n"

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

/* cmdhandle messages */
#define M_CMDHANDLE                     "[cmdhandle]"
#define M_CMDHANDLE_START_ENGINE        M_CMDHANDLE" === Starting the handling engine ===\n"
#define M_CMDHANDLE_HANDLING_CMD        M_CMDHANDLE" Handling command [%s]\n"
#define M_CMDHANDLE_HANDLING_REDIR      M_CMDHANDLE" Handling redirection [%s]\n"
#define M_CMDHANDLE_IN_EXITING_WF       M_CMDHANDLE" Currently in exiting workflow. Ignoring [%s] command.\n"
#define M_CMDHANDLE_FOUND_CMD_EXIT      M_CMDHANDLE" 'exit' command found.\n"
#define M_CMDHANDLE_OK4EXIT             "OK for exiting."
#define M_CMDHANDLE_OKEXIT              M_CMDHANDLE" "M_CMDHANDLE_OK4EXIT"\n"
#define M_CMDHANDLE_OKEXIT_OPER_AND     M_CMDHANDLE" Prev Operator=AND and ReturnCode=true. "M_CMDHANDLE_OK4EXIT"\n"
#define M_CMDHANDLE_OKEXIT_OPER_OR      M_CMDHANDLE" Prev Operator=OR and ReturnCode=false. "M_CMDHANDLE_OK4EXIT"\n"
#define M_CMDHANDLE_OKEXIT_OPER_SCOLON  M_CMDHANDLE" Prev Operator=SEMICOLON. "M_CMDHANDLE_OK4EXIT"\n"
#define M_CMDHANDLE_OKEXIT_NO_PREVCMD   M_CMDHANDLE" No Previous Command. "M_CMDHANDLE_OK4EXIT".\n"


#endif /* MYPRINTF_H */
