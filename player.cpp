#include "player.h"

Player::Player() : pokeballs(5) {}

void Player::addPokemon(const Pokemon& p) {
    if (team.size() < 6) {
        team.push_back(p);
    } else {
        storage.push_back(p);
    }
}

Pokemon* Player::getCurrentPokemon() {
    for (auto& p : team) {
        if (!p.isFainted()) {
            return &p;
        }
    }
    return nullptr;
}

bool Player::hasAlivePokemon() const {
    for (const auto& p : team) {
        if (!p.isFainted()) {
            return true;
        }
    }
    return false;
}
