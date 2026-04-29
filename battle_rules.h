#pragma once

#include "pokemon.h"

// Pure battle-rule helpers extracted for deterministic unit testing.
// These functions do not perform I/O and do not depend on Battle state.

int CalculateDamage(const Pokemon& attacker,
                    const Pokemon& defender,
                    const Move& move,
                    float randomFactor);

int CalculateEscapeChance(int playerSpeed, int enemySpeed);

float CalculateCatchProbability(int hp,
                                int maxHp,
                                int level,
                                float ballRate,
                                bool isMasterBall);
