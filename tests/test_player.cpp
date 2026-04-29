#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../player.h"

namespace {
Pokemon MakePokemon(Type type,
                    int hp = 30,
                    int maxHp = 30,
                    int attack = 10,
                    int defense = 10,
                    int speed = 10,
                    int level = 5) {
    Pokemon pokemon;
    pokemon.name = "TestMon";
    pokemon.species = "TestMon";
    pokemon.type = type;
    pokemon.level = level;
    pokemon.stats = {hp, maxHp, attack, defense, speed};
    pokemon.exp = 0;
    pokemon.maxExp = 100;
    return pokemon;
}

int FindHealItemIndex(const Player& player) {
    for (int i = 0; i < static_cast<int>(player.items.size()); ++i) {
        if (!player.items[i].isRevive && player.items[i].healPercent > 0) {
            return i;
        }
    }
    return -1;
}

int FindExpItemIndex(const Player& player) {
    for (int i = 0; i < static_cast<int>(player.items.size()); ++i) {
        if (player.items[i].expAmount > 0) {
            return i;
        }
    }
    return -1;
}

int FindReviveItemIndex(const Player& player) {
    for (int i = 0; i < static_cast<int>(player.items.size()); ++i) {
        if (player.items[i].isRevive) {
            return i;
        }
    }
    return -1;
}
}

TEST_CASE("useItemOnPokemon rejects invalid item index without touching state") {
    Player player;
    player.addPokemon(MakePokemon(Type::Fire, 12, 30));

    const int hpBefore = player.team[0].stats.hp;
    const int itemCountBefore = player.items[0].count;

    CHECK_FALSE(player.useItemOnPokemon(-1, 0, true));
    CHECK_FALSE(player.useItemOnPokemon(static_cast<int>(player.items.size()), 0, true));
    CHECK(player.team[0].stats.hp == hpBefore);
    CHECK(player.items[0].count == itemCountBefore);
}

TEST_CASE("useItemOnPokemon rejects invalid pokemon index for team and storage") {
    Player player;
    player.addPokemon(MakePokemon(Type::Water, 15, 30));
    player.addPokemon(MakePokemon(Type::Grass, 20, 30));
    REQUIRE(player.movePokemonToStorage(1));

    const int healIndex = FindHealItemIndex(player);
    REQUIRE(healIndex >= 0);
    const int itemCountBefore = player.items[healIndex].count;
    const int teamHpBefore = player.team[0].stats.hp;
    const int storageHpBefore = player.storage[0].stats.hp;

    CHECK_FALSE(player.useItemOnPokemon(healIndex, -1, true));
    CHECK_FALSE(player.useItemOnPokemon(healIndex, 99, true));
    CHECK_FALSE(player.useItemOnPokemon(healIndex, -1, false));
    CHECK_FALSE(player.useItemOnPokemon(healIndex, 99, false));
    CHECK(player.items[healIndex].count == itemCountBefore);
    CHECK(player.team[0].stats.hp == teamHpBefore);
    CHECK(player.storage[0].stats.hp == storageHpBefore);
}

TEST_CASE("useItemOnPokemon fails cleanly when item count is zero") {
    Player player;
    player.addPokemon(MakePokemon(Type::Electric, 10, 30));

    const int healIndex = FindHealItemIndex(player);
    REQUIRE(healIndex >= 0);
    player.items[healIndex].count = 0;
    const int hpBefore = player.team[0].stats.hp;

    CHECK_FALSE(player.useItemOnPokemon(healIndex, 0, true));
    CHECK(player.items[healIndex].count == 0);
    CHECK(player.team[0].stats.hp == hpBefore);
}

TEST_CASE("useItemOnPokemon rejects revive medicine and keeps inventory unchanged") {
    Player player;
    player.addPokemon(MakePokemon(Type::Normal, 0, 30));

    const int reviveIndex = FindReviveItemIndex(player);
    REQUIRE(reviveIndex >= 0);
    const int reviveCountBefore = player.items[reviveIndex].count;

    CHECK_FALSE(player.useItemOnPokemon(reviveIndex, 0, true));
    CHECK(player.items[reviveIndex].count == reviveCountBefore);
    CHECK(player.team[0].stats.hp == 0);
}

