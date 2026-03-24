#include "pokemon.h"

// 属性名称
std::string getTypeName(Type type) {
    switch (type) {
        case Type::Normal:   return "一般";
        case Type::Fire:     return "火";
        case Type::Water:    return "水";
        case Type::Grass:    return "草";
        case Type::Electric: return "电";
        case Type::Ice:      return "冰";
        case Type::Fighting: return "格斗";
        case Type::Ground:   return "地面";
        case Type::Flying:   return "飞行";
        default:             return "未知";
    }
}

// 属性克制关系表
float getTypeEffectiveness(Type attackType, Type defendType) {
    // 克制关系：攻击属性 vs 防御属性
    switch (attackType) {
        case Type::Normal:
            return 1.0f;
        
        case Type::Fire:
            if (defendType == Type::Grass) return 2.0f;
            if (defendType == Type::Ice) return 2.0f;
            if (defendType == Type::Water) return 0.5f;
            if (defendType == Type::Fire) return 0.5f;
            return 1.0f;
        
        case Type::Water:
            if (defendType == Type::Fire) return 2.0f;
            if (defendType == Type::Ground) return 2.0f;
            if (defendType == Type::Grass) return 0.5f;
            if (defendType == Type::Water) return 0.5f;
            return 1.0f;
        
        case Type::Grass:
            if (defendType == Type::Water) return 2.0f;
            if (defendType == Type::Ground) return 2.0f;
            if (defendType == Type::Fire) return 0.5f;
            if (defendType == Type::Grass) return 0.5f;
            return 1.0f;
        
        case Type::Electric:
            if (defendType == Type::Water) return 2.0f;
            if (defendType == Type::Flying) return 2.0f;
            if (defendType == Type::Grass) return 0.5f;
            if (defendType == Type::Electric) return 0.5f;
            if (defendType == Type::Ground) return 0.0f;  // 无效
            return 1.0f;
        
        case Type::Ice:
            if (defendType == Type::Grass) return 2.0f;
            if (defendType == Type::Ground) return 2.0f;
            if (defendType == Type::Flying) return 2.0f;
            if (defendType == Type::Fire) return 0.5f;
            if (defendType == Type::Ice) return 0.5f;
            return 1.0f;
        
        case Type::Fighting:
            if (defendType == Type::Normal) return 2.0f;
            if (defendType == Type::Ice) return 2.0f;
            if (defendType == Type::Flying) return 0.5f;
            return 1.0f;
        
        case Type::Ground:
            if (defendType == Type::Fire) return 2.0f;
            if (defendType == Type::Electric) return 2.0f;
            if (defendType == Type::Grass) return 0.5f;
            if (defendType == Type::Flying) return 0.0f;  // 无效
            return 1.0f;
        
        case Type::Flying:
            if (defendType == Type::Grass) return 2.0f;
            if (defendType == Type::Fighting) return 2.0f;
            if (defendType == Type::Electric) return 0.5f;
            if (defendType == Type::Ice) return 0.5f;
            return 1.0f;
        
        default:
            return 1.0f;
    }
}

Pokemon::Pokemon() : level(1), exp(0), maxExp(100), type(Type::Normal) {}

Pokemon::Pokemon(const std::string& sp, int lvl)
    : species(sp), level(lvl), exp(0), maxExp(100) {
    name = sp;
    initStats();
    initMoves();
}

