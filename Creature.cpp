#include "Creature.hpp"
#include "Game.hpp"
#include <algorithm>
#include <stdexcept>

Creature::Creature(int r,int c,int hp,char symbol):row_(r),col_(c),health_(hp),maxHealth_(hp),symbol_(symbol) {
    if (r<0 || c<0 || hp<=0) throw std::invalid_argument("Invalid creature state");
}
void Creature::takeDamage(int damage) noexcept {
    if (damage>0) health_=std::max(0,health_-std::min(damage,health_));
}
bool Creature::step(Game& game,int dr,int dc) {
    const int nr=row_+dr, nc=col_+dc;
    if (!game.canEnter(nr,nc,this)) return false;
    row_=nr; col_=nc;
    return true;
}
