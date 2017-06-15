#ifndef MYSH_HISTORY_H_
#define MYSH_HISTORY_H_

#include "mysh.h"

#define MYSH_HISTORY_DEFAULT_FILENAME ".mysh_history"

void mysh_history_open_temp_file(mysh_history_p h, mysh_context_p ctx);
void mysh_history_close_temp_file(mysh_context_p ctx);
void mysh_history_init(mysh_history_p h, mysh_context_p ctx);
mysh_history_p mysh_history_new(mysh_context_p ctx);
void mysh_history_add(mysh_context_p ctx, char *cmd);
void mysh_history_flush(mysh_context_p ctx);
void mysh_history_free(mysh_context_p ctx);
void mysh_history_print(mysh_context_p ctx);

#endif /* MYSH_HISTORY_H_ */
