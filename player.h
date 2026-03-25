#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include "pokemon.h"

// 药水类型
struct Item {
    std::string name;
    int count;
    int healAmount;      // 治疗药水的恢复量（固定值）
    int healPercent;     // 治疗药水的恢复百分比
    int expAmount;       // 经验药水的经验值
    bool isRevive;       // 是否为复活药水
};

class Player {
public:
    std::string name;
    std::vector<Pokemon> team;
    std::vector<Pokemon> storage;
    int pokeballs[4];  // 0:PokeBall, 1:GreatBall, 2:UltraBall, 3:MasterBall
    std::vector<Item> items;  // 背包物品

    Player();
    void initItems();  // 初始化物品
    void addPokemon(const Pokemon& p);
    Pokemon* getCurrentPokemon();
    bool hasAlivePokemon() const;
    int getTotalBalls() const;  // 获取所有精灵球总数
    int getHealPotionCount() const;  // 获取治疗药水数量
    
    // 队伍管理
    bool swapTeamPokemon(int index1, int index2);  // 交换队伍中两只精灵位置
    bool movePokemonToStorage(int teamIndex);  // 将队伍中的精灵放入仓库
    bool movePokemonFromStorage(int storageIndex);  // 将仓库中的精灵放入队伍空位
    bool useItemOnPokemon(int itemIndex, int pokemonIndex, bool isTeam);  // 对精灵使用物品
    bool revivePokemon(int itemIndex, int pokemonIndex);  // 复活阵亡的精灵
};

#endif // PLAYER_H
