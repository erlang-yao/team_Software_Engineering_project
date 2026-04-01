#ifndef BATTLE_H
#define BATTLE_H

#include "pokemon.h"
#include "player.h"
#include <string>

// 战斗状态
struct BattleState {
    bool inBattle;
    Pokemon wildPokemon;      // 野生宝可梦
    int playerPokeIndex;      // 玩家当前出战的宝可梦索引
    bool playerTurn;          // 是否玩家回合
};

class Battle {
private:
    BattleState state;
    Player& player;

    // 战斗动作
    int calculateDamage(const Pokemon& attacker, const Pokemon& defender, const Move& move);
    void useMove(Pokemon& attacker, Pokemon& defender, int moveIndex, bool isPlayer);
    bool tryEscape();
    void catchPokemon(int ballIndex);
    bool switchPokemon();
    void healPokemon();

    // 状态检查
    void checkBattleEnd();
    bool hasAlivePokemon() const;

public:
    Battle(Player& player);

    // 战斗控制
    void start(const std::string& wildPokemonSpecies);
    bool isInBattle() const;
    void run();

    // 玩家回合
    bool playerAction(const std::string& action, const std::string& param = "");
    
    // 获取战斗状态
    const BattleState& getState() const;
    void enemyTurn();
};

#endif // BATTLE_H
