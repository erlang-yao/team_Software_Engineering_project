#ifndef GAME_H
#define GAME_H

#include "player.h"

class Game {
private:
    bool running;
    Player player;

public:
    Game();

    void run();
    void initGame();

    void gameLoop();
    void processCommand(const std::string& cmd);
    void showHelp();
    void showStatus();

    void printSeparator();
};

#endif // GAME_H
