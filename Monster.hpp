#pragma once
#include "Creature.hpp"

class Monster : public Creature {
    static int created_;
protected:
    Monster(int row, int col, int hp, char symbol);
public:
    static int createdCount() noexcept { return created_; }
};
class Chaser final : public Monster {
public:
    Chaser(int row, int col);
    void turn(Game& game) override;
};
class Wanderer final : public Monster {
public:
    Wanderer(int row, int col);
    void turn(Game& game) override;
};
class Guard final : public Monster {
public:
    Guard(int row, int col);
    void turn(Game& game) override;
};

