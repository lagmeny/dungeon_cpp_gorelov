#pragma once
class Game;

class Creature {
    int row_, col_, health_, maxHealth_;
    char symbol_;
protected:
    Creature(int row, int col, int hp, char symbol);
    bool step(Game& game, int dr, int dc);
public:
    virtual ~Creature() = default;
    Creature(const Creature&) = delete;
    Creature& operator=(const Creature&) = delete;
    int row() const noexcept { return row_; }
    int col() const noexcept { return col_; }
    int health() const noexcept { return health_; }
    int maxHealth() const noexcept { return maxHealth_; }
    bool alive() const noexcept { return health_ > 0; }
    char symbol() const noexcept { return symbol_; }
    void takeDamage(int damage) noexcept;
    virtual void turn(Game& game) = 0;
};

