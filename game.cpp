#include "game.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <windows.h>

Game::Game() : running(false) {}

void Game::printSeparator() {
    std::cout << "========================================" << std::endl;
}

void Game::run() {
    srand(static_cast<unsigned>(time(nullptr)));

    // 新增：设置控制台输入/输出编码为UTF-8（核心修复代码）
    SetConsoleOutputCP(CP_UTF8);  // 设置输出编码为UTF-8
    SetConsoleCP(CP_UTF8);        // 设置输入编码为UTF-8（输入中文也不会乱码）

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
    running = true;

    std::cout << "\n游戏开始！输入 'help' 查看可用命令。" << std::endl;
}

void Game::gameLoop() {
    while (running) {
        printSeparator();
        std::cout << "📍 当前位置：真新镇郊外" << std::endl;
        std::cout << "🎒 精灵球：" << player.pokeballs << std::endl;
        std::cout << "👥 队伍：" << player.team.size() << "/6" << std::endl;
        printSeparator();
        
        std::cout << "> ";
        std::string cmd;
        std::getline(std::cin >> std::ws, cmd);
        
        if (!cmd.empty()) {
            processCommand(cmd);
        }
    }
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
    } else {
        std::cout << "未知命令，输入 'help' 查看帮助。" << std::endl;
    }
}

void Game::showHelp() {
    std::cout << "\n=== 可用命令 ===" << std::endl;
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
