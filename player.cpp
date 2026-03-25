#include "player.h"

Player::Player() {
    // 初始给予一些精灵球
    pokeballs[0] = 10;  // 精灵球
    pokeballs[1] = 3;   // 超级球
    pokeballs[2] = 1;   // 高级球
    pokeballs[3] = 0;   // 大师球
    initItems();
}

void Player::initItems() {
    // 初始给予一些药水
    items.push_back({"治疗药水", 5, 0, 33, 0, false});    // 恢复最大生命值的 33%
    items.push_back({"经验药水", 3, 0, 0, 50, false});    // 获得 50 经验
    items.push_back({"复活药剂", 2, 0, 0, 0, true});      // 复活阵亡的精灵
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

int Player::getHealPotionCount() const {
    for (const auto& item : items) {
        if (!item.isRevive && item.healPercent > 0) {
            return item.count;
        }
    }
    return 0;
}

bool Player::swapTeamPokemon(int index1, int index2) {
    if (index1 < 0 || index1 >= (int)team.size() || 
        index2 < 0 || index2 >= (int)team.size()) {
        return false;
    }
    Pokemon temp = team[index1];
    team[index1] = team[index2];
    team[index2] = temp;
    return true;
}

bool Player::movePokemonToStorage(int teamIndex) {
    if (teamIndex < 0 || teamIndex >= (int)team.size()) {
        return false;
    }
    if (team.size() <= 1) {
        return false;  // 至少保留一只精灵在队伍中
    }
    storage.push_back(team[teamIndex]);
    team.erase(team.begin() + teamIndex);
    return true;
}

bool Player::movePokemonFromStorage(int storageIndex) {
    if (storageIndex < 0 || storageIndex >= (int)storage.size()) {
        return false;
    }
    if (team.size() >= 6) {
        return false;  // 队伍已满
    }
    team.push_back(storage[storageIndex]);
    storage.erase(storage.begin() + storageIndex);
    return true;
}

bool Player::useItemOnPokemon(int itemIndex, int pokemonIndex, bool isTeam) {
    if (itemIndex < 0 || itemIndex >= (int)items.size()) {
        return false;
    }
    if (items[itemIndex].count <= 0) {
        return false;
    }
    
    Pokemon* targetPoke = nullptr;
    if (isTeam) {
        if (pokemonIndex < 0 || pokemonIndex >= (int)team.size()) {
            return false;
        }
        targetPoke = &team[pokemonIndex];
    } else {
        if (pokemonIndex < 0 || pokemonIndex >= (int)storage.size()) {
            return false;
        }
        targetPoke = &storage[pokemonIndex];
    }
    
    Item& item = items[itemIndex];
    
    // 复活药水不能使用此函数
    if (item.isRevive) {
        return false;
    }
    
    // 已阵亡的精灵无法使用治疗药水
    if (targetPoke->isFainted()) {
        return false;
    }
    
    // 使用治疗药水
    if (item.healPercent > 0) {
        int healAmount = targetPoke->stats.maxHp * item.healPercent / 100;
        targetPoke->stats.hp += healAmount;
        if (targetPoke->stats.hp > targetPoke->stats.maxHp) {
            targetPoke->stats.hp = targetPoke->stats.maxHp;
        }
    }
    
    // 使用经验药水
    if (item.expAmount > 0) {
        targetPoke->exp += item.expAmount;
        // 检查升级
        while (targetPoke->exp >= targetPoke->maxExp) {
            targetPoke->exp -= targetPoke->maxExp;
            targetPoke->level++;
            targetPoke->maxExp += 50;
            // 提升能力值
            targetPoke->stats.maxHp += 3;
            targetPoke->stats.hp += 3;
            targetPoke->stats.attack += 1;
            targetPoke->stats.defense += 1;
            targetPoke->stats.speed += 1;
        }
    }
    
    item.count--;
    return true;
}

bool Player::revivePokemon(int itemIndex, int pokemonIndex) {
    if (itemIndex < 0 || itemIndex >= (int)items.size()) {
        return false;
    }
    if (items[itemIndex].count <= 0) {
        return false;
    }
    if (!items[itemIndex].isRevive) {
        return false;  // 不是复活药水
    }
    if (pokemonIndex < 0 || pokemonIndex >= (int)team.size()) {
        return false;
    }
    
    Pokemon& targetPoke = team[pokemonIndex];
    if (!targetPoke.isFainted()) {
        return false;  // 只有阵亡的精灵才能复活
    }
    
    // 复活精灵，生命值为 1
    targetPoke.stats.hp = 1;
    items[itemIndex].count--;
    return true;
}
