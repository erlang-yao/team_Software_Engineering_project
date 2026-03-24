#include "game.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <windows.h>

Game::Game() : running(false), currentLocation(0) {
    battleState.inBattle = false;
}

void Game::printSeparator() {
    std::cout << "========================================" << std::endl;
}

void Game::run() {
    srand(static_cast<unsigned>(time(nullptr)));

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    initGame();
    gameLoop();
}

void Game::initGame() {
    std::cout << "欢迎来到宝可梦世界！" << std::endl;
    std::cout << "请选择你的初始伙伴：" << std::endl;
    std::cout << "1. 小火龙" << std::endl;
    std::cout << "2. 杰尼龟" << std::endl;
    std::cout << "3. 妙蛙种子" << std::endl;
    std::cout << "> ";

    std::string choice;
    std::cin >> choice;

    Pokemon starter;
    if (choice == "1") {
        starter = Pokemon("小火龙", 5);
    } else if (choice == "2") {
        starter = Pokemon("杰尼龟", 5);
    } else {
        starter = Pokemon("妙蛙种子", 5);
    }

    player.addPokemon(starter);
    
    initMap();
    currentLocation = 0;
    
    running = true;

    std::cout << "\n游戏开始！输入 'help' 查看可用命令。" << std::endl;
}

void Game::initMap() {
    Location loc0;
    loc0.name = "新手村";
    loc0.description = "一个宁静的小村庄，训练家们在这里开始他们的旅程";
    loc0.connections['n'] = 1;
    loc0.connections['e'] = 4;
    loc0.wildPokemons = {"小拉达", "波波", "绿毛虫"};
    locations.push_back(loc0);
    
    Location loc1;
    loc1.name = "望风坡";
    loc1.description = "可以俯瞰整个新手村的高坡，微风拂面";
    loc1.connections['s'] = 0;
    loc1.connections['n'] = 2;
    loc1.wildPokemons = {"波波", "小雀蜂", "独角虫"};
    locations.push_back(loc1);
    
    Location loc2;
    loc2.name = "迷雾森林";
    loc2.description = "被薄雾笼罩的神秘森林，隐藏着许多野生宝可梦";
    loc2.connections['s'] = 1;
    loc2.connections['e'] = 3;
    loc2.wildPokemons = {"绿毛虫", "铁甲蛹", "派拉斯", "皮卡丘"};
    locations.push_back(loc2);
    
    Location loc3;
    loc3.name = "湖边";
    loc3.description = "清澈的湖泊，水面如镜";
    loc3.connections['w'] = 2;
    loc3.connections['n'] = 5;
    loc3.wildPokemons = {"可达鸭", "小海狮", "角金鱼"};
    locations.push_back(loc3);
    
    Location loc4;
    loc4.name = "精灵中心";
    loc4.description = "训练家们休息和治疗宝可梦的地方";
    loc4.connections['w'] = 0;
    loc4.wildPokemons = {};
    locations.push_back(loc4);
    
    Location loc5;
    loc5.name = "山洞入口";
    loc5.description = "阴暗的山洞入口，里面传来神秘的气息";
    loc5.connections['s'] = 3;
    loc5.connections['n'] = 6;
    loc5.wildPokemons = {"小拳石", "大岩蛇", "超音蝠"};
    locations.push_back(loc5);
    
    Location loc6;
    loc6.name = "山洞深处";
    loc6.description = "山洞的最深处，传说中有稀有的宝可梦出没";
    loc6.connections['s'] = 5;
    loc6.wildPokemons = {"大岩蛇", "腕力", "小拳石", "地鼠"};
    locations.push_back(loc6);
}

void Game::gameLoop() {
    while (running) {
        printCurrentLocationInfo();

        std::cout << "> ";
        std::string cmd;
        std::getline(std::cin >> std::ws, cmd);

        if (!cmd.empty()) {
            processCommand(cmd);
        }
    }
}

void Game::printCurrentLocationInfo() {
    printSeparator();
    Location& current = locations[currentLocation];
    std::cout << "📍 当前位置：【" << current.name << "】" << std::endl;
    std::cout << current.description << std::endl;
    std::cout << "🎒 精灵球：" << player.pokeballs << std::endl;
    std::cout << "👥 队伍：" << player.team.size() << "/6" << std::endl;
    std::cout << "🧭 可移动方向：";
    showAvailableDirections();
    std::cout << "🐾 可能遭遇的宝可梦：";
    if (current.wildPokemons.empty()) {
        std::cout << "无" << std::endl;
    } else {
        for (size_t i = 0; i < current.wildPokemons.size(); ++i) {
            if (i > 0) std::cout << "、";
            std::cout << current.wildPokemons[i];
        }
        std::cout << std::endl;
    }
    printSeparator();
}

