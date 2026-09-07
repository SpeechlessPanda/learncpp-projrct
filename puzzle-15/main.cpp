#include <cassert>
#include <iostream>
#include <limits>
#include <utility> // for std::swap

// 注意：Random.h 通常是外部依赖（如 LearnCpp 教程提供的随机数库），
// 请确保该文件存在于你的项目包含路径中。
#include "Random.h"

/**
 * @brief 游戏全局配置设置。
 */
namespace Settings
{
    /// @brief 控制台清屏时打印的空行数，用于将棋盘固定在窗口底部。
    constexpr int g_consoleLines{25};
}

/**
 * @brief 表示移动方向的类。
 */
class Direction
{
public:
    /**
     * @brief 方向枚举类型。使用 enum class 防止隐式转换和命名污染。
     */
    enum class Type
    {
        up,
        down,
        left,
        right,
        maxDirections,
    };

    /**
     * @brief 构造函数。
     * @param type 方向类型。
     */
    explicit Direction(Type type)
        : m_type(type)
    {
    }

    /**
     * @brief 获取当前方向类型。
     * @return Type 方向类型。
     */
    Type getType() const
    {
        return m_type;
    }

    /**
     * @brief 单目负号运算符，返回相反方向。
     * @return Direction 相反方向。
     */
    Direction operator-() const
    {
        switch (m_type)
        {
        case Type::up:
            return Direction{Type::down};
        case Type::down:
            return Direction{Type::up};
        case Type::left:
            return Direction{Type::right};
        case Type::right:
            return Direction{Type::left};
        default:
            break;
        }

        assert(false && "Unsupported direction was passed!");
        return Direction{Type::up};
    }

    /**
     * @brief 生成一个随机的方向。
     * @return Direction 随机方向。
     */
    static Direction getRandomDirection()
    {
        // 使用 static_cast 将随机整数安全地转换为枚举类型
        Type random{static_cast<Type>(Random::get(0, static_cast<int>(Type::maxDirections) - 1))};
        return Direction{random};
    }

private:
    Type m_type{};
};

/**
 * @brief 输出流运算符，将方向打印为字符串。
 */
std::ostream &operator<<(std::ostream &stream, const Direction &dir)
{
    switch (dir.getType())
    {
    case Direction::Type::up:
        return (stream << "up");
    case Direction::Type::down:
        return (stream << "down");
    case Direction::Type::left:
        return (stream << "left");
    case Direction::Type::right:
        return (stream << "right");
    default:
        break;
    }

    assert(false && "Unsupported direction was passed!");
    return (stream << "unknown direction");
}

/**
 * @brief 表示二维坐标点的结构体。
 */
struct Point
{
    int x{};
    int y{};

    /**
     * @brief 相等比较运算符。
     */
    friend bool operator==(const Point &p1, const Point &p2)
    {
        return p1.x == p2.x && p1.y == p2.y;
    }

    /**
     * @brief 不等比较运算符。
     */
    friend bool operator!=(const Point &p1, const Point &p2)
    {
        return !(p1 == p2);
    }

    /**
     * @brief 根据给定方向获取相邻的坐标点。
     * @param dir 移动方向。
     * @return Point 相邻的坐标点。
     */
    Point getAdjacentPoint(Direction dir) const
    {
        switch (dir.getType())
        {
        case Direction::Type::up:
            return Point{x, y - 1};
        case Direction::Type::down:
            return Point{x, y + 1};
        case Direction::Type::left:
            return Point{x - 1, y};
        case Direction::Type::right:
            return Point{x + 1, y};
        default:
            break;
        }

        assert(false && "Unsupported direction was passed!");
        return *this;
    }
};

/**
 * @brief 处理用户输入的命名空间。
 */
namespace UserInput
{
    /**
     * @brief 检查字符是否为有效的游戏命令。
     */
    bool isValidCommand(char ch)
    {
        return ch == 'w' || ch == 'a' || ch == 's' || ch == 'd' || ch == 'q';
    }

    /**
     * @brief 清空输入缓冲区中当前行的剩余字符。
     */
    void ignoreLine()
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    /**
     * @brief 从标准输入获取单个字符。
     */
    char getCharacter()
    {
        char operation{};
        std::cin >> operation;
        ignoreLine(); // 清除多余的输入
        return operation;
    }

    /**
     * @brief 循环提示用户输入，直到获取到有效的命令字符。
     */
    char getCommandFromUser()
    {
        char ch{};
        while (!isValidCommand(ch))
        {
            ch = getCharacter();
        }
        return ch;
    }

    /**
     * @brief 将字符命令转换为 Direction 对象。
     */
    Direction charToDirection(char ch)
    {
        switch (ch)
        {
        case 'w':
            return Direction{Direction::Type::up};
        case 's':
            return Direction{Direction::Type::down};
        case 'a':
            return Direction{Direction::Type::left};
        case 'd':
            return Direction{Direction::Type::right};
        default:
            break;
        }

        assert(false && "Unsupported direction was passed!");
        return Direction{Direction::Type::up};
    }
}

/**
 * @brief 表示棋盘上的单个格子。
 */
class Tile
{
public:
    Tile() = default;

    explicit Tile(int number)
        : m_num(number)
    {
    }

