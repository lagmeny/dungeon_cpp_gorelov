#include "Monster.hpp"
#include "Game.hpp"
#include <array>
#include <cstdlib>
#include <random>
#include <utility>

int Monster::created_=0;
Monster::Monster(int r,int c,int hp,char symbol):Creature(r,c,hp,symbol) { ++created_; }
Chaser::Chaser(int r,int c):Monster(r,c,30,'C'){}
Wanderer::Wanderer(int r,int c):Monster(r,c,20,'W'){}
Guard::Guard(int r,int c):Monster(r,c,40,'G'){}
void Chaser::turn(Game& game) {
    if (!alive()) return;
    int dr=(game.hero().row()>row())-(game.hero().row()<row());
    int dc=(game.hero().col()>col())-(game.hero().col()<col());
    if (std::abs(game.hero().row()-row())+std::abs(game.hero().col()-col())==1) { game.attackHero(7); return; }
    if (dr && step(game,dr,0)) return;
    if (dc) step(game,0,dc);
}
void Wanderer::turn(Game& game) {
    if (!alive()) return;
    static std::mt19937 rng(42);
    static const std::array<std::pair<int,int>,4> moves={{{1,0},{-1,0},{0,1},{0,-1}}};
    auto move=moves[std::uniform_int_distribution<int>(0,3)(rng)];
    int nr=row()+move.first,nc=col()+move.second;
    if (nr==game.hero().row() && nc==game.hero().col()) game.attackHero(3);
    else step(game,move.first,move.second);
}
void Guard::turn(Game& game) {
    if (alive() && std::abs(game.hero().row()-row())+std::abs(game.hero().col()-col())==1) game.attackHero(10);
}
