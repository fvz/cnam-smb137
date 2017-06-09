#ifndef MYSH_FACILITIES_H
#define MYSH_FACILITIES_H

#include "mysh.h"

#define BUFFER_SIZE 1024


void freeif(void* to_free);


void vmyprintf_stream(FILE * stream, int threshold, int level, const char *format, va_list args);
void myprintf_stream(FILE * stream, int threshold, int level, const char *format, ...);
#define myprintf(threshold, level, ...) \
    myprintf_stream(stdout, threshold, level, __VA_ARGS__)
#define myprintf_err(threshold, level, ...) \
    myprintf_stream(stderr, threshold, level, __VA_ARGS__)

void ctx_myprintf_stream(FILE * stream, int threshold, mysh_context_p ctx, const char *format, ...);
#define ctx_myprintf(threshold, ctx, ...) \
    ctx_myprintf_stream(stdout, threshold, ctx, __VA_ARGS__ )
#define ctx_myprintf_err(threshold, ctx, ...) \
    ctx_myprintf_stream(stderr, threshold, ctx, __VA_ARGS__ )

void ctx_dbmyprintf_stream(FILE * stream, int threshold, mysh_context_p ctx, const char *format, ...);
#define ctx_dbmyprintf(threshold, ctx, ...) \
    ctx_dbmyprintf_stream(stdout, threshold, ctx, __VA_ARGS__ )
#define ctx_dbmyprintf_err(threshold, ctx, ...) \
    ctx_dbmyprintf_stream(stderr, threshold, ctx, __VA_ARGS__ )


#endif /* MYSH_FACILITIES_H */
