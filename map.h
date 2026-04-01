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
    bool tryMove(char direction);
    
    // 获取可移动方向
    std::vector<char> getAvailableDirections() const;
    
    // 遭遇检查
    bool shouldEncounter() const;
    const std::vector<std::string>& getWildPokemons() const;
    
    // 方向名称转换
    static std::string getDirectionName(char dir);
    static std::string getDirectionDisplayName(char dir);
};

#endif // MAP_H
