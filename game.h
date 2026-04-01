#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "map.h"
#include "battle.h"
#include "team_view.h"
#include <memory>

class Game {
private:
    bool running;
    Player player;
    Map map;
    std::unique_ptr<Battle> battle;
    std::unique_ptr<TeamView> teamView;

    void printCurrentLocationInfo();
    void encounterBattle(const std::vector<std::string>& wildPokemons);

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
