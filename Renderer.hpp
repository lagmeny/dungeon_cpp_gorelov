#pragma once
#include <iosfwd>
class Game;

class Renderer {
public:
    void draw(const Game& game, std::ostream& out) const;
};

