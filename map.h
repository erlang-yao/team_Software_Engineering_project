#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include <map>

struct Location {
    std::string name;
    std::string description;
    std::map<char, int> connections;  // 方向 -> 相连地图索引
    std::vector<std::string> wildPokemons;  // 可遭遇的野生宝可梦
};

// Map 类职责说明：
// - 管理整个“地图/位置”数据（locations，下标即地图编号）。
// - 维护玩家当前所在地图编号（currentLocation）。
// - 提供地图初始化、当前位置查询、按方向移动、以及野外遭遇判定等能力。
class Map {
private:
    std::vector<Location> locations;
    int currentLocation;

public:
    Map();
    
    void init();
    
    // 获取当前位置信息
    const Location& getCurrentLocation() const;
    int getCurrentLocationIndex() const;
    
    // 移动
    // tryMove() 逻辑说明：
    // - 读取当前地图的 connections（方向 -> 目标地图编号）。
    // - 若 direction 不存在于 connections，则不可移动并返回 false。
    // - 否则将 currentLocation 更新为对应的目标地图编号并返回 true。
    bool tryMove(char direction);
    
    // 获取可移动方向
    std::vector<char> getAvailableDirections() const;
    
    // 遭遇检查
    bool shouldEncounter() const;
    // shouldEncounter() 遭遇判定说明：
    // - 若当前位置 wildPokemons 为空，表示不会遭遇，直接返回 false。
    // - 否则生成 0~99 的随机数，与 ENCOUNTER_CHANCE_PERCENT 比较；
    //   当随机数小于该百分比阈值时，判定为发生一次野外遭遇。
    const std::vector<std::string>& getWildPokemons() const;
    
    // 方向名称转换
    static std::string getDirectionName(char dir);
    static std::string getDirectionDisplayName(char dir);
};

#endif // MAP_H
