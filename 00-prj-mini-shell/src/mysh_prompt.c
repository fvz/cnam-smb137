/**
 * \file mysh_prompt.c
 * \brief Gestion du prompt (affichage, récupération des commandes saisies)
 * \author Fabien VANTARD
 * \version 0.1
 */

#include "mysh_prompt.h"

/**
 * \fn void mysh_prompt_set_with_new(mysh_context_p ctx, char *prompt)
 * \brief Redéfinition du prompt à afficher (il est stocké dans le contexte)
 *
 * \param ctx Pointeur sur l'objet mysh_context
 * \param prompt Nouveau prompt à afficher
 *
 * Cette fonction est identique à mysh_prompt_set() à ceci près qu'elle
 * attend un pointeur sur une chaine nouvellement allouée. Elle assigne
 * le prompt à ce pointeur. Après l'appel de cette fonction, il ne faudra
 * donc en aucun cas libérer cette nouvelle chaine.
 */
void mysh_prompt_set_with_new(mysh_context_p ctx, char *prompt) {
	if (ctx != NULL) {
		freeif(ctx->prompt);
		ctx->prompt = prompt;
	}
}

/**
 * \fn void mysh_prompt_set(mysh_context_p ctx, char *prompt)
 * \brief Redéfinition du prompt à afficher (il est stocké dans le contexte)
 *
 * \param ctx Pointeur sur l'objet mysh_context
 * \param prompt Nouveau prompt à afficher
 */
void mysh_prompt_set(mysh_context_p ctx, char *prompt) {
	if (ctx != NULL) {
		mysh_prompt_set_with_new(ctx, prompt ? strndup(prompt, strlen(prompt)) : NULL);
	}
}

/**
 * \fn void mysh_prompt_free(mysh_context_p ctx)
 * \brief Libération du prompt
 *
 * \param ctx Pointeur sur l'objet mysh_context
 */
void mysh_prompt_free(mysh_context_p ctx) {
	if (ctx != NULL) {
		freeif(ctx->prompt);
		ctx->prompt = NULL;
	}
}

/**
 * \fn void mysh_prompt_print(mysh_context_p ctx)
 * \brief Affichage de l'invite prompt.
 *
 * \param ctx Pointeur sur l'objet mysh_context
 */
void mysh_prompt_print(mysh_context_p ctx) {
	printf("%s ", ctx->prompt);
}

/**
 * \fn int mysh_prompt_catch(mysh_context_p ctx)
 * \brief Essaye de récupérer une ligne de commande passée dans mysh.
 *
 * \param ctx Pointeur sur l'objet mysh_context
 * \return bool true/false suivant si une ligne de cmd a été récupérée.
 */
int mysh_prompt_catch(mysh_context_p ctx) {
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);

	if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
		return false;
	}

	char *pos = NULL;
	if ((pos = strchr(buffer, '\n'))) { *pos = '\0'; }

	freeif(ctx->cmd);
	ctx->cmd = strndup(buffer, strlen(buffer));
	return true;
}

/**
 * \fn void mysh_prompt_release(mysh_context_p ctx)
 * \brief Libère le prompt (pour l'instant, ça free la cmd seulement.)
 *
 * \param h Pointeur sur l'objet mysh_context
 */
void mysh_prompt_release(mysh_context_p ctx) {
	freeif(ctx->cmd);
	ctx->cmd = NULL;
}