void Game::processCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;

    std::transform(action.begin(), action.end(), action.begin(), ::tolower);

    if (action == "help" || action == "h") {
        showHelp();
    } else if (action == "team" || action == "t") {
        std::cout << "\n=== 我的队伍 ===" << std::endl;
        for (size_t i = 0; i < player.team.size(); ++i) {
            Pokemon& p = player.team[i];
            std::cout << (i + 1) << ". " << p.name
                      << " (Lv." << p.level << ")" << std::endl;
        }
        std::cout << std::endl;
    } else if (action == "status" || action == "st") {
        showStatus();
    } else if (action == "quit" || action == "q") {
        std::cout << "感谢游玩，再见！" << std::endl;
        running = false;
    } else if (action.length() == 1) {
        char dir = action[0];
        if (dir == 'e' || dir == 'w' || dir == 's' || dir == 'n') {
            tryMove(dir);
        } else {
            std::cout << "未知命令，输入 'help' 查看帮助。" << std::endl;
        }
    } else {
        std::cout << "未知命令，输入 'help' 查看帮助。" << std::endl;
    }
}

void Game::showHelp() {
    std::cout << "\n=== 可用命令 ===" << std::endl;
    std::cout << "e         - 向东移动" << std::endl;
    std::cout << "w         - 向西移动" << std::endl;
    std::cout << "s         - 向南移动" << std::endl;
    std::cout << "n         - 向北移动" << std::endl;
    std::cout << "team(t)   - 查看队伍" << std::endl;
    std::cout << "status(st)- 查看状态" << std::endl;
    std::cout << "help(h)   - 显示帮助" << std::endl;
    std::cout << "quit(q)   - 退出游戏" << std::endl;
    std::cout << "==============\n" << std::endl;
}

void Game::showStatus() {
    std::cout << "\n=== 玩家状态 ===" << std::endl;
    std::cout << "名字：" << player.name << std::endl;
    std::cout << "精灵球：" << player.pokeballs << std::endl;
    std::cout << "队伍精灵：" << player.team.size() << "/6" << std::endl;
    std::cout << "================\n" << std::endl;
}

void Game::tryMove(char direction) {
    Location& current = locations[currentLocation];

    if (current.connections.find(direction) == current.connections.end()) {
        std::cout << "无法移动！那个方向没有路。" << std::endl;
        showAvailableDirections();
        return;
    }

    currentLocation = current.connections[direction];
    Location& newLoc = locations[currentLocation];

    std::cout << "向";
    if (direction == 'e') std::cout << "东";
    else if (direction == 'w') std::cout << "西";
    else if (direction == 's') std::cout << "南";
    else if (direction == 'n') std::cout << "北";
    std::cout << "移动，来到了【" << newLoc.name << "】" << std::endl;
    std::cout << newLoc.description << std::endl;

    // 随机遭遇检查：如果新地点有野生宝可梦，30% 概率触发遭遇
    if (!newLoc.wildPokemons.empty()) {
        int chance = rand() % 100;
        if (chance < 30) {  // 30% 遭遇率
            encounterBattle(newLoc.wildPokemons);
        }
    }
}

void Game::showAvailableDirections() {
    Location& current = locations[currentLocation];
    
    if (current.connections.empty()) {
        std::cout << "无" << std::endl;
        return;
    }
    
    bool first = true;
    if (current.connections.find('e') != current.connections.end()) {
        if (!first) std::cout << "、";
        std::cout << "东 (e)";
        first = false;
    }
    if (current.connections.find('w') != current.connections.end()) {
        if (!first) std::cout << "、";
        std::cout << "西 (w)";
        first = false;
    }
    if (current.connections.find('s') != current.connections.end()) {
        if (!first) std::cout << "、";
        std::cout << "南 (s)";
        first = false;
    }
    if (current.connections.find('n') != current.connections.end()) {
        if (!first) std::cout << "、";
        std::cout << "北 (n)";
        first = false;
    }
    std::cout << std::endl;
}

void Game::encounterBattle(const std::vector<std::string>& wildPokemons) {
    std::cout << "\n⚠️  草丛在晃动..." << std::endl;
    
    // 随机选择一只野生宝可梦
    int index = rand() % wildPokemons.size();
    std::string wildPokemon = wildPokemons[index];
    
    std::cout << "🐾 一只野生的【" << wildPokemon << "】出现了！" << std::endl;
    
    // 检查玩家是否有存活的宝可梦
    if (!player.hasAlivePokemon()) {
        std::cout << "💤 你的宝可梦都失去了战斗能力，无法对战！" << std::endl;
        std::cout << "🏃 你被迫逃离了！" << std::endl;
        return;
    }
    
    startBattle(wildPokemon);
}

