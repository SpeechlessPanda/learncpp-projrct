#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>

// 注意：Random.h 通常是外部依赖（如 LearnCpp 教程提供的随机数库），
// 请确保该文件存在于你的项目包含路径中，且内部定义了 Random::mt (std::mt19937)。
#include "Random.h"

/**
 * @brief 游戏全局配置设置。
 */
namespace Settings
{
    /// @brief 爆牌（Bust）的临界分数，超过此分数即判负。
    constexpr int bust{21};

    /// @brief 庄家（Dealer）停止要牌的最低分数。
    constexpr int dealerStopsAt{17};
}

/**
 * @brief 扑克牌结构体，包含花色、点数及其相关操作。
 */
struct Card
{
    /**
     * @brief 扑克牌点数枚举。
     */
    enum Rank
    {
        rank_ace,
        rank_2,
        rank_3,
        rank_4,
        rank_5,
        rank_6,
        rank_7,
        rank_8,
        rank_9,
        rank_10,
        rank_jack,
        rank_queen,
        rank_king,
        max_ranks
    };

    /**
     * @brief 扑克牌花色枚举。
     */
    enum Suit
    {
        suit_club,    // 梅花
        suit_diamond, // 方块
        suit_heart,   // 红桃
        suit_spade,   // 黑桃
        max_suits
    };

    /// @brief 包含所有有效点数的数组，方便统一遍历
    static constexpr std::array<Rank, max_ranks> allRanks{
        rank_ace, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7,
        rank_8, rank_9, rank_10, rank_jack, rank_queen, rank_king};

    /// @brief 包含所有有效花色的数组，方便统一遍历
    static constexpr std::array<Suit, max_suits> allSuits{
        suit_club, suit_diamond, suit_heart, suit_spade};

    Rank rank{};
    Suit suit{};

    /**
     * @brief 重载输出运算符，以 "点数花色" 的格式打印扑克牌（如 "AH" 代表红桃A）。
     */
    friend std::ostream &operator<<(std::ostream &out, const Card &card)
    {
        static constexpr std::array<char, Card::max_ranks> ranks{
            'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
        static constexpr std::array<char, Card::max_suits> suits{
            'C', 'D', 'H', 'S'};

        out << ranks[card.rank] << suits[card.suit];
        return out;
    }

    /**
     * @brief 获取扑克牌在 21 点游戏中的基础分值。
     * @return int 牌的分值（A 默认算作 11 点，J/Q/K 算作 10 点）。
     */
    int value() const
    {
        static constexpr std::array<int, max_ranks> rankValues{
            11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};
        return rankValues[rank];
    }
};

/**
 * @brief 牌堆类，管理 52 张扑克牌的初始化、洗牌和发牌。
 */
class Deck
{
private:
    std::array<Card, 52> m_cards{};
    std::size_t m_nextCardIndex{0};

public:
    /**
     * @brief 构造函数，初始化 52 张扑克牌。
     */
    Deck()
    {
        std::size_t count{0};
        for (auto suit : Card::allSuits)
        {
            for (auto rank : Card::allRanks)
            {
                m_cards[count++] = Card{rank, suit};
            }
        }
    }

    /**
     * @brief 洗牌并重置发牌索引。
     */
    void shuffle()
    {
        std::shuffle(m_cards.begin(), m_cards.end(), Random::mt);
        m_nextCardIndex = 0;
    }

    /**
     * @brief 从牌堆顶部发一张牌。
     * @return Card 发出的扑克牌。
     */
    Card dealCard()
    {
        assert(m_nextCardIndex != 52 && "Deck::dealCard ran out of cards");
        return m_cards[m_nextCardIndex++];
    }
};

/**
 * @brief 玩家类，管理玩家的分数和 Ace（A士）的特殊计分逻辑。
 */
class Player
{
private:
    int m_score{};
    int m_ace11Count{0}; // 记录当前算作 11 点的 Ace 的数量

public:
    /**
     * @brief 将一张牌加入玩家的总分，并处理 Ace 的计分逻辑。
     * @param card 要加入的扑克牌。
     */
    void addToScore(Card card)
    {
        m_score += card.value();
        if (card.rank == Card::rank_ace)
        {
            ++m_ace11Count; // Ace 初始默认算作 11 点
        }
        consumeAces();
    }

