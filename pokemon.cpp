#include "pokemon.h"

Pokemon::Pokemon() : level(1), exp(0), maxExp(100) {}

Pokemon::Pokemon(const std::string& sp, int lvl) 
    : species(sp), level(lvl), exp(0), maxExp(100) {
    name = sp;
    initStats();
}

void Pokemon::initStats() {
    if (species == "小火龙") {
        stats = {20, 20, 12, 8, 10};
    } else if (species == "杰尼龟") {
        stats = {22, 22, 10, 10, 8};
    } else if (species == "妙蛙种子") {
        stats = {21, 21, 11, 9, 9};
    } else {
        stats = {15, 15, 10, 10, 10};
    }
    stats.maxHp += level * 2;
    stats.hp = stats.maxHp;
    stats.attack += level;
    stats.defense += level;
    stats.speed += level;
}

bool Pokemon::isFainted() const {
    return stats.hp <= 0;
}
