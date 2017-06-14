#ifndef CMDOPER_PARSER_H_
#define CMDOPER_PARSER_H_

#include "mysh.h"

cmdoper_p cmdoper_new (char *cmd, int type, cmdoper_p prev, cmdoper_p next);
void cmdoper_free (cmdoper_p o);

void cmdoper_print_one (mysh_context_p ctx, cmdoper_p o);
void cmdoper_print (mysh_context_p ctx, cmdoper_p o);

cmdoper_p cmdoper_parser (mysh_context_p ctx, char *str);
void cmdoper_parse_redir (mysh_context_p ctx, cmdoper_p o);

#endif /* CMDOPER_PARSER_H_ */
