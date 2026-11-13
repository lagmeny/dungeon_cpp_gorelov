#include "Map.hpp"
#include <fstream>
#include <stdexcept>
#include <vector>

Map::Map(std::size_t h, std::size_t w) : height_(h), width_(w), cells_(nullptr) {
    if (h < 3 || w < 3 || h > 200 || w > 200) throw std::invalid_argument("Map size must be 3..200");
    cells_ = std::make_unique<char[]>(h * w);
    for (std::size_t r=0;r<h;++r) for(std::size_t c=0;c<w;++c)
        cells_[r*w+c] = (r==0 || c==0 || r+1==h || c+1==w) ? '#' : '.';
}
char Map::get(std::size_t r, std::size_t c) const {
    if (r>=height_ || c>=width_) throw std::out_of_range("Map::get");
    return cells_[r*width_+c];
}
bool Map::passable(int r, int c) const noexcept {
    return r>=0 && c>=0 && static_cast<std::size_t>(r)<height_ && static_cast<std::size_t>(c)<width_ && cells_[static_cast<std::size_t>(r)*width_+static_cast<std::size_t>(c)]!='#';
}
void Map::set(std::size_t r, std::size_t c, char value) {
    if (r>=height_ || c>=width_) throw std::out_of_range("Map::set");
    if (value!='#' && value!='.' && value!='$' && value!='E') throw std::invalid_argument("Invalid cell");
    if ((r==0 || c==0 || r+1==height_ || c+1==width_) && value!='#') throw std::invalid_argument("Border must be walls");
    cells_[r*width_+c]=value;
}
std::unique_ptr<Map> Map::load(const std::string& path, int& startRow, int& startCol) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Cannot open level: "+path);
    std::vector<std::string> lines;
    std::string line;
    int starts=0, exits=0, row=-1, col=-1;
    while (std::getline(file,line)) {
        if (!line.empty() && line.back()=='\r') line.pop_back();
        if (line.empty() || line.size()>200 || (!lines.empty() && line.size()!=lines.front().size()) || lines.size()>=200)
            throw std::runtime_error("Invalid level dimensions");
        for (std::size_t c=0;c<line.size();++c) {
            char ch=line[c];
            if (ch!='#' && ch!='.' && ch!='$' && ch!='E' && ch!='@') throw std::runtime_error("Invalid level character");
            if (ch=='@') { ++starts; row=static_cast<int>(lines.size()); col=static_cast<int>(c); }
            if (ch=='E') ++exits;
        }
        lines.push_back(line);
    }
    if (file.bad()) throw std::runtime_error("Level read error");
    if (lines.size()<3 || lines.front().size()<3 || starts!=1 || exits!=1) throw std::runtime_error("Invalid level markers");
    for(std::size_t r=0;r<lines.size();++r) for(std::size_t c=0;c<lines[r].size();++c)
        if ((r==0 || c==0 || r+1==lines.size() || c+1==lines[r].size()) && lines[r][c]!='#') throw std::runtime_error("Border must be walls");
    auto result=std::make_unique<Map>(lines.size(),lines.front().size());
    for(std::size_t r=0;r<lines.size();++r) for(std::size_t c=0;c<lines[r].size();++c)
        result->set(r,c,lines[r][c]=='@'?'.':lines[r][c]);
    startRow=row; startCol=col;
    return result;
}
