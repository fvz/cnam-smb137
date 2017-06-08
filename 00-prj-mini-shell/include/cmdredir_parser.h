#ifndef CMDREDIR_PARSER_H
#define CMDREDIR_PARSER_H

#include "mysh.h"

cmdredir_p cmdredir_new (char *cmd, int redic, cmdredir_p prev, cmdredir_p next);
void cmdredir_free (cmdredir_p c);

void cmdredir_print (cmdredir_p c);

cmdredir_p cmdredir_parse (char *str);

#endif /* CMDREDIR_PARSER_H */
