# AGENTS.md

## 项目架构概述

宝可梦风格回合制RPG游戏，控制台交互。架构分为：游戏主控、地图探索、回合制战斗、玩家/宝可梦数据、物品系统、队伍管理。

## 目录结构

| 文件 | 职责 |
|------|------|
| `main.cpp` | 入口，启动 Game |
| `game.h/.cpp` | 游戏主循环，命令解析（移动/队伍/状态），遭遇触发 |
| `map.h/.cpp` | 地图节点（连接、野生宝可梦），移动，遭遇概率 |
| `battle.h/.cpp` | 战斗状态机，回合逻辑，伤害计算，捕捉/逃跑/切换/治疗 |
| `player.h/.cpp` | 玩家队伍、背包（精灵球、物品）、仓库、物品使用 |
| `pokemon.h/.cpp` | 宝可梦属性、技能、属性克制表、精灵球信息 |
| `team_view.h/.cpp` | 队伍/背包/仓库的查看与操作界面 |
| `config.h` | 全局常量（伤害系数、捕捉概率、初始资源等） |
| `utils.h` | 工具函数（字符串转整数、转小写、分割） |

## 核心模块职责

- **Game**：`run()` 主循环，处理移动命令，调用 `encounterBattle()` 触发战斗。
- **Map**：`Location` 结构体定义地图格子，`tryMove()` 改变当前位置，`shouldEncounter()` 按概率返回是否遇敌。
- **Battle**：`start()` 初始化野生宝可梦，`run()` 回合循环（玩家菜单 → 敌方AI），内部处理技能、捕捉、逃跑、切换、治疗、经验升级。
- **Player**：持有 `team`（出战）、`storage`（仓库）、`pokeballs[4]`（精灵球数量）、`items`（药品）。提供队伍管理、物品使用接口。
- **Pokemon**：属性 `Type`，技能 `Move`，`getTypeEffectiveness()` 返回克制倍率。`isFainted()` 检查 HP。
- **TeamView**：独立界面，允许交换队伍顺序、使用物品、仓库/队伍转移、翻页。
- **config.h**：所有魔法数字集中定义（如 `DAMAGE_BASE_MULTIPLIER`，`ESCAPE_BASE_CHANCE`）。

## 编码规范

- **命名**：类/结构体 `PascalCase`，函数/变量 `camelCase`，常量 `UPPER_SNAKE_CASE`，成员变量加 `_` 后缀（如 `state.inBattle` 除外）。
- **头文件**：使用 `#pragma once` 或标准守卫 `#ifndef ...`。
- **类型安全**：枚举类 `enum class` 用于属性类型、精灵球类型。
- **常量**：所有配置值放在 `config.h`，使用 `constexpr`。
- **输入处理**：使用 `std::getline` 读取整行，避免 `std::cin >>` 残留换行符；用 `tryParseInt()` 安全转换。
- **随机数**：仅使用 `rand() % N`，在 `Game::run()` 中已 `srand(time(nullptr))`，无需额外初始化。
- **战斗交互**：菜单文本、提示信息直接输出到 `std::cout`，无需封装日志系统。
- **错误处理**：非法输入时给出提示，允许重试或返回上级菜单，不要崩溃。

## 禁止操作

- 禁止在头文件中 `using namespace std;`
- 禁止使用全局变量（除 `main` 局部对象）
- 禁止修改 `config.h` 中的常量值（除非添加新配置）
- 禁止在 `Battle`、`Player` 核心逻辑中添加 `std::cin` 之外的阻塞输入（保持可测试性）
- 禁止绕过 `Player` 的公共接口直接修改 `team`、`pokeballs` 或 `items`
- 禁止在技能伤害计算中硬编码属性克制——必须调用 `getTypeEffectiveness()`
- 禁止在 `Game::processCommand` 之外直接调用 `battle->run()`，必须经由 `encounterBattle` 触发
- 禁止在 `TeamView` 之外使用队伍管理功能（如 `swapTeamPokemon`、`movePokemonToStorage`）——应通过 `TeamView` 界面操作