void Pokemon::initStats() {
    if (species == "小火龙") {
        type = Type::Fire;
        stats = {30, 30, 10, 8, 10};
    } else if (species == "杰尼龟") {
        type = Type::Water;
        stats = {32, 32, 9, 10, 7};
    } else if (species == "妙蛙种子") {
        type = Type::Grass;
        stats = {31, 31, 10, 9, 8};
    } else if (species == "小拉达") {
        type = Type::Normal;
        stats = {25, 25, 9, 7, 11};
    } else if (species == "波波") {
        type = Type::Flying;
        stats = {26, 26, 8, 8, 10};
    } else if (species == "绿毛虫") {
        type = Type::Grass;
        stats = {24, 24, 7, 7, 7};
    } else if (species == "小雀蜂") {
        type = Type::Flying;
        stats = {23, 23, 8, 7, 9};
    } else if (species == "独角虫") {
        type = Type::Grass;
        stats = {24, 24, 7, 7, 7};
    } else if (species == "派拉斯") {
        type = Type::Grass;
        stats = {27, 27, 9, 8, 6};
    } else if (species == "皮卡丘") {
        type = Type::Electric;
        stats = {26, 26, 10, 7, 12};
    } else if (species == "可达鸭") {
        type = Type::Water;
        stats = {28, 28, 9, 9, 7};
    } else if (species == "小海狮") {
        type = Type::Ice;
        stats = {27, 27, 8, 8, 8};
    } else if (species == "角金鱼") {
        type = Type::Water;
        stats = {26, 26, 9, 8, 9};
    } else if (species == "小拳石") {
        type = Type::Ground;
        stats = {30, 30, 10, 11, 5};
    } else if (species == "大岩蛇") {
        type = Type::Ground;
        stats = {35, 35, 9, 13, 6};
    } else if (species == "超音蝠") {
        type = Type::Flying;
        stats = {25, 25, 8, 7, 11};
    } else if (species == "腕力") {
        type = Type::Fighting;
        stats = {30, 30, 11, 8, 6};
    } else if (species == "地鼠") {
        type = Type::Ground;
        stats = {26, 26, 9, 8, 10};
    } else {
        type = Type::Normal;
        stats = {25, 25, 9, 9, 9};
    }
    stats.maxHp += level * 3;
    stats.hp = stats.maxHp;
    stats.attack += level;
    stats.defense += level;
    stats.speed += level;
}

void Pokemon::initMoves() {
    moves.clear();
    
    // 共同技能：所有宝可梦都会的技能
    Move tackle = {"撞击", Type::Normal, 40, 100, false};
    Move growl = {"叫声", Type::Normal, 0, 100, false};  // 0 威力为变化技能
    moves.push_back(tackle);
    moves.push_back(growl);
    
    // 根据属性添加第二个共同技能
    if (type == Type::Fire) {
        Move ember = {"火花", Type::Fire, 40, 100, false};
        moves.push_back(ember);
    } else if (type == Type::Water) {
        Move watergun = {"水枪", Type::Water, 40, 100, false};
        moves.push_back(watergun);
    } else if (type == Type::Grass) {
        Move vinewhip = {"藤鞭", Type::Grass, 45, 100, false};
        moves.push_back(vinewhip);
    } else if (type == Type::Electric) {
        Move thundershock = {"电击", Type::Electric, 40, 100, false};
        moves.push_back(thundershock);
    } else if (type == Type::Ice) {
        Move powdersnow = {"粉末雪", Type::Ice, 40, 100, false};
        moves.push_back(powdersnow);
    } else if (type == Type::Fighting) {
        Move lowkick = {"踢倒", Type::Fighting, 50, 100, false};
        moves.push_back(lowkick);
    } else if (type == Type::Ground) {
        Move mudslap = {"掷泥", Type::Ground, 20, 100, false};
        moves.push_back(mudslap);
    } else if (type == Type::Flying) {
        Move gust = {"起风", Type::Flying, 40, 100, false};
        moves.push_back(gust);
    }
    
    // 专属技能（根据物种）
    if (species == "小火龙") {
        Move flamethrower = {"喷射火焰", Type::Fire, 90, 100, true};
        moves.push_back(flamethrower);
    } else if (species == "杰尼龟") {
        Move hydropump = {"水炮", Type::Water, 110, 80, true};
        moves.push_back(hydropump);
    } else if (species == "妙蛙种子") {
        Move solarbeam = {"日光束", Type::Grass, 120, 100, true};
        moves.push_back(solarbeam);
    } else if (species == "皮卡丘") {
        Move thunderbolt = {"十万伏特", Type::Electric, 90, 100, true};
        moves.push_back(thunderbolt);
    } else if (species == "波波") {
        Move whirlwind = {"暴风", Type::Flying, 110, 70, true};
        moves.push_back(whirlwind);
    } else if (species == "腕力") {
        Move crosschop = {"十字劈", Type::Fighting, 100, 80, true};
        moves.push_back(crosschop);
    }
}

bool Pokemon::isFainted() const {
    return stats.hp <= 0;
}
