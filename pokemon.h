#ifndef POKEMON_H
#define POKEMON_H

#include <string>
#include <vector>

// 9 种属性类型
enum class Type {
    Normal,     // 一般
    Fire,       // 火
    Water,      // 水
    Grass,      // 草
    Electric,   // 电
    Ice,        // 冰
    Fighting,   // 格斗
    Ground,     // 地面
    Flying      // 飞行
};

// 精灵球类型（按等级从低到高）
enum class PokeBallType {
    PokeBall,       // 普通精灵球 - 捕捉倍率 1.0
    GreatBall,      // 超级球 - 捕捉倍率 1.5
    UltraBall,      // 高级球 - 捕捉倍率 2.0
    MasterBall      // 大师球 - 捕捉倍率 5.0（几乎必中）
};

// 精灵球属性
struct PokeBall {
    PokeBallType type;
    std::string name;
    float catchRate;    // 捕捉倍率
};

// 获取精灵球信息
PokeBall getPokeBallInfo(PokeBallType type);

// 技能结构
struct Move {
    std::string name;
    Type type;
    int power;
    int accuracy;
    bool isSpecial;  // 是否为专属技能
};

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
    Type type;
    int level;
    Stats stats;
    int exp;
    int maxExp;
    std::vector<Move> moves;

    Pokemon();
    Pokemon(const std::string& sp, int lvl);
    void initStats();
    void initMoves();
    bool isFainted() const;
};

// 属性克制关系：返回克制倍率 (1.0=正常，2.0=克制，0.5=被克制)
float getTypeEffectiveness(Type attackType, Type defendType);
std::string getTypeName(Type type);

#endif // POKEMON_H
