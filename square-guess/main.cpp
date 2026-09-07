#include <algorithm> // std::find, std::min_element
#include <cmath>     // std::abs
#include <cstddef>   // std::size_t
#include <iostream>
#include <vector>

// 注意：Random.h 通常是外部依赖（如 LearnCpp 教程提供的随机数库），
// 请确保该文件存在于你的项目包含路径中。
#include "Random.h"

/**
 * @brief 游戏中使用的数字集合类型别名。
 */
using Numbers = std::vector<int>;

/**
 * @brief 游戏全局配置参数。
 */
namespace config
{
    /// @brief 乘数的最小值。
    constexpr int multiplierMin{2};
    /// @brief 乘数的最大值。
    constexpr int multiplierMax{6};
}

/**
 * @brief 生成指定数量的平方数，并乘以给定的乘数。
 *
 * @details 从 @p start 开始，生成 @p count 个连续整数的平方，
 * 并将每个平方数乘以 @p multiplier。
 *
 * @param start 起始整数。
 * @param count 要生成的数字数量。
 * @param multiplier 乘数。
 * @return Numbers 包含生成数字的向量。
 */
Numbers generateNumbers(int start, int count, int multiplier)
{
    // 预分配 vector 容量，避免push_back带来的扩容开销
    Numbers numbers(static_cast<std::size_t>(count));

    for (int index = 0; index < count; ++index)
    {
        // 显式转换为 std::size_t 以消除有符号/无符号索引的编译警告
        auto uindex{static_cast<std::size_t>(index)};
        numbers[uindex] = (start + index) * (start + index) * multiplier;
    }

    return numbers;
}

/**
 * @brief 初始化游戏，获取用户输入并生成目标数字集合。
 *
 * @return Numbers 生成的目标数字集合。
 */
Numbers setupGame()
{
    int start{};
    std::cout << "Start where? ";
    std::cin >> start;

    int count{};
    std::cout << "How many? ";
    std::cin >> count;

    // 假设 Random::get 能够生成 [min, max] 范围内的随机整数
    int multiplier{Random::get(config::multiplierMin, config::multiplierMax)};

    std::cout << "I generated " << count
              << " square numbers. Do you know what each number is after multiplying it by "
              << multiplier << "?\n";

    return generateNumbers(start, count, multiplier);
}

/**
 * @brief 获取用户的猜测输入。
 *
 * @return int 用户输入的数字。
 */
int getUserGuess()
{
    int guess{};

    std::cout << "> ";
    std::cin >> guess;

    return guess;
}

/**
 * @brief 在数字集合中查找并移除指定的猜测值。
 *
 * @param numbers 目标数字集合（会被修改）。
 * @param guess 用户的猜测值。
 * @return bool 如果找到并成功移除返回 true，否则返回 false。
 */
bool findAndRemove(Numbers &numbers, int guess)
{
    auto found{std::find(numbers.begin(), numbers.end(), guess)};

    if (found == numbers.end())
    {
        return false;
    }

    // 从 vector 中移除元素，时间复杂度为 O(N)
    numbers.erase(found);
    return true;
}

/**
 * @brief 在数字集合中寻找与猜测值最接近的数字。
 *
 * @param numbers 目标数字集合（必须非空）。
 * @param guess 用户的猜测值。
 * @return int 与猜测值绝对差最小的数字。
 */
int findClosestNumber(const Numbers &numbers, int guess)
{
    // 防御性检查：确保集合不为空，避免对 end() 迭代器解引用导致未定义行为
    if (numbers.empty())
    {
        return guess;
    }

    return *std::min_element(numbers.begin(), numbers.end(),
                             [guess](int a, int b) // 优化：显式捕获 guess，替代 [=]，意图更清晰且符合现代 C++ 规范
                             {
                                 return std::abs(a - guess) < std::abs(b - guess);
                             });
}

/**
 * @brief 猜测正确时打印成功信息及剩余数字数量。
 *
 * @param numbers 当前剩余的数字集合。
 */
void printSuccess(const Numbers &numbers)
{
    std::cout << "Nice! ";

    // 优化：使用 empty() 替代 size() == 0，更符合 C++ 容器判空的标准做法
    if (numbers.empty())
    {
        std::cout << "You found all numbers, good job!\n";
    }
    else
    {
        std::cout << numbers.size() << " number(s) left.\n";
    }
}

/**
 * @brief 猜测错误时打印失败信息及最接近的正确数字。
 *
 * @param numbers 当前剩余的数字集合。
 * @param guess 用户的错误猜测值。
 */
void printFailure(const Numbers &numbers, int guess)
{
    int closest{findClosestNumber(numbers, guess)};

    std::cout << guess << " is wrong!\n";
    std::cout << "Try " << closest << " next time.\n";
}

/**
 * @brief 程序入口，运行游戏主循环。
 *
 * @return int 程序退出状态码。
 */
int main()
{
    Numbers numbers{setupGame()};

    while (true)
    {
        int guess{getUserGuess()};

        if (!findAndRemove(numbers, guess))
        {
            printFailure(numbers, guess);
            break;
        }

        printSuccess(numbers);

        // 优化：使用 empty() 替代 size() == 0
        if (numbers.empty())
        {
            break;
        }
    }

    return 0;
}