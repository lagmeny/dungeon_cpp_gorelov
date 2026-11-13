#include "Renderer.hpp"
#include "Game.hpp"
#include <ostream>

void Renderer::draw(const Game& game,std::ostream& out) const {
    for(std::size_t r=0;r<game.map().height();++r) {
        for(std::size_t c=0;c<game.map().width();++c) {
            char ch=game.map().get(r,c);
            for(const auto& m:game.monsters()) if (m->alive() && m->row()==static_cast<int>(r) && m->col()==static_cast<int>(c)) { ch=m->symbol(); break; }
            if (game.hero().alive() && game.hero().row()==static_cast<int>(r) && game.hero().col()==static_cast<int>(c)) ch='@';
            out<<ch;
        }
        out<<'\n';
    }
}
