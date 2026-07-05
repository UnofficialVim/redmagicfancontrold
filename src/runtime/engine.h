#ifndef ENGINE_H
#define ENGINE_H

struct Runtime;

int engine_init(struct Runtime *rt);
void engine_run(struct Runtime *rt);
void engine_shutdown(struct Runtime *rt);

#endif