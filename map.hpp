#pragma once
#include <cstddef>
#include <memory>
#include <string>

class Map {
    std::size_t height_, width_;
    std::unique_ptr<char[]> cells_;
public:
    Map(std::size_t height, std::size_t width);
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;
    std::size_t height() const noexcept { return height_; }
    std::size_t width() const noexcept { return width_; }
    char get(std::size_t row, std::size_t col) const;
    bool passable(int row, int col) const noexcept;
    void set(std::size_t row, std::size_t col, char value);
    static std::unique_ptr<Map> load(const std::string& path, int& row, int& col);
};

