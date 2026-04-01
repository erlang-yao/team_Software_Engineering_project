#include "battle.h"
#include <iostream>
#include <cstdlib>
#include "config.h"
#include "utils.h"

Battle::Battle(Player& player) : player(player) {
    state.inBattle = false;
}

void Battle::start(const std::string& wildPokemonSpecies) {
    state.inBattle = true;
    state.wildPokemon = Pokemon(wildPokemonSpecies, 5);
    state.playerPokeIndex = 0;

    // 根据速度决定谁先行动
    Pokemon& playerPoke = player.team[state.playerPokeIndex];
    state.playerTurn = playerPoke.stats.speed >= state.wildPokemon.stats.speed;

    std::cout << "\n===============================" << std::endl;
    std::cout << "⚔️  战斗开始！" << std::endl;
    std::cout << "===============================" << std::endl;
}

bool Battle::isInBattle() const {
    return state.inBattle;
}

const BattleState& Battle::getState() const {
    return state;
}

void Battle::run() {
    while (state.inBattle) {
        // 显示战斗信息
        Pokemon& playerPoke = player.team[state.playerPokeIndex];
        Pokemon& enemyPoke = state.wildPokemon;

        std::cout << "\n--- 战斗状态 ---" << std::endl;
        std::cout << "🐾 【" << enemyPoke.name << "】(Lv." << enemyPoke.level
                  << ") HP: " << enemyPoke.stats.hp << "/" << enemyPoke.stats.maxHp << std::endl;
        std::cout << "🧭 【" << playerPoke.name << "】(Lv." << playerPoke.level
                  << ") HP: " << playerPoke.stats.hp << "/" << playerPoke.stats.maxHp << std::endl;
        std::cout << "----------------" << std::endl;

        if (state.playerTurn) {
            std::cout << "\n👉 你的回合！" << std::endl;
            
            // 显示战斗菜单
            std::cout << "\n=== 战斗菜单 ===" << std::endl;
            std::cout << "1. 📋 技能" << std::endl;
            std::cout << "2. ⚪ 精灵球 (剩余：" << player.getTotalBalls() << ")" << std::endl;
            std::cout << "3. 💊 治疗 (剩余：" << player.getHealPotionCount() << ")" << std::endl;
            std::cout << "4. 🔄 更换精灵" << std::endl;
            std::cout << "5. 🏃 逃跑" << std::endl;
            std::cout << "> ";
            
            std::string choice;
            std::cin >> choice;
            
            if (choice == "1") {
                // 技能选择
                Pokemon& playerPoke = player.team[state.playerPokeIndex];
                std::cout << "\n=== 选择技能 ===" << std::endl;
                for (size_t i = 0; i < playerPoke.moves.size(); ++i) {
                    Move& move = playerPoke.moves[i];
                    std::cout << (i + 1) << ". " << move.name
                              << " [" << getTypeName(move.type) << "]"
                              << " 威力:" << move.power;
                    if (move.isSpecial) std::cout << " ⭐";
                    std::cout << std::endl;
                }
                std::cout << "0. 返回" << std::endl;
                std::cout << "> ";
                
                std::string moveChoice;
                std::cin >> moveChoice;
                
                if (moveChoice >= "1" && moveChoice <= "4") {
                    int moveIndex = std::stoi(moveChoice) - 1;
                    if (moveIndex < (int)playerPoke.moves.size()) {
                        useMove(playerPoke, enemyPoke, moveIndex, true);
                        state.playerTurn = false;
                    } else {
                        std::cout << "无效的选择！" << std::endl;
                    }
                } else if (moveChoice != "0") {
                    std::cout << "无效的选择！" << std::endl;
                }
            } else if (choice == "2") {
                // 捕捉
                std::cout << "\n=== 选择精灵球 ===" << std::endl;
                const char* ballNames[] = {"精灵球", "超级球", "高级球", "大师球"};
                for (int i = 0; i < 4; ++i) {
                    std::cout << (i + 1) << ". " << ballNames[i];
                    if (player.pokeballs[i] > 0) {
                        std::cout << " (持有：" << player.pokeballs[i] << "个)";
                    } else {
                        std::cout << " (未持有)";
                    }
                    std::cout << std::endl;
                }
                std::cout << "0. 返回" << std::endl;
                std::cout << "> ";
                
                std::string ballChoice;
                std::cin >> ballChoice;
                
                if (ballChoice >= "1" && ballChoice <= "4") {
                    int ballIndex = std::stoi(ballChoice) - 1;
                    if (player.pokeballs[ballIndex] > 0) {
                        catchPokemon(ballIndex);
                        state.playerTurn = false;
                    } else {
                        std::cout << "这种精灵球已经用完了！" << std::endl;
                    }
                }
            } else if (choice == "3") {
                // 治疗
                healPokemon();
                state.playerTurn = false;
            } else if (choice == "4") {
                // 更换精灵
                if (switchPokemon()) {
                    state.playerTurn = false;
                }
            } else if (choice == "5") {
                // 逃跑
                if (tryEscape()) {
                    state.inBattle = false;
                } else {
                    state.playerTurn = false;
                }
            }
        } else {
            enemyTurn();
        }

        checkBattleEnd();
    }
}

