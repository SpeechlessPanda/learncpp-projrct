#include <array>
#include <iostream>
#include <iterator> // for std::size
#include <limits>   // for std::numeric_limits
#include <string>
#include <string_view>

// 注意：Random.h 通常是外部依赖（如 LearnCpp 教程提供的随机数库），
// 请确保该文件存在于你的项目包含路径中。
#include "Random.h"

/**
 * @brief 药水相关的枚举、名称和价格配置。
 */
namespace Potion
{
    /**
     * @brief 药水类型枚举。
     */
    enum Type
    {
        healing,
        mana,
        speed,
        invisibility,
        max_potions // 用于表示药水总数，并作为数组大小的边界
    };

    /// @brief 包含所有有效药水类型的数组，方便统一遍历
    constexpr std::array<Type, max_potions> types{healing, mana, speed, invisibility};

    /// @brief 药水的名称映射表
    constexpr std::array<std::string_view, max_potions> name{"healing", "mana", "speed", "invisibility"};

    /// @brief 药水的价格映射表
    constexpr std::array<int, max_potions> cost{20, 30, 12, 50};

    // 编译期断言：确保数组的实际元素数量与枚举定义的总数严格一致，防止漏写或多写
    static_assert(std::size(types) == max_potions);
    static_assert(std::size(cost) == max_potions);
    static_assert(std::size(name) == max_potions);
}

/**
 * @brief 玩家类，管理玩家的名称、金币和药水库存。
 */
class Player
{
private:
    static constexpr int s_minStartingGold{80};
    static constexpr int s_maxStartingGold{120};

    std::string m_name{};
    int m_gold{};
    std::array<int, Potion::max_potions> m_inventory{};

public:
    /**
     * @brief 构造函数，初始化玩家名称并随机分配初始金币。
     *
     * @param name 玩家名称。
     */
    explicit Player(std::string_view name) : m_name{name},
                                             m_gold{Random::get(s_minStartingGold, s_maxStartingGold)}
    {
    }

    /**
     * @brief 尝试购买指定类型的药水。
     *
     * @param type 要购买的药水类型。
     * @return bool 如果金币足够并购买成功返回 true，金币不足返回 false。
     */
    bool buy(Potion::Type type)
    {
        if (m_gold < Potion::cost[type])
            return false;

        m_gold -= Potion::cost[type];
        ++m_inventory[type];
        return true;
    }

    /// @brief 获取当前剩余金币数量。
    int gold() const { return m_gold; }

    /// @brief 获取指定药水的当前库存数量。
    int inventory(Potion::Type p) const { return m_inventory[p]; }
};

/**
 * @brief 清空输入缓冲区中当前行的剩余字符，防止影响后续输入。
 */
void ignoreLine()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * @brief 将数字字符（'0'-'9'）转换为对应的整数值。
 *
 * @param c 数字字符。
 * @return int 对应的整数。
 */
int charNumToInt(char c)
{
    return c - '0';
}

/**
 * @brief 提示用户输入并获取想要购买的药水类型。
 *
 * @details 包含完整的输入验证逻辑，处理了非法字符、多余输入以及退出指令。
 *
 * @return Potion::Type 用户选择的药水类型。如果用户输入 'q' 选择退出，则返回 Potion::max_potions。
 */
Potion::Type whichPotion()
{
    std::cout << "Enter the number of the potion you'd like to buy, or 'q' to quit: ";
    char input{};

    while (true)
    {
        std::cin >> input;

        // 如果输入流状态错误（例如用户输入了无法转换为 char 的内容），进行恢复
        if (!std::cin)
        {
            std::cin.clear(); // 清除错误标志（如 failbit），使流恢复正常操作模式
            ignoreLine();     // 清除缓冲区中的错误输入
            continue;
        }

        // 检查是否有额外的多余输入（例如用户输入了 "1a"，'1' 被读取后，'a' 还在缓冲区）
        // std::cin.peek() 用于查看下一个字符但不提取它
        if (!std::cin.eof() && std::cin.peek() != '\n')
        {
            std::cout << "I didn't understand what you said.  Try again: ";
            ignoreLine(); // 忽略多余的输入
            continue;
        }

        // 用户选择退出游戏
        if (input == 'q')
            return Potion::max_potions;

        // 将字符转换为数字并检查是否为有效的药水索引
        int val{charNumToInt(input)};
        if (val >= 0 && val < Potion::max_potions)
            return static_cast<Potion::Type>(val);

        // 输入了不在有效范围内的字符
        std::cout << "I didn't understand what you said.  Try again: ";
        ignoreLine();
    }
}

/**
 * @brief 商店主循环，展示商品并处理玩家的购买逻辑。
 *
 * @param player 玩家对象的引用，用于修改金币和库存。
 */
void shop(Player &player)
{
    while (true)
    {
        std::cout << "Here is our selection for today:\n";

        // 遍历并打印所有药水及其价格
        for (auto p : Potion::types)
            std::cout << p << ") " << Potion::name[p] << " costs " << Potion::cost[p] << '\n';

        Potion::Type which{whichPotion()};

        // 如果用户选择退出，则结束商店循环
        if (which == Potion::max_potions)
            return;

        bool success{player.buy(which)};
        if (!success)
            std::cout << "You can not afford that.\n\n";
        else
            std::cout << "You purchased a potion of " << Potion::name[which] << ".  You have " << player.gold() << " gold left.\n\n";
    }
}

/**
 * @brief 打印玩家的最终库存和剩余金币。
 *
 * @param player 玩家对象的常量引用，因为该函数仅读取状态。
 */
void printInventory(const Player &player)
{
    std::cout << "Your inventory contains: \n";

    for (auto p : Potion::types)
    {
        if (player.inventory(p) > 0)
            std::cout << player.inventory(p) << "x potion of " << Potion::name[p] << '\n';
    }

    std::cout << "You escaped with " << player.gold() << " gold remaining.\n";
}

/**
 * @brief 程序入口，初始化并运行游戏主流程。
 *
 * @return int 程序退出状态码。
 */
int main()
{
    std::cout << "Welcome to Roscoe's potion emporium!\n";
    std::cout << "Enter your name: ";

    std::string name{};
    std::cin >> name;

    Player player{name};

    std::cout << "Hello, " << name << ", you have " << player.gold() << " gold.\n\n";

    shop(player);

    std::cout << '\n';

    printInventory(player);

    std::cout << "\nThanks for shopping at Roscoe's potion emporium!\n";

    return 0;
}