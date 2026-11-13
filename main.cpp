#include "Game.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

int main(int argc,char** argv) {
    try {
        int row=1,col=1;
        auto map=(argc>1) ? Map::load(argv[1],row,col) : std::make_unique<Map>(12,24);
        Game game(std::move(map),row,col);
        int added=0;
        for (int r=static_cast<int>(game.map().height())-2;r>=1 && added<3;--r)
            for(int c=static_cast<int>(game.map().width())-2;c>=1 && added<3;--c)
                if (game.canEnter(r,c)) {
                    if (added==0) game.addMonster(std::make_unique<Chaser>(r,c));
                    if (added==1) game.addMonster(std::make_unique<Wanderer>(r,c));
                    if (added==2) game.addMonster(std::make_unique<Guard>(r,c));
                    ++added;
                }
        game.run(std::cin,std::cout);
    } catch(const std::exception& e) { std::cerr<<"Error: "<<e.what()<<'\n'; return 1; }
}
