#ifndef ENGINE_H
#define ENGINE_H

struct Runtime;

void engine_run(struct Runtime *rt);
void engine_shutdown(struct Runtime *rt);

#endif