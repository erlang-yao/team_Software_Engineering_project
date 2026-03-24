#include "player.h"

Player::Player() {
    // 初始给予一些精灵球
    pokeballs[0] = 10;  // 精灵球
    pokeballs[1] = 3;   // 超级球
    pokeballs[2] = 1;   // 高级球
    pokeballs[3] = 0;   // 大师球
}

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

int Player::getTotalBalls() const {
    return pokeballs[0] + pokeballs[1] + pokeballs[2] + pokeballs[3];
}