void Game::startBattle(const std::string& wildPokemonSpecies) {
    battleState.inBattle = true;
    battleState.wildPokemon = Pokemon(wildPokemonSpecies, 5);
    battleState.playerPokeIndex = 0;
    
    // 根据速度决定谁先行动
    Pokemon& playerPoke = player.team[battleState.playerPokeIndex];
    if (playerPoke.stats.speed >= battleState.wildPokemon.stats.speed) {
        battleState.playerTurn = true;
    } else {
        battleState.playerTurn = false;
    }
    
    std::cout << "\n===============================" << std::endl;
    std::cout << "⚔️  战斗开始！" << std::endl;
    std::cout << "===============================" << std::endl;
    
    battleLoop();
}

void Game::battleLoop() {
    while (battleState.inBattle) {
        // 显示战斗信息
        Pokemon& playerPoke = player.team[battleState.playerPokeIndex];
        Pokemon& enemyPoke = battleState.wildPokemon;
        
        std::cout << "\n--- 战斗状态 ---" << std::endl;
        std::cout << "🐾 【" << enemyPoke.name << "】(Lv." << enemyPoke.level 
                  << ") HP: " << enemyPoke.stats.hp << "/" << enemyPoke.stats.maxHp << std::endl;
        std::cout << "🧭 【" << playerPoke.name << "】(Lv." << playerPoke.level 
                  << ") HP: " << playerPoke.stats.hp << "/" << playerPoke.stats.maxHp << std::endl;
        std::cout << "----------------" << std::endl;
        
        if (battleState.playerTurn) {
            playerTurn();
        } else {
            enemyTurn();
        }
        
        checkBattleEnd();
    }
}

void Game::showBattleMenu() {
    std::cout << "\n=== 战斗菜单 ===" << std::endl;
    std::cout << "1. 📋 技能" << std::endl;
    std::cout << "2. ⚪ 精灵球 (剩余：" << player.pokeballs << ")" << std::endl;
    std::cout << "3. 💊 治疗" << std::endl;
    std::cout << "4. 🏃 逃跑" << std::endl;
    std::cout << "> ";
}

void Game::showMoveSelection() {
    Pokemon& playerPoke = player.team[battleState.playerPokeIndex];
    
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
}

void Game::playerTurn() {
    std::cout << "\n👉 你的回合！" << std::endl;
    
    showBattleMenu();
    std::string choice;
    std::cin >> choice;
    
    Pokemon& playerPoke = player.team[battleState.playerPokeIndex];
    Pokemon& enemyPoke = battleState.wildPokemon;
    
    if (choice == "1") {
        // 技能
        showMoveSelection();
        std::string moveChoice;
        std::cin >> moveChoice;
        
        if (moveChoice >= "1" && moveChoice <= "4") {
            int moveIndex = std::stoi(moveChoice) - 1;
            if (moveIndex < (int)playerPoke.moves.size()) {
                useMove(playerPoke, enemyPoke, moveIndex, true);
                battleState.playerTurn = false;
                return;
            }
        }
        std::cout << "无效的选择！" << std::endl;
        playerTurn();  // 重新选择
    } else if (choice == "2") {
        // 捕捉
        catchPokemon();
        battleState.playerTurn = false;
    } else if (choice == "3") {
        // 治疗
        healPokemon();
        battleState.playerTurn = false;
    } else if (choice == "4") {
        // 逃跑
        if (tryEscape()) {
            battleState.inBattle = false;
        } else {
            battleState.playerTurn = false;
        }
    } else {
        std::cout << "无效的选择！" << std::endl;
        playerTurn();  // 重新选择
    }
}

void Game::enemyTurn() {
    std::cout << "\n👉 敌方回合！" << std::endl;
    
    Pokemon& playerPoke = player.team[battleState.playerPokeIndex];
    Pokemon& enemyPoke = battleState.wildPokemon;
    
    // 敌人随机选择一个技能
    if (!enemyPoke.moves.empty()) {
        int moveIndex = rand() % enemyPoke.moves.size();
        useMove(enemyPoke, playerPoke, moveIndex, false);
    }
    
    battleState.playerTurn = true;
}

void Game::useMove(Pokemon& attacker, Pokemon& defender, int moveIndex, bool isPlayer) {
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
        
        // 属性克制
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
        // 变化技能（暂时只提示）
        std::cout << "💫 但是这是变化技能，效果尚未实现！" << std::endl;
    }
}

