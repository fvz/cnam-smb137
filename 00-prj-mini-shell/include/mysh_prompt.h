#ifndef MYSH_PROMPT_H_
#define MYSH_PROMPT_H_

#include "mysh.h"

void mysh_prompt_print(mysh_context_p ctx);
int mysh_prompt_catch(mysh_context_p ctx);
void mysh_prompt_release(mysh_context_p ctx);

#endif /* MYSH_PROMPT_H_ */
