#ifndef CMDARGS_PARSER_H
#define CMDARGS_PARSER_H

#include "mysh.h"

void cmdargs_free (char ***array);
int cmdargs_parser (mysh_context_p ctx, char *str, char ***arr);

#endif /* CMDARGS_PARSER_H */
