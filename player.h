#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include "pokemon.h"

class Player {
public:
    std::string name;
    std::vector<Pokemon> team;
    std::vector<Pokemon> storage;
    int pokeballs[4];  // 0:PokeBall, 1:GreatBall, 2:UltraBall, 3:MasterBall

    Player();
    void addPokemon(const Pokemon& p);
    Pokemon* getCurrentPokemon();
    bool hasAlivePokemon() const;
    int getTotalBalls() const;  // 获取所有精灵球总数
};

#endif // PLAYER_H
