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

// 战斗状态
struct BattleState {
    bool inBattle;
    Pokemon wildPokemon;      // 野生宝可梦
    int playerPokeIndex;      // 玩家当前出战的宝可梦索引
    bool playerTurn;          // 是否玩家回合
};

// 查看队伍界面状态
struct TeamViewState {
    bool viewing;
    int storagePage;         // 仓库当前页码
    int pokemonsPerPage;     // 每页显示的精灵数量
};

class Game {
private:
    bool running;
    Player player;

    // 地图相关
    std::vector<Location> locations;
    int currentLocation;

    // 战斗相关
    BattleState battleState;
    
    // 查看队伍界面相关
    TeamViewState teamViewState;

    void tryMove(char direction);
    void showAvailableDirections();
    void printCurrentLocationInfo();
    void encounterBattle(const std::vector<std::string>& wildPokemons);

    // 战斗系统
    void startBattle(const std::string& wildPokemonSpecies);
    void battleLoop();
    void playerTurn();
    void enemyTurn();
    void useMove(Pokemon& attacker, Pokemon& defender, int moveIndex, bool isPlayer);
    void showBattleMenu();
    void showMoveSelection();
    void catchPokemon();
    void healPokemon();
    bool switchPokemon();  // 更换出战精灵，返回是否成功切换
    bool tryEscape();
    void checkBattleEnd();
    int calculateDamage(const Pokemon& attacker, const Pokemon& defender, const Move& move);
    
    // 查看队伍系统（闲置状态）
    void openTeamView();
    void teamViewLoop();
    void showTeamViewMenu();
    void showStorageView();
    void showItemView();
    void handleTeamViewCommand(const std::string& cmd);

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
