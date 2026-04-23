#include "pch.h"
#include "CppUnitTest.h"
#include "G:/team_Software_Engineering_project-main/team_Software_Engineering_project-main/battle.h"
#include "G:/team_Software_Engineering_project-main/team_Software_Engineering_project-main/player.h"
#include "G:/team_Software_Engineering_project-main/team_Software_Engineering_project-main/map.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PokemonTestsNative {
    TEST_CLASS(BattleTests) {
public:
    TEST_METHOD(TestDamageCalculation) {
        srand(42);  // 固定随机种子，使伤害浮动可预测
        Player dummy;
        Battle battle(dummy);
        Pokemon attacker("小火龙", 5);
        attacker.stats.attack = 20;
        Move flamethrower = { "喷射火焰", Type::Fire, 90, 100, true };
        Pokemon defender("妙蛙种子", 5);
        defender.stats.defense = 10;

        int damage = battle.calculateDamage(attacker, defender, flamethrower);
        // 实际伤害应在 137~162 之间（基础54 * STAB1.5 * 克制2.0 * 浮动0.85~1.0）
        Assert::IsTrue(damage >= 137 && damage <= 162);
    }

    TEST_METHOD(TestMapMovement) {
        Map map;
        map.init();
        Assert::IsTrue(map.tryMove('n'));
        Assert::AreEqual(1, map.getCurrentLocationIndex());
        Assert::IsFalse(map.tryMove('w'));
    }

    TEST_METHOD(TestTypeEffectiveness) {
        Assert::AreEqual(2.0f, getTypeEffectiveness(Type::Fire, Type::Grass));
        Assert::AreEqual(0.0f, getTypeEffectiveness(Type::Electric, Type::Ground));
    }
    };
}