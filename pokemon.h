#ifndef POKEMON_H
#define POKEMON_H

#include <string>

struct Stats {
    int hp;
    int maxHp;
    int attack;
    int defense;
    int speed;
};

class Pokemon {
public:
    std::string name;
    std::string species;
    int level;
    Stats stats;
    int exp;
    int maxExp;
    
    Pokemon();
    Pokemon(const std::string& sp, int lvl);
    void initStats();
    bool isFainted() const;
};

#endif // POKEMON_H
