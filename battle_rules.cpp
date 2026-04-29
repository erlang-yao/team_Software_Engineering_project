#include "battle_rules.h"

#include "config.h"

namespace {
template <typename T>
T ClampValue(T value, T minValue, T maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}
}

int CalculateDamage(const Pokemon& attacker,
                    const Pokemon& defender,
                    const Move& move,
                    float randomFactor) {
    const float effectiveness = getTypeEffectiveness(move.type, defender.type);
    if (effectiveness <= 0.0f) {
        return 0;
    }

    const float stab = (move.type == attacker.type) ? STAB_MULTIPLIER : 1.0f;
    const float clampedRandom = ClampValue(randomFactor, MIN_DAMAGE_MULTIPLIER, MAX_DAMAGE_MULTIPLIER);
    const float baseDamage =
        static_cast<float>(move.power) * attacker.stats.attack / defender.stats.defense * DAMAGE_BASE_MULTIPLIER;

    int damage = static_cast<int>(baseDamage * stab * effectiveness * clampedRandom);
    if (damage < MIN_DAMAGE) {
        damage = MIN_DAMAGE;
    }
    return damage;
}

int CalculateEscapeChance(int playerSpeed, int enemySpeed) {
    const int rawChance = ESCAPE_BASE_CHANCE + (playerSpeed - enemySpeed) * ESCAPE_SPEED_MULTIPLIER;
    return ClampValue(rawChance, ESCAPE_MIN_CHANCE, ESCAPE_MAX_CHANCE);
}

float CalculateCatchProbability(int hp,
                                int maxHp,
                                int level,
                                float ballRate,
                                bool isMasterBall) {
    if (isMasterBall) {
        return MASTERBALL_CATCH_RATE;
    }

    if (maxHp <= 0) {
        return CATCH_MIN_PROBABILITY;
    }

    const float hpRatio = static_cast<float>(hp) / maxHp;
    const float baseRate = (1.0f - hpRatio) * 100.0f;
    const float levelFactor = 1.0f / (1.0f + level * CATCH_LEVEL_FACTOR_BASE);
    const float rawProbability = baseRate * levelFactor * ballRate;
    return ClampValue(rawProbability, CATCH_MIN_PROBABILITY, CATCH_MAX_PROBABILITY);
}
