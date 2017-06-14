#ifndef MYSH_PROMPT_H_
#define MYSH_PROMPT_H_

#include "mysh.h"

/* TODO : ability to customize ? (arg cmdline or with rc file) */
#define MYSH_DEFAULT_PROMPT "Prompt#"

void mysh_prompt_set(mysh_context_p ctx, char *prompt);
void mysh_prompt_free(mysh_context_p ctx);
void mysh_prompt_print(mysh_context_p ctx);
int mysh_prompt_catch(mysh_context_p ctx);
void mysh_prompt_release(mysh_context_p ctx);

#endif /* MYSH_PROMPT_H_ */
