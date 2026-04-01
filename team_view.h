#ifndef TEAM_VIEW_H
#define TEAM_VIEW_H

#include "player.h"
#include <string>

// 查看队伍界面状态
struct TeamViewState {
    bool viewing;
    int storagePage;         // 仓库当前页码
    int pokemonsPerPage;     // 每页显示的精灵数量
};

class TeamView {
private:
    TeamViewState state;
    Player& player;

    void showStorageView();
    void showItemView();

public:
    TeamView(Player& player);

    void open();
    void run();
    bool isOpen() const;
    void handleCommand(const std::string& cmd);
    
    // 显示菜单
    void showMenu();
};

#endif // TEAM_VIEW_H
