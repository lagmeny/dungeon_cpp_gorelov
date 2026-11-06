#ifndef DUNGEON_HERO_HPP
#define DUNGEON_HERO_HPP
class Map;
class Hero {
public:
    Hero();
    Hero(int row, int col, int health, int gold);
    bool move(int dr, int dc, Map& map);
    void takeDamage(int amount);
    void status() const;
    int row() const;
    int col() const;
private:
    int row_;
    int col_;
    int health_;
    int gold_;
};
#endif