    /**
     * @brief 判断格子是否为空（数字为 0）。
     */
    bool isEmpty() const
    {
        return m_num == 0;
    }

    /**
     * @brief 获取格子上的数字。
     */
    int getNum() const { return m_num; }

private:
    int m_num{0};
};

/**
 * @brief 输出流运算符，格式化打印单个格子。
 */
std::ostream &operator<<(std::ostream &stream, const Tile &tile)
{
    if (tile.getNum() > 9) // 两位数
        stream << " " << tile.getNum() << " ";
    else if (tile.getNum() > 0) // 一位数
        stream << "  " << tile.getNum() << " ";
    else if (tile.getNum() == 0) // 空格子
        stream << "    ";
    return stream;
}

/**
 * @brief 表示 15 拼图（数字推盘）的棋盘。
 */
class Board
{
public:
    Board() = default;

    /**
     * @brief 打印指定数量的空行，用于控制台清屏效果。
     */
    static void printEmptyLines(int count)
    {
        for (int i = 0; i < count; ++i)
            std::cout << '\n';
    }

    /**
     * @brief 输出流运算符，打印整个棋盘。
     */
    friend std::ostream &operator<<(std::ostream &stream, const Board &board)
    {
        // 打印空行，使棋盘始终显示在控制台底部
        Board::printEmptyLines(Settings::g_consoleLines);

        for (int y = 0; y < s_size; ++y)
        {
            for (int x = 0; x < s_size; ++x)
                stream << board.m_tiles[y][x];
            stream << '\n';
        }

        return stream;
    }

    /**
     * @brief 查找棋盘上空格子（数字为 0）的坐标。
     */
    Point getEmptyTilePos() const
    {
        for (int y = 0; y < s_size; ++y)
            for (int x = 0; x < s_size; ++x)
                if (m_tiles[y][x].isEmpty())
                    return {x, y};

        assert(false && "There is no empty tile in the board!!!");
        return {-1, -1};
    }

    /**
     * @brief 检查给定的坐标是否在棋盘有效范围内。
     */
    static bool isValidTilePos(Point pt)
    {
        return (pt.x >= 0 && pt.x < s_size) && (pt.y >= 0 && pt.y < s_size);
    }

    /**
     * @brief 交换棋盘上两个格子的内容。
     */
    void swapTiles(Point pt1, Point pt2)
    {
        std::swap(m_tiles[pt1.y][pt1.x], m_tiles[pt2.y][pt2.x]);
    }

    /**
     * @brief 比较两个棋盘的状态是否完全相同。
     */
    friend bool operator==(const Board &f1, const Board &f2)
    {
        for (int y = 0; y < s_size; ++y)
            for (int x = 0; x < s_size; ++x)
                if (f1.m_tiles[y][x].getNum() != f2.m_tiles[y][x].getNum())
                    return false;

        return true;
    }

    /**
     * @brief 尝试向指定方向移动格子。
     * @param dir 玩家输入的移动方向。
     * @return bool 如果移动成功返回 true，否则返回 false。
     */
    bool moveTile(Direction dir)
    {
        Point emptyTile{getEmptyTilePos()};

        // 核心逻辑：玩家按下的方向是“空格子”移动的方向，
        // 实际上是被移动的方块向相反方向 (-dir) 滑入空格子
        Point adj{emptyTile.getAdjacentPoint(-dir)};

        if (!isValidTilePos(adj))
            return false;

        swapTiles(adj, emptyTile);
        return true;
    }

    /**
     * @brief 检查玩家是否已经获胜（棋盘恢复到初始有序状态）。
     */
    bool playerWon() const
    {
        static Board s_solved{}; // 生成一个已解决的初始棋盘作为对比基准
        return s_solved == *this;
    }

    /**
     * @brief 随机打乱棋盘。
     * @details 通过随机移动空格子 1000 次来打乱，这种物理模拟的方式能保证打乱后的棋盘一定是可解的。
     */
    void randomize()
    {
        for (int i = 0; i < 1000;)
        {
            if (moveTile(Direction::getRandomDirection()))
                ++i;
        }
    }

private:
    static constexpr int s_size{4};

    // 初始状态（已解决状态）的棋盘布局
    Tile m_tiles[s_size][s_size]{
        Tile{1}, Tile{2}, Tile{3}, Tile{4},
        Tile{5}, Tile{6}, Tile{7}, Tile{8},
        Tile{9}, Tile{10}, Tile{11}, Tile{12},
        Tile{13}, Tile{14}, Tile{15}, Tile{0}};
};

/**
 * @brief 程序入口，运行 15 拼图游戏主循环。
 */
int main()
{
    Board board{};
    board.randomize();
    std::cout << board;

    while (!board.playerWon())
    {
        char ch{UserInput::getCommandFromUser()};

        // 处理退出命令
        if (ch == 'q')
        {
            std::cout << "\n\nBye!\n\n";
            return 0;
        }

        // 处理方向命令
        Direction dir{UserInput::charToDirection(ch)};

        bool userMoved{board.moveTile(dir)};
        if (userMoved)
            std::cout << board;
    }

    std::cout << "\n\nYou won!\n\n";
    return 0;
}