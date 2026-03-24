#ifndef GAME_H
#define GAME_H

#include "player.h"
#include <vector>
#include <map>

struct Location {
    std::string name;
    std::string description;
    std::map<char, int> connections;  // 方向 -> 相连地图索引
    std::vector<std::string> wildPokemons;  // 可遭遇的野生宝可梦
};

class Game {
private:
    bool running;
    Player player;
    
    // 地图相关
    std::vector<Location> locations;
    int currentLocation;
    
    void tryMove(char direction);
    void showAvailableDirections();
    void printCurrentLocationInfo();

public:
    Game();

    void run();
    void initGame();
    void initMap();

    void gameLoop();
    void processCommand(const std::string& cmd);
    void showHelp();
    void showStatus();

    void printSeparator();
};

#endif // GAME_H
