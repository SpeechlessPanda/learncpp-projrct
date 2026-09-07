#include <array>
#include <cctype>
#include <iostream>
#include <string>
#include <string_view>

// 注意：Random.h 通常是外部依赖（如 LearnCpp 教程提供的随机数库），
// 请确保该文件存在于你的项目包含路径中。
#include "Random.h"

/**
 * @brief 药水的类型、大小及效果定义。
 */
class Potion
{
public:
    /// @brief 药水类型枚举
    enum Type
    {
        health,
        strength,
        poison,
        max_type ///< 用于随机生成和数组边界
    };

    /// @brief 药水大小枚举
    enum Size
    {
        small,
        medium,
        large,
        max_size ///< 用于随机生成和数组边界
    };

private:
    Type m_type{};
    Size m_size{};

public:
    /**
     * @brief 构造一个药水。
     * @param type 药水类型。
     * @param size 药水大小。
     */
    Potion(Type type, Size size)
        : m_type{type}, m_size{size}
    {
    }

    Type getType() const { return m_type; }
    Size getSize() const { return m_size; }

    /**
     * @brief 获取药水类型的字符串名称。
     * @param type 药水类型。
     * @return std::string_view 类型名称。
     */
    static std::string_view getPotionTypeName(Type type)
    {
        static constexpr std::array<std::string_view, max_type> names{
            "Health", "Strength", "Poison"};
        return names[static_cast<std::size_t>(type)];
    }

    /**
     * @brief 获取药水大小的字符串名称。
     * @param size 药水大小。
     * @return std::string_view 大小名称。
     */
    static std::string_view getPotionSizeName(Size size)
    {
        static constexpr std::array<std::string_view, max_size> names{
            "Small", "Medium", "Large"};
        return names[static_cast<std::size_t>(size)];
    }

    /**
     * @brief 获取药水的完整描述名称（如 "Small potion of Health"）。
     * @return std::string 完整名称。
     */
    std::string getName() const
    {
        // 优化：使用 std::string 拼接替代 std::stringstream，减少头文件依赖和运行时开销
        return std::string{getPotionSizeName(getSize())} + " potion of " + std::string{getPotionTypeName(getType())};
    }

    /**
     * @brief 随机生成一个药水。
     * @return Potion 随机生成的药水对象。
     */
    static Potion getRandomPotion()
    {
        return Potion{
            static_cast<Type>(Random::get(0, max_type - 1)),
            static_cast<Size>(Random::get(0, max_size - 1))};
    }
};

/**
 * @brief 游戏中的生物基类（包含玩家和怪物）。
 */
class Creature
{
protected:
    std::string m_name;
    char m_symbol{};
    int m_health{};
    int m_damage{};
    int m_gold{};

public:
    /**
     * @brief 构造一个生物。
     */
    Creature(std::string_view name, char symbol, int health, int damage, int gold)
        : m_name{name}, m_symbol{symbol}, m_health{health}, m_damage{damage}, m_gold{gold}
    {
    }

    char getSymbol() const { return m_symbol; }
    const std::string &getName() const { return m_name; }
    bool isDead() const { return m_health <= 0; }
    int getGold() const { return m_gold; }
    void addGold(int gold) { m_gold += gold; }
    void reduceHealth(int health) { m_health -= health; }
    int getHealth() const { return m_health; }
    int getDamage() const { return m_damage; }
};

/**
 * @brief 玩家类，继承自 Creature，拥有等级和药水效果。
 */
class Player : public Creature
{
    int m_level{1};

public:
    /**
     * @brief 构造玩家，初始符号为 '@'，血量 10，伤害 1，金币 0。
     * @param name 玩家名称。
     */
    // 优化：参数改为 std::string_view，与基类保持一致，避免不必要的 std::string 构造
    Player(std::string_view name)
        : Creature{name, '@', 10, 1, 0}
    {
    }

    /// @brief 玩家升级，伤害 +1。
    void levelUp()
    {
        ++m_level;
        ++m_damage;
    }

    /**
     * @brief 喝下药水，应用药水效果。
     * @param potion 要喝下的药水。
     */
    void drinkPotion(const Potion &potion)
    {
        switch (potion.getType())
        {
        case Potion::health:
            // 只有血瓶的效果受大小影响，大血瓶恢复 5 点，其他恢复 2 点
            m_health += ((potion.getSize() == Potion::large) ? 5 : 2);
            break;
        case Potion::strength:
            ++m_damage;
            break;
        case Potion::poison:
            reduceHealth(1);
            break;
        case Potion::max_type:
            // 处理 max_type 以消除编译器警告。不使用 default 是为了让编译器
            // 在未来新增药水类型时，能警告我们遗漏了实现。
            break;
        }
    }

