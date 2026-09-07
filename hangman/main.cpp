#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <string_view>
#include <vector>

// 注意：Random.h 通常是外部依赖（如 LearnCpp 教程提供的随机数库），
// 请确保该文件存在于你的项目包含路径中。
#include "Random.h"

/**
 * @brief 游戏全局配置设置。
 */
namespace Settings
{
    /// @brief 游戏中允许的最大错误猜测次数。
    constexpr int wrongGuessesAllowed{6};
}

/**
 * @brief 游戏单词列表管理。
 */
namespace WordList
{
    /// @brief 存储游戏中的候选单词列表。
    const std::vector<std::string_view> words{
        "mystery", "broccoli", "account", "almost",
        "spaghetti", "opinion", "beautiful", "distance", "luggage"};

    /**
     * @brief 从候选单词列表中随机返回一个单词。
     *
     * @return std::string_view 随机选中的单词视图。
     */
    std::string_view getRandomWord()
    {
        return words[Random::get<std::size_t>(0, words.size() - 1)];
    }
}

/**
 * @brief 管理单次 Hangman 游戏会话的状态和逻辑。
 */
class Session
{
private:
    std::string_view m_word{WordList::getRandomWord()};
    int m_wrongGuessesLeft{Settings::wrongGuessesAllowed};

    // 优化：使用 std::array 替代 std::vector，因为字母数量固定为 26，
    // 避免了 std::vector 的堆内存分配开销，且访问性能更好。
    std::array<bool, 26> m_letterGuessed{};

    /**
     * @brief 将小写字母转换为 0-25 的数组索引。
     *
     * @param c 小写字母 ('a'-'z')。
     * @return std::size_t 对应的数组索引。
     */
    std::size_t toIndex(char c) const
    {
        // 优化：输入已限制为小写，直接相减比 (c % 32) - 1 更直观高效。
        return static_cast<std::size_t>(c - 'a');
    }

public:
    /**
     * @brief 获取当前目标单词。
     * @return std::string_view 目标单词。
     */
    std::string_view getWord() const { return m_word; }

    /**
     * @brief 获取剩余的错误猜测次数。
     * @return int 剩余次数。
     */
    int wrongGuessesLeft() const { return m_wrongGuessesLeft; }

    /**
     * @brief 扣除一次错误猜测机会。
     */
    void removeGuess() { --m_wrongGuessesLeft; }

    /**
     * @brief 检查某个字母是否已经被猜测过。
     * @param c 要检查的字母。
     * @return bool 如果已猜测返回 true，否则返回 false。
     */
    bool isLetterGuessed(char c) const { return m_letterGuessed[toIndex(c)]; }

    /**
     * @brief 将某个字母标记为已猜测。
     * @param c 要标记的字母。
     */
    void setLetterGuessed(char c) { m_letterGuessed[toIndex(c)] = true; }

    /**
     * @brief 检查某个字母是否存在于目标单词中。
     * @param c 要检查的字母。
     * @return bool 如果存在返回 true，否则返回 false。
     */
    bool isLetterInWord(char c) const
    {
        // 优化：使用标准库算法 std::find 替代手写循环，代码更简洁。
        return std::find(m_word.begin(), m_word.end(), c) != m_word.end();
    }

    /**
     * @brief 检查玩家是否已经猜出了所有字母（即是否获胜）。
     * @return bool 如果获胜返回 true，否则返回 false。
     */
    bool won() const
    {
        for (char c : m_word)
        {
            if (!isLetterGuessed(c))
            {
                return false;
            }
        }
        return true;
    }
};

/**
 * @brief 在控制台绘制当前的游戏状态（单词进度、剩余机会、错误猜测）。
 *
 * @param s 当前游戏会话的常量引用。
 */
void draw(const Session &s)
{
    std::cout << '\n';

    std::cout << "The word: ";
    for (char c : s.getWord())
    {
        if (s.isLetterGuessed(c))
            std::cout << c;
        else
            std::cout << '_';
    }

    std::cout << "   Wrong guesses: ";
    for (int i = 0; i < s.wrongGuessesLeft(); ++i)
        std::cout << '+';

    // 打印所有猜错的字母
    for (char c = 'a'; c <= 'z'; ++c)
    {
        if (s.isLetterGuessed(c) && !s.isLetterInWord(c))
            std::cout << c;
    }

    std::cout << '\n';
}

/**
 * @brief 获取并验证用户的字母猜测输入。
 *
 * @details 循环提示用户输入，直到输入有效的小写字母且未被猜测过。
 * 处理了非字符输入和多余缓冲区的清理。
 *
 * @param s 当前游戏会话的常量引用，用于检查字母是否已被猜测。
 * @return char 有效的用户猜测字母。
 */
char getGuess(const Session &s)
{
    while (true)
    {
        std::cout << "Enter your next letter: ";

        char c{};
        std::cin >> c;

        // 如果输入流状态错误（如输入了非字符内容），进行恢复
        if (!std::cin)
        {
            std::cin.clear(); // 清除错误标志
            std::cout << "That wasn't a valid input.  Try again.\n";
            // 忽略缓冲区中直到换行符的所有剩余字符
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        // 清除输入行中多余的字符（防止用户输入 "abc" 时，'b' 和 'c' 被当作后续输入）
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // 验证是否为小写字母
        if (c < 'a' || c > 'z')
        {
            std::cout << "That wasn't a valid input.  Try again.\n";
            continue;
        }

        // 验证是否已经猜测过该字母
        if (s.isLetterGuessed(c))
        {
            std::cout << "You already guessed that.  Try again.\n";
            continue;
        }

        return c;
    }
}

/**
 * @brief 处理用户的猜测并更新游戏状态。
 *
 * @param s 当前游戏会话的引用，用于更新状态。
 * @param c 用户猜测的字母。
 */
void handleGuess(Session &s, char c)
{
    s.setLetterGuessed(c);

    if (s.isLetterInWord(c))
    {
        std::cout << "Yes, '" << c << "' is in the word!\n";
        return;
    }

    std::cout << "No, '" << c << "' is not in the word!\n";
    s.removeGuess();
}

/**
 * @brief 程序入口，初始化并运行游戏主循环。
 *
 * @return int 程序退出状态码。
 */
int main()
{
    std::cout << "Welcome to C++man (a variant of Hangman)\n";
    std::cout << "To win: guess the word.  To lose: run out of pluses.\n";

    Session s{};

    // 优化：使用 > 0 替代隐式布尔转换，使条件判断更清晰
    while (s.wrongGuessesLeft() > 0 && !s.won())
    {
        draw(s);
        char c{getGuess(s)};
        handleGuess(s, c);
    }

    // 绘制游戏结束时的最终状态
    draw(s);

    // 优化：使用 == 0 替代隐式布尔转换
    if (s.wrongGuessesLeft() == 0)
        std::cout << "You lost!  The word was: " << s.getWord() << '\n';
    else
        std::cout << "You won!\n";

    return 0;
}