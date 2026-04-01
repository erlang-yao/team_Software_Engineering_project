#include "team_view.h"
#include <iostream>
#include <sstream>
#include "config.h"
#include "utils.h"

TeamView::TeamView(Player& player) : player(player) {
    state.viewing = false;
    state.storagePage = 0;
    state.pokemonsPerPage = POKEMONS_PER_PAGE;
}

void TeamView::open() {
    state.viewing = true;
    state.storagePage = 0;
    std::cout << "\n📋 进入查看队伍界面" << std::endl;
    run();
}

bool TeamView::isOpen() const {
    return state.viewing;
}

void TeamView::run() {
    while (state.viewing) {
        showMenu();

        std::cout << "> ";
        std::string cmd;
        std::getline(std::cin >> std::ws, cmd);

        if (!cmd.empty()) {
            handleCommand(cmd);
        }
    }
}

void TeamView::showMenu() {
    std::cout << "\n=== 🎒 背包中的精灵 ===" << std::endl;
    std::cout << "队伍：" << player.team.size() << "/" << MAX_TEAM_SIZE << std::endl;
    for (size_t i = 0; i < player.team.size(); ++i) {
        Pokemon& p = player.team[i];
        std::cout << (i + 1) << ". " << p.name << " (Lv." << p.level << ") "
                  << "HP:" << p.stats.hp << "/" << p.stats.maxHp << std::endl;
    }

    std::cout << "\n=== 🏠 仓库中的精灵 ===" << std::endl;
    std::cout << "仓库：" << player.storage.size() << " 只" << std::endl;
    showStorageView();

    std::cout << "\n=== 📦 背包物品 ===" << std::endl;
    showItemView();

    std::cout << "\n命令:" << std::endl;
    std::cout << "  swap [1] [2]  - 调整队伍精灵位置" << std::endl;
    std::cout << "  use [物品编号] [精灵编号] - 使用物品" << std::endl;
    std::cout << "  stor [背包编号] - 将背包精灵放入仓库" << std::endl;
    std::cout << "  take [仓库编号] - 将仓库精灵放入背包空位" << std::endl;
    std::cout << "  page [页码]   - 翻页 (仓库)" << std::endl;
    std::cout << "  q             - 返回" << std::endl;
}

void TeamView::showStorageView() {
    if (player.storage.empty()) {
        std::cout << "(仓库为空)" << std::endl;
        return;
    }

    int start = state.storagePage * state.pokemonsPerPage;
    int end = start + state.pokemonsPerPage;
    if (start >= (int)player.storage.size()) {
        state.storagePage = 0;
        start = 0;
        end = state.pokemonsPerPage;
    }

    int totalPages = (player.storage.size() + state.pokemonsPerPage - 1) / state.pokemonsPerPage;
    std::cout << "第 " << (state.storagePage + 1) << "/" << totalPages << " 页" << std::endl;

    for (int i = start; i < end && i < (int)player.storage.size(); ++i) {
        Pokemon& p = player.storage[i];
        int displayIndex = i - start + 1;
        std::cout << displayIndex << ". " << p.name << " (Lv." << p.level << ") "
                  << "HP:" << p.stats.hp << "/" << p.stats.maxHp << std::endl;
    }
}

void TeamView::showItemView() {
    if (player.items.empty()) {
        std::cout << "(背包为空)" << std::endl;
        return;
    }

    for (size_t i = 0; i < player.items.size(); ++i) {
        Item& item = player.items[i];
        std::cout << (i + 1) << ". " << item.name << " x" << item.count;
        if (item.healAmount > 0) {
            std::cout << " (恢复" << item.healAmount << "HP)";
        }
        if (item.healPercent > 0) {
            std::cout << " (恢复" << item.healPercent << "%)";
        }
        if (item.expAmount > 0) {
            std::cout << " (+" << item.expAmount << "经验)";
        }
        if (item.isRevive) {
            std::cout << " (复活)";
        }
        std::cout << std::endl;
    }
}

void TeamView::handleCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;

    action = toLower(action);

    if (action == "q" || action == "quit") {
        state.viewing = false;
        std::cout << "返回游戏。" << std::endl;
        return;
    }

    if (action == "page") {
        int pageNum;
        iss >> pageNum;
        if (pageNum < 1) {
            std::cout << "页码必须大于 0!" << std::endl;
            return;
        }
        state.storagePage = pageNum - 1;
        int totalPages = (player.storage.size() + state.pokemonsPerPage - 1) / state.pokemonsPerPage;
        if (state.storagePage >= totalPages) {
            state.storagePage = totalPages - 1;
        }
        std::cout << "翻到第 " << pageNum << " 页。" << std::endl;
        return;
    }

    if (action == "swap") {
        int idx1, idx2;
        iss >> idx1 >> idx2;
        if (idx1 < 1 || idx1 > (int)player.team.size() ||
            idx2 < 1 || idx2 > (int)player.team.size()) {
            std::cout << "无效的精灵编号!" << std::endl;
            return;
        }
        if (player.swapTeamPokemon(idx1 - 1, idx2 - 1)) {
            std::cout << "已交换精灵位置。" << std::endl;
        } else {
            std::cout << "交换失败!" << std::endl;
        }
        return;
    }

    if (action == "use") {
        int itemIdx, pokeIdx;
        iss >> itemIdx >> pokeIdx;
        if (itemIdx < 1 || itemIdx > (int)player.items.size()) {
            std::cout << "无效的物品编号!" << std::endl;
            return;
        }
        if (pokeIdx < 1 || pokeIdx > (int)player.team.size()) {
            std::cout << "无效的精灵编号!" << std::endl;
            return;
        }
        if (player.items[itemIdx - 1].count <= 0) {
            std::cout << "物品数量不足!" << std::endl;
            return;
        }
        if (player.useItemOnPokemon(itemIdx - 1, pokeIdx - 1, true)) {
            std::cout << "使用了 " << player.items[itemIdx - 1].name << "!" << std::endl;
        } else {
            std::cout << "使用失败!" << std::endl;
        }
        return;
    }

    if (action == "stor") {
        int pokeIdx;
        iss >> pokeIdx;
        if (pokeIdx < 1 || pokeIdx > (int)player.team.size()) {
            std::cout << "无效的背包精灵编号!" << std::endl;
            return;
        }
        if (player.movePokemonToStorage(pokeIdx - 1)) {
            std::cout << "已将精灵放入仓库。" << std::endl;
        } else {
            std::cout << "无法放入仓库 (队伍至少需要保留一只精灵)!" << std::endl;
        }
        return;
    }

    if (action == "take") {
        int storageIdx;
        iss >> storageIdx;
        int start = state.storagePage * state.pokemonsPerPage;
        int actualIdx = start + storageIdx - 1;

        if (storageIdx < 1 || actualIdx >= (int)player.storage.size()) {
            std::cout << "无效的仓库精灵编号!" << std::endl;
            return;
        }
        if (player.team.size() >= MAX_TEAM_SIZE) {
            std::cout << "背包已满 (6 只)，无法放入!" << std::endl;
            return;
        }
        if (player.movePokemonFromStorage(actualIdx)) {
            std::cout << "已将精灵放入背包。" << std::endl;
        } else {
            std::cout << "放入失败!" << std::endl;
        }
        return;
    }

    std::cout << "未知命令，输入 'q' 返回。" << std::endl;
}