void Battle::enemyTurn() {
    std::cout << "\n👉 敌方回合！" << std::endl;

    Pokemon& playerPoke = player.team[state.playerPokeIndex];
    Pokemon& enemyPoke = state.wildPokemon;

    // 敌人随机选择一个技能
    if (!enemyPoke.moves.empty()) {
        int moveIndex = rand() % enemyPoke.moves.size();
        useMove(enemyPoke, playerPoke, moveIndex, false);
    }

    state.playerTurn = true;
}

int Battle::calculateDamage(const Pokemon& attacker, const Pokemon& defender, const Move& move) {
    float baseDamage = (float)move.power * attacker.stats.attack / defender.stats.defense * DAMAGE_BASE_MULTIPLIER;
    float stab = (move.type == attacker.type) ? STAB_MULTIPLIER : 1.0f;
    float effectiveness = getTypeEffectiveness(move.type, defender.type);
    float random = (float)(rand() % 16 + 85) / 100.0f;

    int damage = (int)(baseDamage * stab * effectiveness * random);
    if (damage < MIN_DAMAGE) damage = MIN_DAMAGE;

    return damage;
}

void Battle::useMove(Pokemon& attacker, Pokemon& defender, int moveIndex, bool isPlayer) {
    if (moveIndex < 0 || moveIndex >= (int)attacker.moves.size()) {
        std::cout << "技能无效！" << std::endl;
        return;
    }

    Move& move = attacker.moves[moveIndex];

    std::cout << "\n✨ " << attacker.name << " 使用了 " << move.name << "！" << std::endl;

    // 检查命中率
    int accuracyRoll = rand() % 100 + 1;
    if (accuracyRoll > move.accuracy) {
        std::cout << "💨 但是攻击落空了！" << std::endl;
        return;
    }

    // 计算伤害
    if (move.power > 0) {
        int damage = calculateDamage(attacker, defender, move);
        float effectiveness = getTypeEffectiveness(move.type, defender.type);

        if (effectiveness == 0.0f) {
            std::cout << "🚫 对" << getTypeName(defender.type) << "属性无效！" << std::endl;
            return;
        }

        if (effectiveness > 1.0f) {
            std::cout << "🔥 效果绝佳！" << std::endl;
        } else if (effectiveness < 1.0f) {
            std::cout << "🛡️ 效果不好..." << std::endl;
        }

        defender.stats.hp -= damage;
        if (defender.stats.hp < 0) defender.stats.hp = 0;

        std::cout << "💥 造成了 " << damage << " 点伤害！" << std::endl;
    } else {
        std::cout << "💫 但是这是变化技能，效果尚未实现！" << std::endl;
    }
}

bool Battle::tryEscape() {
    Pokemon& playerPoke = player.team[state.playerPokeIndex];
    Pokemon& enemyPoke = state.wildPokemon;

    int escapeChance = ESCAPE_BASE_CHANCE + (playerPoke.stats.speed - enemyPoke.stats.speed) * ESCAPE_SPEED_MULTIPLIER;
    if (escapeChance < ESCAPE_MIN_CHANCE) escapeChance = ESCAPE_MIN_CHANCE;
    if (escapeChance > ESCAPE_MAX_CHANCE) escapeChance = ESCAPE_MAX_CHANCE;

    int randomRoll = rand() % 100;

    if (randomRoll < escapeChance) {
        std::cout << "\n🏃 成功逃脱了！" << std::endl;
        return true;
    } else {
        std::cout << "\n🚫 逃跑失败了！" << std::endl;
        return false;
    }
}

void Battle::catchPokemon(int ballIndex) {
    Pokemon& enemyPoke = state.wildPokemon;

    // 消耗精灵球
    player.pokeballs[ballIndex]--;
    PokeBall ball = getPokeBallInfo(static_cast<PokeBallType>(ballIndex));

    // 计算捕捉概率
    float hpRatio = (float)enemyPoke.stats.hp / enemyPoke.stats.maxHp;
    float baseRate = (1.0f - hpRatio) * 100.0f;
    float levelFactor = 1.0f / (1.0f + enemyPoke.level * CATCH_LEVEL_FACTOR_BASE);
    float ballMultiplier = ball.catchRate;
    float catchProbability = baseRate * levelFactor * ballMultiplier;

    if (ballIndex == 3) {  // 大师球
        catchProbability = MASTERBALL_CATCH_RATE;
    }

    if (catchProbability < CATCH_MIN_PROBABILITY) catchProbability = CATCH_MIN_PROBABILITY;
    if (catchProbability > CATCH_MAX_PROBABILITY) catchProbability = CATCH_MAX_PROBABILITY;

    std::cout << "\n🔵 投出" << ball.name << "..." << std::endl;

    int randomRoll = rand() % 100;
    if ((float)randomRoll < catchProbability) {
        std::cout << "💫 摇晃...摇晃...咔嚓！" << std::endl;
        std::cout << "🎉 成功收服了 " << enemyPoke.name << "！" << std::endl;

        Pokemon caught = enemyPoke;
        caught.stats.hp = caught.stats.maxHp;
        player.addPokemon(caught);

        state.inBattle = false;
    } else {
        std::cout << "💨 哎呀！宝可梦挣脱了！" << std::endl;
    }
}

