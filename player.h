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
    int pokeballs;
    
    Player();
    void addPokemon(const Pokemon& p);
    Pokemon* getCurrentPokemon();
    bool hasAlivePokemon() const;
};

#endif // PLAYER_H
