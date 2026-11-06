#include "Hero.hpp"
#include "Map.hpp"
#include <iostream>

// Default constructor initializes a hero at the standard start position.
Hero::Hero() : Hero(1, 1, 100, 0) {}

// Parameterized constructor initializes all hero attributes.
Hero::Hero(int row, int col, int health, int gold)
    : row_(row), col_(col), health_(health), gold_(gold) {}

// Method receives its own hero through implicit this and the map as a reference.
bool Hero::move(int dr, int dc, Map& map) {
    const int nextRow = this->row_ + dr;
    const int nextCol = this->col_ + dc;
    if (!map.passable(nextRow, nextCol)) return false;
    this->row_ = nextRow;
    this->col_ = nextCol;
    char cell = map.get(static_cast<std::size_t>(nextRow), static_cast<std::size_t>(nextCol));
    if (cell == '$') {
        ++this->gold_;
        map.set(static_cast<std::size_t>(nextRow), static_cast<std::size_t>(nextCol), '.');
        std::cout << "Gold collected!\n";
    }
    if (cell == 'E') std::cout << "Exit reached!\n";
    return true;
}

// Reduces health without allowing it to become negative.
void Hero::takeDamage(int amount) {
    if (amount > 0) health_ = amount >= health_ ? 0 : health_ - amount;
}

// Displays the current hero state.
void Hero::status() const {
    std::cout << "Hero: (" << row_ << ", " << col_ << ") HP: " << health_ << " Gold: " << gold_ << '\n';
}

// Returns the current row.
int Hero::row() const { return row_; }

// Returns the current column.
int Hero::col() const { return col_; }
