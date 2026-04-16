#include "map.h"
#include <cstdlib>
#include <ctime>
#include "config.h"

Map::Map() : currentLocation(0) {}

void Map::init() {
    /*
     * 地图编号对应关系（locations 下标）：
     * 0 - 新手村
     * 1 - 望风坡
     * 2 - 迷雾森林
     * 3 - 湖边
     * 4 - 精灵中心
     * 5 - 山洞入口
     * 6 - 山洞深处
     *
     * 说明：
     * - 每个 Location 的 connections 记录“方向(char) -> 目标地图编号(int)”的邻接关系。
     * - wildPokemons 为空表示该地图不会触发野外遭遇（例如城镇/中心）。
     */

    // 地图 0: 新手村
    Location loc0;
    loc0.name = "新手村";
    loc0.description = "一个宁静的小村庄，训练家们在这里开始他们的旅程";
    loc0.connections['n'] = 1;
    loc0.connections['e'] = 4;
    loc0.wildPokemons = {"小拉达", "波波", "绿毛虫"};
    locations.push_back(loc0);

    // 地图 1: 望风坡
    Location loc1;
    loc1.name = "望风坡";
    loc1.description = "可以俯瞰整个新手村的高坡，微风拂面";
    loc1.connections['s'] = 0;
    loc1.connections['n'] = 2;
    loc1.wildPokemons = {"波波", "小雀蜂", "独角虫"};
    locations.push_back(loc1);

    // 地图 2: 迷雾森林
    Location loc2;
    loc2.name = "迷雾森林";
    loc2.description = "被薄雾笼罩的神秘森林，隐藏着许多野生宝可梦";
    loc2.connections['s'] = 1;
    loc2.connections['e'] = 3;
    loc2.wildPokemons = {"绿毛虫", "铁甲蛹", "派拉斯", "皮卡丘"};
    locations.push_back(loc2);

    // 地图 3: 湖边
    Location loc3;
    loc3.name = "湖边";
    loc3.description = "清澈的湖泊，水面如镜";
    loc3.connections['w'] = 2;
    loc3.connections['n'] = 5;
    loc3.wildPokemons = {"可达鸭", "小海狮", "角金鱼"};
    locations.push_back(loc3);

    // 地图 4: 精灵中心
    Location loc4;
    loc4.name = "精灵中心";
    loc4.description = "训练家们休息和治疗宝可梦的地方";
    loc4.connections['w'] = 0;
    loc4.wildPokemons = {};
    locations.push_back(loc4);

    // 地图 5: 山洞入口
    Location loc5;
    loc5.name = "山洞入口";
    loc5.description = "阴暗的山洞入口，里面传来神秘的气息";
    loc5.connections['s'] = 3;
    loc5.connections['n'] = 6;
    loc5.wildPokemons = {"小拳石", "大岩蛇", "超音蝠"};
    locations.push_back(loc5);

    // 地图 6: 山洞深处
    Location loc6;
    loc6.name = "山洞深处";
    loc6.description = "山洞的最深处，传说中有稀有的宝可梦出没";
    loc6.connections['s'] = 5;
    loc6.wildPokemons = {"大岩蛇", "腕力", "小拳石", "地鼠"};
    locations.push_back(loc6);
}

const Location& Map::getCurrentLocation() const {
    return locations[currentLocation];
}

int Map::getCurrentLocationIndex() const {
    return currentLocation;
}

// tryMove() 的逻辑说明：
// - 在当前位置的 connections 中查找方向键 direction；未找到则表示该方向不可达。
// - 找到时，将 currentLocation 切换为目标地图编号。
bool Map::tryMove(char direction) {
    Location& current = locations[currentLocation];
    
    if (current.connections.find(direction) == current.connections.end()) {
        return false;
    }
    
    currentLocation = current.connections[direction];
    return true;
}

std::vector<char> Map::getAvailableDirections() const {
    std::vector<char> directions;
    const Location& current = locations[currentLocation];
    
    for (const auto& conn : current.connections) {
        directions.push_back(conn.first);
    }
    return directions;
}

// shouldEncounter() 的遭遇判定说明：
// - 若当前位置没有可遭遇的野生宝可梦（wildPokemons 为空），则一定不会遭遇。
// - 否则生成 0~99 的随机数，若其小于 ENCOUNTER_CHANCE_PERCENT 则触发遭遇。
bool Map::shouldEncounter() const {
    const Location& current = locations[currentLocation];
    if (current.wildPokemons.empty()) {
        return false;
    }
    
    int chance = rand() % 100;
    return chance < ENCOUNTER_CHANCE_PERCENT;
}

const std::vector<std::string>& Map::getWildPokemons() const {
    return locations[currentLocation].wildPokemons;
}

std::string Map::getDirectionName(char dir) {
    switch (dir) {
        case 'e': return "东";
        case 'w': return "西";
        case 's': return "南";
        case 'n': return "北";
        default: return "";
    }
}

std::string Map::getDirectionDisplayName(char dir) {
    switch (dir) {
        case 'e': return "东 (e)";
        case 'w': return "西 (w)";
        case 's': return "南 (s)";
        case 'n': return "北 (n)";
        default: return "";
    }
}
