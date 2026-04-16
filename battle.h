#ifndef BATTLE_H
#define BATTLE_H

#include "pokemon.h"
#include "player.h"
#include <string>

/*
 * 战斗模块（接口层）
 *
 * - BattleState：对外只读的战斗快照（用于 UI / Game 层展示）。
 * - Battle：封装战斗流程与结算逻辑（回合制：玩家回合 ↔ 敌方回合）。
 */

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
    // 说明：这些函数是战斗内部的“动作/规则实现”，不直接负责 UI 菜单展示。
    int calculateDamage(const Pokemon& attacker, const Pokemon& defender, const Move& move);
    void useMove(Pokemon& attacker, Pokemon& defender, int moveIndex, bool isPlayer);
    bool tryEscape();
    void catchPokemon(int ballIndex);
    bool switchPokemon();
    void healPokemon();

    // 状态检查
    // 根据双方 HP / 可用宝可梦等条件，决定战斗是否结束，并做必要收尾。
    void checkBattleEnd();
    // 玩家队伍中是否仍有可继续战斗的宝可梦。
    bool hasAlivePokemon() const;

public:
    // 绑定到一个 Player（读取其队伍、背包与精灵球数量）。
    Battle(Player& player);

    // 战斗控制
    // 开始一场与指定种类野生宝可梦的战斗（初始化对手、先手回合等）。
    void start(const std::string& wildPokemonSpecies);
    bool isInBattle() const;
    // 战斗主循环：直到胜利/失败/捕捉/逃跑等条件触发结束。
    void run();

    // 玩家回合
    // 可选的“外部驱动入口”（例如未来改成图形界面时，可由 UI 传入动作）。
    bool playerAction(const std::string& action, const std::string& param = "");
    
    // 获取战斗状态
    const BattleState& getState() const;
    // 敌方回合逻辑（当前实现：随机选择一个技能）。
    void enemyTurn();
};

#endif // BATTLE_H
