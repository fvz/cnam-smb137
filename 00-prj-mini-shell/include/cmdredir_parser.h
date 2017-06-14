#ifndef CMDREDIR_PARSER_H_
#define CMDREDIR_PARSER_H_

#include "mysh.h"

cmdredir_p cmdredir_new (cmdoper_p oper, char *cmd, int redic, cmdredir_p prev, cmdredir_p next);
void cmdredir_free (cmdredir_p c);

void cmdredir_print (mysh_context_p ctx, cmdredir_p c);

cmdredir_p cmdredir_parser (mysh_context_p ctx, cmdoper_p oper);
void cmdredir_parse_args (mysh_context_p ctx, cmdredir_p c);

#endif /* CMDREDIR_PARSER_H_ */