bool Battle::switchPokemon() {
    int aliveCount = 0;
    for (const auto& p : player.team) {
        if (!p.isFainted()) {
            aliveCount++;
        }
    }

    if (aliveCount <= 1) {
        std::cout << "❌ 没有其他可以出战的精灵了！" << std::endl;
        return false;
    }

    std::cout << "\n=== 选择出战的精灵 ===" << std::endl;
    for (size_t i = 0; i < player.team.size(); ++i) {
        Pokemon& p = player.team[i];
        std::string marker = "";
        if (i == (size_t)state.playerPokeIndex) {
            marker = " [当前出战]";
        } else if (p.isFainted()) {
            marker = " [无法战斗]";
        }
        std::cout << (i + 1) << ". " << p.name << " (Lv." << p.level << ") "
                  << "HP:" << p.stats.hp << "/" << p.stats.maxHp << marker << std::endl;
    }
    std::cout << "0. 返回" << std::endl;
    std::cout << "> ";

    std::string choice;
    std::cin >> choice;

    if (choice == "0") {
        return false;
    }

    int pokeIdx;
    if (!tryParseInt(choice, pokeIdx) || pokeIdx < 1 || pokeIdx > (int)player.team.size()) {
        std::cout << "无效的选择！" << std::endl;
        return switchPokemon();
    }

    int newIndex = pokeIdx - 1;

    if (newIndex == state.playerPokeIndex) {
        std::cout << "❌ 这只精灵已经在出战中！" << std::endl;
        return switchPokemon();
    }

    if (player.team[newIndex].isFainted()) {
        std::cout << "❌ 这只精灵已经失去战斗能力，无法出战！" << std::endl;
        return switchPokemon();
    }

    state.playerPokeIndex = newIndex;
    std::cout << "👉 切换 " << player.team[state.playerPokeIndex].name << " 出战！" << std::endl;
    std::cout << "⚠️  消耗了一个回合！" << std::endl;
    return true;
}

void Battle::healPokemon() {
    Pokemon& playerPoke = player.team[state.playerPokeIndex];

    if (player.getHealPotionCount() <= 0) {
        std::cout << "❌ 没有治疗药水了！" << std::endl;
        return;
    }

    if (playerPoke.isFainted()) {
        std::cout << "❌ " << playerPoke.name << " 已经失去战斗能力，无法使用治疗药水！" << std::endl;
        return;
    }

    int healAmount = playerPoke.stats.maxHp * HEAL_POTION_PERCENT / 100;
    if (healAmount < 1) healAmount = 1;

    playerPoke.stats.hp += healAmount;
    if (playerPoke.stats.hp > playerPoke.stats.maxHp) {
        playerPoke.stats.hp = playerPoke.stats.maxHp;
    }

    for (auto& item : player.items) {
        if (!item.isRevive && item.healPercent > 0) {
            item.count--;
            break;
        }
    }

    std::cout << "\n💊 使用了治疗药水！" << std::endl;
    std::cout << "❤️  " << playerPoke.name << " 恢复了 " << healAmount << " 点 HP！"
              << " (当前 HP: " << playerPoke.stats.hp << "/" << playerPoke.stats.maxHp << ")" << std::endl;
}

bool Battle::hasAlivePokemon() const {
    return player.hasAlivePokemon();
}

void Battle::checkBattleEnd() {
    Pokemon& playerPoke = player.team[state.playerPokeIndex];
    Pokemon& enemyPoke = state.wildPokemon;

    if (enemyPoke.isFainted()) {
        std::cout << "\n💫 " << enemyPoke.name << " 失去了战斗能力！" << std::endl;
        std::cout << "🎉 战斗胜利！获得了经验值！" << std::endl;

        playerPoke.exp += EXP_BASE_GAIN;
        if (playerPoke.exp >= playerPoke.maxExp) {
            playerPoke.level++;
            playerPoke.maxExp += EXP_LEVEL_BASE;
            std::cout << "🎊 " << playerPoke.name << " 升到了 Lv." << playerPoke.level << "！" << std::endl;
        }

        state.inBattle = false;
        return;
    }

    if (playerPoke.isFainted()) {
        std::cout << "\n💫 " << playerPoke.name << " 失去了战斗能力！" << std::endl;

        bool foundAlive = false;
        for (size_t i = 0; i < player.team.size(); ++i) {
            if (!player.team[i].isFainted()) {
                state.playerPokeIndex = i;
                foundAlive = true;
                std::cout << "👉 派出 " << player.team[i].name << "！" << std::endl;
                break;
            }
        }

        if (!foundAlive) {
            std::cout << "💤 所有宝可梦都失去了战斗能力！" << std::endl;
            std::cout << "🏃 被迫逃离了战斗！" << std::endl;
            state.inBattle = false;
        }
    }
}
