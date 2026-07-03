#ifdef ENGINE_H
#define ENGINE_H

#include <stddef.h>
typedef struct Runtime Runtime;

void engine_init(Runtime *rt);
void engine_run(Runtime *rt);
void engine_shutdown(Runtime *rt);

#endif