#include "game.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include "config.h"
#include "utils.h"

Game::Game() : running(false) {
    battle = std::make_unique<Battle>(player);
    teamView = std::make_unique<TeamView>(player);
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
    
    // 清除输入缓冲区，防止影响后续的 getline
    std::cin.ignore(1000, '\n');

    Pokemon starter;
    if (choice == "1") {
        starter = Pokemon("小火龙", 5);
    } else if (choice == "2") {
        starter = Pokemon("杰尼龟", 5);
    } else {
        starter = Pokemon("妙蛙种子", 5);
    }

    player.addPokemon(starter);
    map.init();

    running = true;

    std::cout << "\n游戏开始！输入 'help' 查看可用命令。" << std::endl;
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
    const Location& current = map.getCurrentLocation();
    std::cout << "📍 当前位置：【" << current.name << "】" << std::endl;
    std::cout << current.description << std::endl;
    std::cout << "🎒 精灵球：" << player.getTotalBalls() << std::endl;
    std::cout << "👥 队伍：" << player.team.size() << "/" << MAX_TEAM_SIZE << std::endl;
    
    std::cout << "🧭 可移动方向：";
    std::vector<char> directions = map.getAvailableDirections();
    if (directions.empty()) {
        std::cout << "无" << std::endl;
    } else {
        for (size_t i = 0; i < directions.size(); ++i) {
            if (i > 0) std::cout << "、";
            std::cout << Map::getDirectionDisplayName(directions[i]);
        }
        std::cout << std::endl;
    }
    
    std::cout << "🐾 可能遭遇的宝可梦：";
    const std::vector<std::string>& wildPokemons = map.getWildPokemons();
    if (wildPokemons.empty()) {
        std::cout << "无" << std::endl;
    } else {
        for (size_t i = 0; i < wildPokemons.size(); ++i) {
            if (i > 0) std::cout << "、";
            std::cout << wildPokemons[i];
        }
        std::cout << std::endl;
    }
    printSeparator();
}

void Game::processCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;

    action = toLower(action);

    if (action == "help" || action == "h") {
        showHelp();
    } else if (action == "team" || action == "t") {
        if (battle->isInBattle()) {
            std::cout << "战斗中无法查看队伍！" << std::endl;
        } else {
            teamView->open();
        }
    } else if (action == "status" || action == "st") {
        showStatus();
    } else if (action == "quit" || action == "q") {
        std::cout << "感谢游玩，再见！" << std::endl;
        running = false;
    } else if (action.length() == 1) {
        char dir = action[0];
        if (dir == 'e' || dir == 'w' || dir == 's' || dir == 'n') {
            if (map.tryMove(dir)) {
                const Location& newLoc = map.getCurrentLocation();
                std::cout << "向" << Map::getDirectionName(dir) << "移动，来到了【" << newLoc.name << "】" << std::endl;
                std::cout << newLoc.description << std::endl;

                if (map.shouldEncounter()) {
                    encounterBattle(map.getWildPokemons());
                }
            } else {
                std::cout << "无法移动！那个方向没有路。" << std::endl;
            }
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
    std::cout << "精灵球：" << player.getTotalBalls() << std::endl;
    std::cout << "队伍精灵：" << player.team.size() << "/" << MAX_TEAM_SIZE << std::endl;
    std::cout << "================\n" << std::endl;
}

void Game::encounterBattle(const std::vector<std::string>& wildPokemons) {
    std::cout << "\n⚠️  草丛在晃动..." << std::endl;

    int index = rand() % wildPokemons.size();
    std::string wildPokemon = wildPokemons[index];

    std::cout << "🐾 一只野生的【" << wildPokemon << "】出现了！" << std::endl;

    if (!player.hasAlivePokemon()) {
        std::cout << "💤 你的宝可梦都失去了战斗能力，无法对战！" << std::endl;
        std::cout << "🏃 你被迫逃离了！" << std::endl;
        return;
    }

    battle->start(wildPokemon);
    battle->run();
}
