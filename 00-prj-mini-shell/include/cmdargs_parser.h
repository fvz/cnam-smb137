/**
 * \file cmdargs_parser.h
 * \brief Parser d'arguments d'une commande
 * \author Fabien VANTARD
 * \version 0.1
 */

#ifndef CMDARGS_PARSER_H_
#define CMDARGS_PARSER_H_

#include "mysh.h"

void cmdargs_free (char ***array);
int cmdargs_append (char ***array, char *arg);
int cmdargs_append_with_new (char ***array, char *arg);
int cmdargs_parser (mysh_context_p ctx, char *str, char ***arr);

#endif /* CMDARGS_PARSER_H_ */
