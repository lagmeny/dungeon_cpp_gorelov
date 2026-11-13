#include "Game.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

Game::Game(std::unique_ptr<Map> map,int r,int c):map_(std::move(map)),hero_(r,c) {
    if (!map_ || !map_->passable(r,c)) throw std::invalid_argument("Invalid hero spawn");
}
bool Game::occupied(int r,int c,const Creature* except) const noexcept {
    for(const auto& m:monsters_) if (m.get()!=except && m->alive() && m->row()==r && m->col()==c) return true;
    return false;
}
bool Game::canEnter(int r,int c,const Creature* except) const noexcept {
    return map_->passable(r,c) && !occupied(r,c,except) && (except==&hero_ || r!=hero_.row() || c!=hero_.col());
}
void Game::addMonster(std::unique_ptr<Creature> monster) {
    if (!monster || !monster->alive() || !map_->passable(monster->row(),monster->col()) || occupied(monster->row(),monster->col()) || (monster->row()==hero_.row() && monster->col()==hero_.col()))
        throw std::invalid_argument("Invalid monster spawn");
    monsters_.push_back(std::move(monster));
}
void Game::attackHero(int damage) noexcept { hero_.takeDamage(damage); if (!hero_.alive()) finished_=true; }
void Game::run(std::istream& in,std::ostream& out) {
    out<<"Commands: w/a/s/d, status, look, quit\n";
    renderer_.draw(*this,out);
    std::string cmd;
    while (!finished_ && hero_.alive() && std::getline(in,cmd)) {
        if (cmd=="q" || cmd=="quit") break;
        if (cmd=="status") { out<<"HP "<<hero_.health()<<"/"<<hero_.maxHealth()<<" Gold "<<hero_.gold()<<" Turns "<<turns_<<" Monsters created "<<Monster::createdCount()<<'\n'; continue; }
        if (cmd=="look") { renderer_.draw(*this,out); continue; }
        int dr=0,dc=0;
        if (cmd=="w" || cmd=="n") dr=-1;
        else if (cmd=="s") dr=1;
        else if (cmd=="a") dc=-1;
        else if (cmd=="d" || cmd=="e") dc=1;
        else { out<<"Unknown command\n"; continue; }
        if (!hero_.alive()) break;
        const int nr=hero_.row()+dr,nc=hero_.col()+dc;
        if (occupied(nr,nc)) { out<<"A monster blocks the way\n"; continue; }
        if (!map_->passable(nr,nc)) { out<<"Wall\n"; continue; }
        // Hero is a Creature, but its movement is controlled by a command.
        // Move via a narrow Game API: coordinates remain private to Creature.
        if (!moveHero(dr,dc)) continue;
        ++turns_;
        if (map_->get(static_cast<std::size_t>(hero_.row()),static_cast<std::size_t>(hero_.col()))=='$') {
            hero_.collectGold(); map_->set(static_cast<std::size_t>(hero_.row()),static_cast<std::size_t>(hero_.col()),'.'); out<<"Gold collected\n";
        }
        if (map_->get(static_cast<std::size_t>(hero_.row()),static_cast<std::size_t>(hero_.col()))=='E') { out<<"Exit reached!\n"; finished_=true; }
        if (!finished_) for(auto& m:monsters_) m->turn(*this); // virtual dispatch; no type checks
        renderer_.draw(*this,out);
    }
    if (!hero_.alive()) out<<"Game over\n";
}
