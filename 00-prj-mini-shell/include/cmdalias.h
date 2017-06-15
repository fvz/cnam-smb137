#ifndef CMDALIAS_H_
#define CMDALIAS_H_

#include "mysh.h"

cmdalias_p cmdalias_new(char *name, char *cmd);
void cmdalias_free(cmdalias_p alias);
void cmdalias_free_array_elt(cmdalias_p *alias);
void cmdalias_free_array(cmdalias_p **alias);
cmdalias_p cmdalias_search(mysh_context_p ctx, char *name);
void cmdalias_set(cmdalias_p alias, char *name, char *cmd);
void cmdalias_set_with_new(cmdalias_p alias, char *name, char *cmd);
void cmdalias_redefine_cmd(cmdalias_p alias, char *cmd);
void cmdalias_redefine_cmd_with_new(cmdalias_p alias, char *cmd);
void cmdalias_add(mysh_context_p ctx, char *name, char *cmd);
int cmdalias_parse_cmd(mysh_context_p ctx, char *cmd, char **newalias, char **newcmd);
void cmdalias_print_one(cmdalias_p alias);
void cmdalias_print(mysh_context_p ctx);

#endif /* CMDALIAS_H_ */
