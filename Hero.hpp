#pragma once
#include "Creature.hpp"

class Hero final : public Creature {
    int gold_ = 0;
public:
    Hero(int row, int col, int hp = 100);
    int gold() const noexcept { return gold_; }
    void turn(Game& game) override;
    bool move(Game& game, int dr, int dc);
    void collectGold() noexcept { ++gold_; }
};