TEST_CASE("useItemOnPokemon refuses healing a fainted pokemon") {
    Player player;
    player.addPokemon(MakePokemon(Type::Grass, 0, 30));

    const int healIndex = FindHealItemIndex(player);
    REQUIRE(healIndex >= 0);
    const int countBefore = player.items[healIndex].count;

    CHECK_FALSE(player.useItemOnPokemon(healIndex, 0, true));
    CHECK(player.items[healIndex].count == countBefore);
    CHECK(player.team[0].stats.hp == 0);
}

TEST_CASE("useItemOnPokemon heals living pokemon without exceeding max hp") {
    Player player;
    player.addPokemon(MakePokemon(Type::Fire, 25, 30));

    const int healIndex = FindHealItemIndex(player);
    REQUIRE(healIndex >= 0);
    const int countBefore = player.items[healIndex].count;

    REQUIRE(player.useItemOnPokemon(healIndex, 0, true));
    CHECK(player.items[healIndex].count == countBefore - 1);
    CHECK(player.team[0].stats.hp == player.team[0].stats.maxHp);
}

TEST_CASE("useItemOnPokemon can apply exp potion and trigger level up") {
    Player player;
    Pokemon pokemon = MakePokemon(Type::Water, 20, 30, 11, 12, 13, 5);
    pokemon.exp = 90;
    pokemon.maxExp = 100;
    player.addPokemon(pokemon);

    const int expIndex = FindExpItemIndex(player);
    REQUIRE(expIndex >= 0);
    const int countBefore = player.items[expIndex].count;

    REQUIRE(player.useItemOnPokemon(expIndex, 0, true));
    CHECK(player.items[expIndex].count == countBefore - 1);
    CHECK(player.team[0].level == 6);
    CHECK(player.team[0].exp == 40);
    CHECK(player.team[0].maxExp == 150);
    CHECK(player.team[0].stats.maxHp == 33);
    CHECK(player.team[0].stats.hp == 23);
    CHECK(player.team[0].stats.attack == 12);
    CHECK(player.team[0].stats.defense == 13);
    CHECK(player.team[0].stats.speed == 14);
}

TEST_CASE("revivePokemon only succeeds for fainted team pokemon with revive item") {
    Player player;
    player.addPokemon(MakePokemon(Type::Flying, 0, 28));
    player.addPokemon(MakePokemon(Type::Flying, 10, 28));

    const int healIndex = FindHealItemIndex(player);
    const int reviveIndex = FindReviveItemIndex(player);
    REQUIRE(healIndex >= 0);
    REQUIRE(reviveIndex >= 0);

    CHECK_FALSE(player.revivePokemon(healIndex, 0));
    CHECK_FALSE(player.revivePokemon(reviveIndex, 1));

    const int countBefore = player.items[reviveIndex].count;
    REQUIRE(player.revivePokemon(reviveIndex, 0));
    CHECK(player.items[reviveIndex].count == countBefore - 1);
    CHECK(player.team[0].stats.hp == REVIVE_HP_AMOUNT);
}

TEST_CASE("revivePokemon rejects invalid index and zero count without polluting state") {
    Player player;
    player.addPokemon(MakePokemon(Type::Ground, 0, 35));

    const int reviveIndex = FindReviveItemIndex(player);
    REQUIRE(reviveIndex >= 0);

    const int hpBefore = player.team[0].stats.hp;
    const int countBefore = player.items[reviveIndex].count;
    CHECK_FALSE(player.revivePokemon(reviveIndex, -1));
    CHECK_FALSE(player.revivePokemon(reviveIndex, 99));
    CHECK(player.team[0].stats.hp == hpBefore);
    CHECK(player.items[reviveIndex].count == countBefore);

    player.items[reviveIndex].count = 0;
    CHECK_FALSE(player.revivePokemon(reviveIndex, 0));
    CHECK(player.team[0].stats.hp == hpBefore);
    CHECK(player.items[reviveIndex].count == 0);
}

TEST_CASE("movePokemonToStorage rejects removing the last team member") {
    Player player;
    player.addPokemon(MakePokemon(Type::Normal));

    CHECK_FALSE(player.movePokemonToStorage(0));
    CHECK(player.team.size() == 1);
    CHECK(player.storage.empty());
}

TEST_CASE("movePokemonToStorage moves selected pokemon and preserves others") {
    Player player;
    Pokemon first = MakePokemon(Type::Fire);
    first.name = "First";
    Pokemon second = MakePokemon(Type::Water);
    second.name = "Second";
    player.addPokemon(first);
    player.addPokemon(second);

    REQUIRE(player.movePokemonToStorage(1));
    REQUIRE(player.team.size() == 1);
    REQUIRE(player.storage.size() == 1);
    CHECK(player.team[0].name == "First");
    CHECK(player.storage[0].name == "Second");
}

