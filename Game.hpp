#pragma once
#include "Map.hpp"
#include "Hero.hpp"
#include "Monster.hpp"
#include "Renderer.hpp"
#include <iosfwd>
#include <memory>
#include <vector>

class Game {
    std::unique_ptr<Map> map_;
    Hero hero_;
    std::vector<std::unique_ptr<Creature>> monsters_;
    Renderer renderer_;
    bool finished_ = false;
    unsigned turns_ = 0;
public:
    Game(std::unique_ptr<Map> map, int row, int col);
    const Map& map() const noexcept { return *map_; }
    Map& map() noexcept { return *map_; }
    Hero& hero() noexcept { return hero_; }
    const Hero& hero() const noexcept { return hero_; }
    const std::vector<std::unique_ptr<Creature>>& monsters() const noexcept { return monsters_; }
    bool occupied(int row, int col, const Creature* except = nullptr) const noexcept;
    bool canEnter(int row, int col, const Creature* except = nullptr) const noexcept;
    void addMonster(std::unique_ptr<Creature> monster);
    void attackHero(int damage) noexcept;
    bool moveHero(int dr, int dc) { return hero_.move(*this, dr, dc); }
    void finish() noexcept { finished_ = true; }
    void run(std::istream& in, std::ostream& out);
};

