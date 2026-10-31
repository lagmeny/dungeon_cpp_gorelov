#ifndef MAP_H
#define MAP_H
#include <stdbool.h>
#include <stddef.h>
enum GameError { GAME_OK, GAME_FILE_NOT_FOUND, GAME_BAD_FORMAT, GAME_NO_MEMORY, GAME_BAD_SIZE };
struct Map { size_t height, width; char *cells; };
GameError map_create(Map *map,size_t height,size_t width);
GameError map_load(Map *out,const char *filename);
void map_free(Map *map);
char *map_cell(Map *map,size_t row,size_t col);
char map_get(const Map *map,size_t row,size_t col);
bool map_info(const Map *map,int row,int col,char *type,bool *passable);
bool map_reachable(const Map *map,int row,int col);
void map_draw(const Map *map,int hero_row,int hero_col);
const char *game_error_string(GameError error);
#endif
