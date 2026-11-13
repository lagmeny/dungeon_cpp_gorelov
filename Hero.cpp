#include "Hero.hpp"
#include "Game.hpp"

Hero::Hero(int r,int c,int hp):Creature(r,c,hp,'@'){}
void Hero::turn(Game&) {}
bool Hero::move(Game& game,int dr,int dc) { return step(game,dr,dc); }