    int getLevel() const { return m_level; }
    bool hasWon() const { return m_level >= 20; }
};

/**
 * @brief 怪物类，继承自 Creature，通过静态数组预设怪物数据。
 */
class Monster : public Creature
{
public:
    enum Type
    {
        dragon,
        orc,
        slime,
        max_types
    };

private:
    // 优化：使用 std::array 替代 C 风格数组，类型更安全，且能直接使用 .size() 方法
    inline static std::array<Creature, max_types> monsterData{
        Creature{"dragon", 'D', 20, 4, 100},
        Creature{"orc", 'o', 4, 2, 25},
        Creature{"slime", 's', 1, 1, 10}};

    // 编译期断言：确保数组大小与枚举数量一致
    static_assert(monsterData.size() == max_types);

public:
    /**
     * @brief 根据类型构造怪物。
     * @param type 怪物类型。
     */
    Monster(Type type)
        : Creature{monsterData[static_cast<std::size_t>(type)]}
    {
    }

    /**
     * @brief 随机生成一个怪物。
     * @return Monster 随机生成的怪物对象。
     */
    static Monster getRandomMonster()
    {
        int num{Random::get(0, max_types - 1)};
        return Monster{static_cast<Type>(num)};
    }
};

/**
 * @brief 怪物被击杀后的奖励结算逻辑。
 */
void onMonsterKilled(Player &player, const Monster &monster)
{
    std::cout << "You killed the " << monster.getName() << ".\n";
    player.levelUp();
    std::cout << "You are now level " << player.getLevel() << ".\n";
    std::cout << "You found " << monster.getGold() << " gold.\n";
    player.addGold(monster.getGold());

    // 30% 概率掉落药水
    constexpr int potionChance{30};
    if (Random::get(1, 100) <= potionChance)
    {
        auto potion{Potion::getRandomPotion()};

        std::cout << "You found a mythical potion! Do you want to drink it? [y/n]: ";
        char choice{};
        std::cin >> choice;

        // 优化：使用 std::tolower 简化大小写判断，并转换为 unsigned char 防止 UB
        if (std::tolower(static_cast<unsigned char>(choice)) == 'y')
        {
            player.drinkPotion(potion);
            std::cout << "You drank a " << potion.getName() << ".\n";
        }
    }
}

/**
 * @brief 玩家攻击怪物的逻辑。
 */
void attackMonster(Player &player, Monster &monster)
{
    if (player.isDead())
        return;

    std::cout << "You hit the " << monster.getName() << " for " << player.getDamage() << " damage.\n";
    monster.reduceHealth(player.getDamage());

    if (monster.isDead())
    {
        onMonsterKilled(player, monster);
    }
}

/**
 * @brief 怪物攻击玩家的逻辑。
 */
void attackPlayer(const Monster &monster, Player &player)
{
    if (monster.isDead())
        return;

    player.reduceHealth(monster.getDamage());
    std::cout << "The " << monster.getName() << " hit you for " << monster.getDamage() << " damage.\n";
}

/**
 * @brief 处理玩家与随机生成的怪物之间的完整战斗流程。
 */
void fightMonster(Player &player)
{
    Monster monster{Monster::getRandomMonster()};
    std::cout << "You have encountered a " << monster.getName() << " (" << monster.getSymbol() << ").\n";

    while (!monster.isDead() && !player.isDead())
    {
        std::cout << "(R)un or (F)ight: ";
        char input{};
        std::cin >> input;

        char lowerInput = std::tolower(static_cast<unsigned char>(input));

        if (lowerInput == 'r')
        {
            // 50% 概率逃跑成功
            if (Random::get(1, 2) == 1)
            {
                std::cout << "You successfully fled.\n";
                return;
            }
            else
            {
                std::cout << "You failed to flee.\n";
                attackPlayer(monster, player);
                continue;
            }
        }

        if (lowerInput == 'f')
        {
            attackMonster(player, monster);
            attackPlayer(monster, player);
        }
    }
}

/**
 * @brief 程序入口，运行游戏主循环。
 */
int main()
{
    std::cout << "Enter your name: ";
    std::string playerName;
    std::cin >> playerName;

    Player player{playerName};
    std::cout << "Welcome, " << player.getName() << '\n';

    // 游戏主循环：玩家未死且未达到 20 级时继续战斗
    while (!player.isDead() && !player.hasWon())
        fightMonster(player);

    // 结算游戏结果
    if (player.isDead())
    {
        std::cout << "You died at level " << player.getLevel() << " and with " << player.getGold() << " gold.\n";
        std::cout << "Too bad you can't take it with you!\n";
    }
    else
    {
        std::cout << "You won the game with " << player.getGold() << " gold!\n";
    }

    return 0;
}