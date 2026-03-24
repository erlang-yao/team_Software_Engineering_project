#include "game.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <windows.h>

Game::Game() : running(false), currentLocation(0) {}

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
