#ifndef GAME_H
#define GAME_H
#include "entities.hpp"
#include "map.hpp"
#include <stdbool.h>
struct Game { Map map; Hero hero; Monsters monsters; unsigned turns; bool running; };
GameError game_init(Game *game,size_t height,size_t width,const char *level);
void game_free(Game *game);
bool game_move(Game *game,int dr,int dc);
void game_command(Game *game,char *line);
int game_run(int argc,char **argv);
#endif
