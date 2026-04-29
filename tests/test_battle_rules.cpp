#include "doctest.h"

#include "../battle_rules.h"
#include "../config.h"

namespace {
Pokemon MakePokemon(Type type,
                    int attack,
                    int defense,
                    int speed = 10,
                    int maxHp = 40,
                    int hp = 40) {
    Pokemon pokemon;
    pokemon.name = "RuleMon";
    pokemon.species = "RuleMon";
    pokemon.type = type;
    pokemon.level = 5;
    pokemon.stats = {hp, maxHp, attack, defense, speed};
    pokemon.exp = 0;
    pokemon.maxExp = 100;
    return pokemon;
}

Move MakeMove(Type type, int power) {
    Move move = {"RuleMove", type, power, 100, false};
    return move;
}
}

TEST_CASE("CalculateEscapeChance returns base chance when both sides have equal speed") {
    CHECK(CalculateEscapeChance(20, 20) == ESCAPE_BASE_CHANCE);
}

TEST_CASE("CalculateEscapeChance is clamped to minimum for very slow player pokemon") {
    CHECK(CalculateEscapeChance(5, 40) == ESCAPE_MIN_CHANCE);
}

TEST_CASE("CalculateEscapeChance is clamped to maximum for very fast player pokemon") {
    CHECK(CalculateEscapeChance(80, 10) == ESCAPE_MAX_CHANCE);
}

TEST_CASE("CalculateCatchProbability gives master ball its fixed special rate") {
    const float probability = CalculateCatchProbability(40, 40, 99, 1.0f, true);
    CHECK(probability == doctest::Approx(MASTERBALL_CATCH_RATE));
}

TEST_CASE("CalculateCatchProbability respects lower bound when target is healthy") {
    const float probability = CalculateCatchProbability(40, 40, 50, 1.0f, false);
    CHECK(probability == doctest::Approx(CATCH_MIN_PROBABILITY));
}

TEST_CASE("CalculateCatchProbability respects upper bound when target is weak and easy to catch") {
    const float probability = CalculateCatchProbability(0, 40, 1, 2.0f, false);
    CHECK(probability == doctest::Approx(CATCH_MAX_PROBABILITY));
}

TEST_CASE("CalculateDamage preserves minimum damage for extremely weak attacks") {
    const Pokemon attacker = MakePokemon(Type::Water, 1, 10);
    const Pokemon defender = MakePokemon(Type::Water, 10, 999);
    const Move move = MakeMove(Type::Normal, 1);

    CHECK(CalculateDamage(attacker, defender, move, 0.85f) == MIN_DAMAGE);
}

TEST_CASE("CalculateDamage gives stab moves more damage than same power off-type moves") {
    const Pokemon attacker = MakePokemon(Type::Fire, 20, 10);
    const Pokemon defender = MakePokemon(Type::Normal, 10, 10);

    const int stabDamage = CalculateDamage(attacker, defender, MakeMove(Type::Fire, 40), 1.0f);
    const int offTypeDamage = CalculateDamage(attacker, defender, MakeMove(Type::Water, 40), 1.0f);

    CHECK(stabDamage > offTypeDamage);
}

TEST_CASE("CalculateDamage rewards super effective matchups over resisted matchups") {
    const Pokemon attacker = MakePokemon(Type::Fire, 20, 10);
    const Move fireMove = MakeMove(Type::Fire, 40);

    const int versusGrass = CalculateDamage(attacker, MakePokemon(Type::Grass, 10, 10), fireMove, 1.0f);
    const int versusWater = CalculateDamage(attacker, MakePokemon(Type::Water, 10, 10), fireMove, 1.0f);

    CHECK(versusGrass > versusWater);
}

TEST_CASE("CalculateDamage returns zero for immune targets") {
    const Pokemon attacker = MakePokemon(Type::Electric, 20, 10);
    const Pokemon defender = MakePokemon(Type::Ground, 10, 10);
    const Move electricMove = MakeMove(Type::Electric, 40);

    CHECK(CalculateDamage(attacker, defender, electricMove, 1.0f) == 0);
}
