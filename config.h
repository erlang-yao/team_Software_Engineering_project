#ifndef CONFIG_H
#define CONFIG_H

// ==================== 战斗系统配置 ====================
constexpr float DAMAGE_BASE_MULTIPLIER = 0.3f;     // 伤害基础系数
constexpr float STAB_MULTIPLIER = 1.5f;            // 本系技能加成
constexpr float MIN_DAMAGE_MULTIPLIER = 0.85f;     // 最小随机波动
constexpr float MAX_DAMAGE_MULTIPLIER = 1.0f;      // 最大随机波动
constexpr int MIN_DAMAGE = 1;                      // 最小伤害值

constexpr int EXP_BASE_GAIN = 20;                  // 基础经验获得
constexpr int EXP_LEVEL_BASE = 50;                 // 升级经验基数

// ==================== 物品系统配置 ====================
constexpr int HEAL_POTION_PERCENT = 33;            // 治疗药水恢复百分比
constexpr int EXP_POTION_AMOUNT = 50;              // 经验药水经验值
constexpr int REVIVE_HP_AMOUNT = 1;                // 复活后生命值

// ==================== 遭遇系统配置 ====================
constexpr int ENCOUNTER_CHANCE_PERCENT = 30;       // 野生宝可梦遭遇概率 (%)

// ==================== 逃跑系统配置 ====================
constexpr int ESCAPE_BASE_CHANCE = 50;             // 逃跑基础概率
constexpr int ESCAPE_SPEED_MULTIPLIER = 2;         // 速度差系数
constexpr int ESCAPE_MIN_CHANCE = 30;              // 最小逃跑概率
constexpr int ESCAPE_MAX_CHANCE = 95;              // 最大逃跑概率

// ==================== 捕捉系统配置 ====================
constexpr float CATCH_LEVEL_FACTOR_BASE = 0.2f;    // 等级修正系数
constexpr float CATCH_MIN_PROBABILITY = 1.0f;      // 最小捕捉概率
constexpr float CATCH_MAX_PROBABILITY = 100.0f;    // 最大捕捉概率
constexpr float MASTERBALL_CATCH_RATE = 99.0f;     // 大师球捕捉概率

// ==================== 队伍系统配置 ====================
constexpr int MAX_TEAM_SIZE = 6;                   // 队伍最大容量
constexpr int POKEMONS_PER_PAGE = 5;               // 仓库每页显示数量

// ==================== 游戏系统配置 ====================
constexpr int STARTER_POKEBALL = 10;               // 初始精灵球
constexpr int STARTER_GREATBALL = 3;               // 初始超级球
constexpr int STARTER_ULTRABALL = 1;               // 初始高级球
constexpr int STARTER_MASTERBALL = 0;              // 初始大师球
constexpr int STARTER_HEAL_POTION = 5;             // 初始治疗药水
constexpr int STARTER_EXP_POTION = 3;              // 初始经验药水
constexpr int STARTER_REVIVE = 2;                  // 初始复活药剂

#endif // CONFIG_H