    /**
     * @brief 处理爆牌时的 Ace 降级逻辑。
     * @details 如果总分超过 21 点，且手中还有算作 11 点的 Ace，
     * 则将其降级为 1 点（即总分减去 10），直到不爆牌或没有 Ace 可降级为止。
     */
    void consumeAces()
    {
        while (m_score > Settings::bust && m_ace11Count > 0)
        {
            m_score -= 10;
            --m_ace11Count;
        }
    }

    /// @brief 获取玩家当前总分。
    int score() const { return m_score; }
};

/**
 * @brief 提示玩家选择“要牌”或“停牌”，并处理输入验证。
 * @return bool 如果要牌返回 true，停牌返回 false。
 */
bool playerWantsHit()
{
    while (true)
    {
        std::cout << "(h) to hit, or (s) to stand: ";

        char ch{};
        std::cin >> ch;

        // 优化：清除输入缓冲区中当前行的剩余字符，防止用户输入 "hh" 导致连续触发
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (ch)
        {
        case 'h':
            return true;
        case 's':
            return false;
        default:
            std::cout << "Invalid input. Please enter 'h' or 's'.\n";
        }
    }
}

/**
 * @brief 执行玩家的回合。
 * @param deck 牌堆引用。
 * @param player 玩家对象引用。
 * @return bool 如果玩家爆牌（Bust）返回 true，否则返回 false。
 */
bool playerTurn(Deck &deck, Player &player)
{
    while (player.score() < Settings::bust && playerWantsHit())
    {
        Card card{deck.dealCard()};
        player.addToScore(card);

        std::cout << "You were dealt " << card << ". You now have: " << player.score() << '\n';
    }

    if (player.score() > Settings::bust)
    {
        std::cout << "You went bust!\n";
        return true;
    }

    return false;
}

/**
 * @brief 执行庄家的回合。
 * @param deck 牌堆引用。
 * @param dealer 庄家对象引用。
 * @return bool 如果庄家爆牌（Bust）返回 true，否则返回 false。
 */
bool dealerTurn(Deck &deck, Player &dealer)
{
    // 庄家规则：分数低于 17 点必须强制要牌
    while (dealer.score() < Settings::dealerStopsAt)
    {
        Card card{deck.dealCard()};
        dealer.addToScore(card);

        std::cout << "The dealer flips a " << card << ".  They now have: " << dealer.score() << '\n';
    }

    if (dealer.score() > Settings::bust)
    {
        std::cout << "The dealer went bust!\n";
        return true;
    }

    return false;
}

/**
 * @brief 游戏结果枚举。
 */
enum class GameResult
{
    playerWon,
    dealerWon,
    tie
};

/**
 * @brief 运行一局完整的 21 点游戏。
 * @return GameResult 游戏的最终结果。
 */
GameResult playBlackjack()
{
    Deck deck{};
    deck.shuffle();

    // 庄家初始发牌（明牌）
    Player dealer{};
    Card card1{deck.dealCard()};
    dealer.addToScore(card1);
    std::cout << "The dealer is showing " << card1 << " (" << dealer.score() << ")\n";

    // 玩家初始发牌（两张明牌）
    Player player{};
    Card card2{deck.dealCard()};
    Card card3{deck.dealCard()};
    player.addToScore(card2);
    player.addToScore(card3);
    std::cout << "You are showing " << card2 << ' ' << card3 << " (" << player.score() << ")\n";

    if (playerTurn(deck, player)) // 玩家爆牌，庄家直接获胜
        return GameResult::dealerWon;

    if (dealerTurn(deck, dealer)) // 庄家爆牌，玩家直接获胜
        return GameResult::playerWon;

    // 双方均未爆牌，比较分数
    if (player.score() == dealer.score())
        return GameResult::tie;

    return (player.score() > dealer.score() ? GameResult::playerWon : GameResult::dealerWon);
}

/**
 * @brief 程序入口。
 * @return int 程序退出状态码。
 */
int main()
{
    switch (playBlackjack())
    {
    case GameResult::playerWon:
        std::cout << "You win!\n";
        break;
    case GameResult::dealerWon:
        std::cout << "You lose!\n";
        break;
    case GameResult::tie:
        std::cout << "It's a tie.\n";
        break;
    }

    return 0;
}