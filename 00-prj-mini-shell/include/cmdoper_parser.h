#ifndef CMDOPER_PARSER_H
#define CMDOPER_PARSER_H

#include "mysh.h"

cmdoper_p cmdoper_new (char *cmd, int oper, cmdoper_p prev, cmdoper_p next);
void cmdoper_free (cmdoper_p c);

void cmdoper_print_one (cmdoper_p c);
//void cmdoper_print_all (cmdoper_p c);
void cmdoper_print (cmdoper_p c);

cmdoper_p cmdoper_parse (char *str);

void cmdoper_parse_redir (cmdoper_p c);

#endif /* CMDOPER_PARSER_H */
