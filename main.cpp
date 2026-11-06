#include "Hero.hpp"
#include "Map.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// Demonstrates a stack object's destruction at block exit and a heap object's explicit delete.
static void demonstrateLifetime() {
    std::cout << "[Demo: enter block]\n";
    {
        Map local(3, 3);
        std::cout << "[Demo: leaving block]\n";
    }
    Map* heap = new Map(3, 3);
    std::cout << "[Demo: explicit delete]\n";
    delete heap;
}

// Reads commands and delegates behavior to the Map and Hero objects.
static void play(Map& map, Hero& hero) {
    std::cout << "Welcome to the dungeon!\n";
    map.draw(hero.row(), hero.col());
    std::string line;
    while (true) {
        std::cout << "Enter command > " << std::flush;
        if (!std::getline(std::cin, line)) break;
        int dr = 0, dc = 0;
        if (line == "quit" || line == "q") break;
        if (line == "north" || line == "n" || line == "go north") dr = -1;
        else if (line == "south" || line == "s" || line == "go south") dr = 1;
        else if (line == "west" || line == "w" || line == "go west") dc = -1;
        else if (line == "east" || line == "e" || line == "go east") dc = 1;
        else if (line == "status") { hero.status(); continue; }
        else if (line == "look") { map.draw(hero.row(), hero.col()); continue; }
        else if (line == "help") { std::cout << "n/s/e/w, north/south/east/west, go north, look, status, quit\n"; continue; }
        else { std::cout << "Unknown command\n"; continue; }
        std::cout << (hero.move(dr, dc, map) ? "Moved\n" : "Blocked\n");
        map.draw(hero.row(), hero.col());
    }
}

// Selects a generated map, a file level or the lifetime demonstration.
int main(int argc, char** argv) {
    try {
        if (argc == 2 && std::string(argv[1]) == "--demo-lifetime") {
            demonstrateLifetime();
            return 0;
        }
        std::unique_ptr<Map> map;
        int startRow = 1, startCol = 1;
        if (argc == 3 && std::string(argv[1]) == "--level") {
            map.reset(Map::load(argv[2], startRow, startCol));
        } else if (argc == 3) {
            std::size_t used1 = 0, used2 = 0;
            std::string h = argv[1], w = argv[2];
            std::size_t height = std::stoul(h, &used1), width = std::stoul(w, &used2);
            if (used1 != h.size() || used2 != w.size()) throw std::invalid_argument("invalid dimensions");
            map.reset(new Map(height, width));
        } else if (argc == 1) {
            map.reset(new Map(8, 12));
        } else {
            throw std::invalid_argument("usage: dungeon [height width] | --level file | --demo-lifetime");
        }
        Hero hero(startRow, startCol, 100, 0);
        if (argc != 3 || std::string(argv[1]) != "--level") {
            map->set(1, 2, '$');
            map->set(map->height() - 2, map->width() - 2, 'E');
        }
        play(*map, hero);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
