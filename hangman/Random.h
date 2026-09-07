#ifndef RANDOM_MT_H
#define RANDOM_MT_H

#include <chrono>
#include <random>

// 本头文件实现的 Random 命名空间是一个自播种的 Mersenne Twister。
// 需要 C++17 或更新版本。
// 可以根据需要 #include 到任意多个代码文件中（inline 关键字可避免 ODR 违规）
// 可自由再分发，来自 learncpp.com（https://learncpp.cn/cpp-tutorial/global-random-numbers-random-h/）
namespace Random
{
    // 返回一个已播种的 Mersenne Twister
    // 注意：我们更希望返回 std::seed_seq（用于初始化 std::mt19937），但 std::seed_seq 不可复制，因此无法按值返回。
    // 所以，我们先创建一个 std::mt19937，对它播种，然后返回这个可以复制的 std::mt19937。
    inline std::mt19937 generate()
    {
        std::random_device rd{};

        // 用时钟和来自 std::random_device 的 7 个随机数创建 seed_seq
        std::seed_seq ss{
            static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
            rd(), rd(), rd(), rd(), rd(), rd(), rd()};

        return std::mt19937{ss};
    }

    // 这是全局的 std::mt19937 对象。
    // inline 关键字确保整个程序只有一个全局实例。
    inline std::mt19937 mt{generate()}; // 生成一个已播种的 std::mt19937，并将其复制到全局对象中

    // 生成一个范围在 [min, max]（含）之间的随机 int
    // * 也能处理两个参数类型不同但可以转换为 int 的情况
    inline int get(int min, int max)
    {
        return std::uniform_int_distribution{min, max}(mt);
    }

    // 以下函数模板可用于其他情况生成随机数

    // 参见 https://learncpp.cn/cpp-tutorial/function-template-instantiation/
    // 如果你不理解，可以先忽略它们

    // 生成一个范围在 [min, max]（含）之间的随机值
    // * min 和 max 必须具有相同类型
    // * 返回值类型与 min、max 相同
    // * 支持的类型：
    // *    short, int, long, long long
    // *    unsigned short, unsigned int, unsigned long, unsigned long long
    // 示例调用：Random::get(1L, 6L);             // 返回 long
    // 示例调用：Random::get(1u, 6u);             // 返回 unsigned int
    template <typename T>
    T get(T min, T max)
    {
        return std::uniform_int_distribution<T>{min, max}(mt);
    }

    // 生成一个范围在 [min, max]（含）之间的随机值
    // * min 和 max 可以具有不同类型
    // * 返回值类型必须显式指定为模板参数
    // * min 和 max 会被转换为返回类型
    // 示例调用：Random::get<std::size_t>(0, 6);  // 返回 std::size_t
    // 示例调用：Random::get<std::size_t>(0, 6u); // 返回 std::size_t
    // 示例调用：Random::get<std::int>(0, 6u);    // 返回 int
    template <typename R, typename S, typename T>
    R get(S min, T max)
    {
        return get<R>(static_cast<R>(min), static_cast<R>(max));
    }
}

#endif