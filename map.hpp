#ifndef DUNGEON_MAP_HPP
#define DUNGEON_MAP_HPP
#include <cstddef>
#include <string>

class Map {
public:
    Map(std::size_t height, std::size_t width);
    ~Map();
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;
    char get(std::size_t row, std::size_t col) const;
    void set(std::size_t row, std::size_t col, char value);
    bool passable(int row, int col) const;
    void draw(int heroRow, int heroCol) const;
    std::size_t height() const;
    std::size_t width() const;
    static Map* load(const std::string& filename, int& startRow, int& startCol);
private:
    std::size_t height_;
    std::size_t width_;
    char* cells_;
};
#endif
