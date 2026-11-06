#include "Map.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Constructor allocates and initializes the map; ownership belongs to this object.
Map::Map(std::size_t height, std::size_t width) : height_(height), width_(width), cells_(nullptr) {
    if (height < 3 || width < 3 || height > 200 || width > 200)
        throw std::invalid_argument("invalid map size (3..200)");
    cells_ = new char[height_ * width_];
    for (std::size_t r = 0; r < height_; ++r)
        for (std::size_t c = 0; c < width_; ++c)
            cells_[r * width_ + c] = (r == 0 || c == 0 || r + 1 == height_ || c + 1 == width_) ? '#' : '.';
    std::cout << "[Map constructor] " << height_ << "x" << width_ << "\n";
}

// Destructor releases exactly the block acquired by the constructor (RAII).
Map::~Map() {
    delete[] cells_;
    std::cout << "[Map destructor]\n";
}

// Returns a cell character after bounds checking.
char Map::get(std::size_t row, std::size_t col) const {
    if (row >= height_ || col >= width_) throw std::out_of_range("cell outside map");
    return cells_[row * width_ + col];
}

// Changes a cell after bounds checking.
void Map::set(std::size_t row, std::size_t col, char value) {
    if (row >= height_ || col >= width_) throw std::out_of_range("cell outside map");
    cells_[row * width_ + col] = value;
}

// Checks whether the requested position is inside the map and is not a wall.
bool Map::passable(int row, int col) const {
    return row >= 0 && col >= 0 && static_cast<std::size_t>(row) < height_ &&
           static_cast<std::size_t>(col) < width_ &&
           get(static_cast<std::size_t>(row), static_cast<std::size_t>(col)) != '#';
}

// Prints the map, drawing the hero over the underlying cell.
void Map::draw(int heroRow, int heroCol) const {
    for (std::size_t r = 0; r < height_; ++r) {
        for (std::size_t c = 0; c < width_; ++c)
            std::cout << ((static_cast<int>(r) == heroRow && static_cast<int>(c) == heroCol) ? '@' : get(r, c));
        std::cout << '\n';
    }
}

// Returns the number of map rows.
std::size_t Map::height() const { return height_; }

// Returns the number of map columns.
std::size_t Map::width() const { return width_; }

// Validates the entire file before creating the map, so errors never leak a map object.
Map* Map::load(const std::string& filename, int& startRow, int& startCol) {
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("level file not found: " + filename);
    std::vector<std::string> lines;
    std::string line;
    std::size_t starts = 0, exits = 0;
    int foundRow = -1, foundCol = -1;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line.size() > 200 || (!lines.empty() && line.size() != lines[0].size()))
            throw std::runtime_error("damaged level format: inconsistent row size");
        for (std::size_t c = 0; c < line.size(); ++c) {
            char ch = line[c];
            if (ch != '#' && ch != '.' && ch != '$' && ch != 'E' && ch != '@')
                throw std::runtime_error("damaged level format: unknown symbol");
            if (ch == '@') { ++starts; foundRow = static_cast<int>(lines.size()); foundCol = static_cast<int>(c); }
            if (ch == 'E') ++exits;
        }
        lines.push_back(line);
        if (lines.size() > 200) throw std::runtime_error("damaged level format: too many rows");
    }
    if (file.bad()) throw std::runtime_error("error reading level file");
    if (lines.size() < 3 || lines[0].size() < 3 || starts != 1 || exits != 1)
        throw std::runtime_error("damaged level format: size, start or exit");
    for (std::size_t r = 0; r < lines.size(); ++r)
        for (std::size_t c = 0; c < lines[r].size(); ++c)
            if ((r == 0 || c == 0 || r + 1 == lines.size() || c + 1 == lines[r].size()) && lines[r][c] != '#')
                throw std::runtime_error("damaged level format: border must be walls");
    // Construct only after validation; the caller takes ownership of the returned pointer.
    Map* result = new Map(lines.size(), lines[0].size());
    try {
        for (std::size_t r = 0; r < lines.size(); ++r)
            for (std::size_t c = 0; c < lines[r].size(); ++c)
                result->set(r, c, lines[r][c] == '@' ? '.' : lines[r][c]);
    } catch (...) { delete result; throw; }
    startRow = foundRow;
    startCol = foundCol;
    return result;
}