TEST_CASE("movePokemonFromStorage rejects adding when team is full") {
    Player player;
    for (int i = 0; i < 7; ++i) {
        Pokemon pokemon = MakePokemon(Type::Normal);
        pokemon.name = "P" + std::to_string(i);
        player.addPokemon(pokemon);
    }

    REQUIRE(player.team.size() == 6);
    REQUIRE(player.storage.size() == 1);
    const std::string storedName = player.storage[0].name;

    CHECK_FALSE(player.movePokemonFromStorage(0));
    CHECK(player.team.size() == 6);
    CHECK(player.storage.size() == 1);
    CHECK(player.storage[0].name == storedName);
}

TEST_CASE("movePokemonFromStorage rejects invalid index and succeeds for free slot") {
    Player player;
    player.addPokemon(MakePokemon(Type::Normal));
    player.addPokemon(MakePokemon(Type::Fire));
    REQUIRE(player.movePokemonToStorage(1));

    CHECK_FALSE(player.movePokemonFromStorage(-1));
    CHECK_FALSE(player.movePokemonFromStorage(99));
    CHECK(player.team.size() == 1);
    CHECK(player.storage.size() == 1);

    REQUIRE(player.movePokemonFromStorage(0));
    CHECK(player.team.size() == 2);
    CHECK(player.storage.empty());
}

TEST_CASE("swapTeamPokemon swaps valid slots and rejects invalid ones") {
    Player player;
    Pokemon first = MakePokemon(Type::Fire);
    first.name = "Alpha";
    Pokemon second = MakePokemon(Type::Water);
    second.name = "Beta";
    player.addPokemon(first);
    player.addPokemon(second);

    REQUIRE(player.swapTeamPokemon(0, 1));
    CHECK(player.team[0].name == "Beta");
    CHECK(player.team[1].name == "Alpha");

    CHECK_FALSE(player.swapTeamPokemon(-1, 1));
    CHECK_FALSE(player.swapTeamPokemon(0, 99));
    CHECK(player.team[0].name == "Beta");
    CHECK(player.team[1].name == "Alpha");
}

TEST_CASE("addPokemon keeps first six in team and overflows extra members into storage") {
    Player player;

    for (int i = 0; i < 7; ++i) {
        Pokemon pokemon = MakePokemon(Type::Normal);
        pokemon.name = "Member" + std::to_string(i);
        player.addPokemon(pokemon);
    }

    REQUIRE(player.team.size() == 6);
    REQUIRE(player.storage.size() == 1);
    CHECK(player.team[0].name == "Member0");
    CHECK(player.team[5].name == "Member5");
    CHECK(player.storage[0].name == "Member6");
}

TEST_CASE("getCurrentPokemon returns first alive team member and null when all fainted") {
    Player player;
    Pokemon fainted = MakePokemon(Type::Fire, 0, 30);
    fainted.name = "Fainted";
    Pokemon alive = MakePokemon(Type::Water, 12, 30);
    alive.name = "Alive";

    player.addPokemon(fainted);
    player.addPokemon(alive);

    Pokemon* current = player.getCurrentPokemon();
    REQUIRE(current != nullptr);
    CHECK(current->name == "Alive");

    player.team[1].stats.hp = 0;
    CHECK(player.getCurrentPokemon() == nullptr);
}

TEST_CASE("hasAlivePokemon reflects whether the team still has battle-ready members") {
    Player player;
    player.addPokemon(MakePokemon(Type::Grass, 0, 30));
    CHECK_FALSE(player.hasAlivePokemon());

    player.addPokemon(MakePokemon(Type::Electric, 8, 30));
    CHECK(player.hasAlivePokemon());
}

TEST_CASE("getTotalBalls and getHealPotionCount expose current inventory totals") {
    Player player;
    CHECK(player.getTotalBalls() ==
          STARTER_POKEBALL + STARTER_GREATBALL + STARTER_ULTRABALL + STARTER_MASTERBALL);
    CHECK(player.getHealPotionCount() == STARTER_HEAL_POTION);

    const int healIndex = FindHealItemIndex(player);
    REQUIRE(healIndex >= 0);
    player.items[healIndex].count = 2;
    CHECK(player.getHealPotionCount() == 2);
}