int Game::calculateDamage(const Pokemon& attacker, const Pokemon& defender, const Move& move) {
    // 简化版伤害公式（调整后的数值更平衡）
    // 基础伤害 = 威力 * (攻击/防御) * 0.3 系数
    float baseDamage = (float)move.power * attacker.stats.attack / defender.stats.defense * 0.3f;
    float stab = (move.type == attacker.type) ? 1.5f : 1.0f;  // 本系加成
    float effectiveness = getTypeEffectiveness(move.type, defender.type);
    float random = (float)(rand() % 16 + 85) / 100.0f;  // 0.85 - 1.00 随机波动

    int damage = (int)(baseDamage * stab * effectiveness * random);
    if (damage < 1) damage = 1;

    return damage;
}

void Game::catchPokemon() {
    Pokemon& enemyPoke = battleState.wildPokemon;
    
    if (player.pokeballs <= 0) {
        std::cout << "❌ 没有精灵球了！" << std::endl;
        return;
    }
    
    player.pokeballs--;
    
    // 捕捉率计算：血量越低越容易捕捉
    float catchRate = (float)enemyPoke.stats.hp / enemyPoke.stats.maxHp;
    int randomRoll = rand() % 100;
    int threshold = (int)((1.0f - catchRate) * 100);
    
    std::cout << "\n🔵 投出精灵球..." << std::endl;
    
    if (randomRoll < threshold) {
        std::cout << "💫 摇晃...摇晃...咔嚓！" << std::endl;
        std::cout << "🎉 成功收服了 " << enemyPoke.name << "！" << std::endl;
        
        // 将野生宝可梦加入队伍
        Pokemon caught = enemyPoke;
        caught.stats.hp = caught.stats.maxHp;  // 收服后恢复满 HP
        player.addPokemon(caught);
        
        battleState.inBattle = false;
    } else {
        std::cout << "💨 哎呀！宝可梦挣脱了！" << std::endl;
    }
}

void Game::healPokemon() {
    Pokemon& playerPoke = player.team[battleState.playerPokeIndex];
    
    // 治疗当前宝可梦
    int healAmount = playerPoke.stats.maxHp / 2;
    playerPoke.stats.hp += healAmount;
    if (playerPoke.stats.hp > playerPoke.stats.maxHp) {
        playerPoke.stats.hp = playerPoke.stats.maxHp;
    }
    
    std::cout << "\n💊 使用了伤药！" << std::endl;
    std::cout << "❤️  " << playerPoke.name << " 恢复了 " << healAmount << " 点 HP！" << std::endl;
}

bool Game::tryEscape() {
    Pokemon& playerPoke = player.team[battleState.playerPokeIndex];
    Pokemon& enemyPoke = battleState.wildPokemon;
    
    // 逃跑成功率基于速度差
    int escapeChance = 50 + (playerPoke.stats.speed - enemyPoke.stats.speed) * 2;
    if (escapeChance < 30) escapeChance = 30;
    if (escapeChance > 95) escapeChance = 95;
    
    int randomRoll = rand() % 100;
    
    if (randomRoll < escapeChance) {
        std::cout << "\n🏃 成功逃脱了！" << std::endl;
        return true;
    } else {
        std::cout << "\n🚫 逃跑失败了！" << std::endl;
        return false;
    }
}

void Game::checkBattleEnd() {
    Pokemon& playerPoke = player.team[battleState.playerPokeIndex];
    Pokemon& enemyPoke = battleState.wildPokemon;
    
    // 检查敌方是否濒死
    if (enemyPoke.isFainted()) {
        std::cout << "\n💫 " << enemyPoke.name << " 失去了战斗能力！" << std::endl;
        std::cout << "🎉 战斗胜利！获得了经验值！" << std::endl;
        
        // 简单经验系统
        playerPoke.exp += 20;
        if (playerPoke.exp >= playerPoke.maxExp) {
            playerPoke.level++;
            playerPoke.maxExp += 50;
            std::cout << "🎊 " << playerPoke.name << " 升到了 Lv." << playerPoke.level << "！" << std::endl;
        }
        
        battleState.inBattle = false;
        return;
    }
    
    // 检查玩家宝可梦是否濒死
    if (playerPoke.isFainted()) {
        std::cout << "\n💫 " << playerPoke.name << " 失去了战斗能力！" << std::endl;
        
        // 寻找下一个存活的宝可梦
        bool foundAlive = false;
        for (size_t i = 0; i < player.team.size(); ++i) {
            if (!player.team[i].isFainted()) {
                battleState.playerPokeIndex = i;
                foundAlive = true;
                std::cout << "👉 派出 " << player.team[i].name << "！" << std::endl;
                break;
            }
        }
        
        if (!foundAlive) {
            std::cout << "💤 所有宝可梦都失去了战斗能力！" << std::endl;
            std::cout << "🏃 被迫逃离了战斗！" << std::endl;
            battleState.inBattle = false;
        }
    }
}
