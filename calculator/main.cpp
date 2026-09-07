#include <cstdlib> // for std::exit
#include <iostream>
#include <limits> // for std::numeric_limits

void ignoreLine()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// 如果提取失败返回 true，否则返回 false
bool clearFailedExtraction()
{
    // 检查是否提取失败
    if (!std::cin) // 如果上一次提取失败
    {
        if (std::cin.eof()) // 如果流已关闭
        {
            std::exit(0); // 立即关闭程序
        }

        // 处理失败
        std::cin.clear(); // 恢复为“正常”操作模式
        ignoreLine();     // 移除错误输入

        return true;
    }

    return false;
}

// 如果 std::cin 在当前行还有未提取的输入，返回 true，否则返回 false
bool hasUnextractedInput()
{
    return !std::cin.eof() && std::cin.peek() != '\n';
}

double getDouble()
{
    while (true) // 循环直到用户输入有效值
    {
        std::cout << "Enter a decimal number: ";
        double x{};
        std::cin >> x;

        if (clearFailedExtraction())
        {
            std::cout << "Oops, that input is invalid.  Please try again.\n";
            continue;
        }

        ignoreLine(); // 移除任何多余输入
        return x;     // 返回提取到的值
    }
}

char getOperator()
{
    while (true) // 循环直到用户输入有效值
    {
        std::cout << "Enter one of the following: +, -, *, or /: ";
        char operation{};
        std::cin >> operation;

        if (!clearFailedExtraction()) // 如果提取失败，错误提示会在下面处理
            ignoreLine();             // 仅在提取成功时移除多余输入

        // 检查用户是否输入了有意义的值
        switch (operation)
        {
        case '+':
        case '-':
        case '*':
        case '/':
            return operation; // 将输入的字符返回给调用者
        default:              // 否则告诉用户哪里出错了
            std::cout << "Oops, that input is invalid.  Please try again.\n";
        }
    }
}

void printResult(double x, char operation, double y)
{
    std::cout << x << ' ' << operation << ' ' << y << " is ";

    switch (operation)
    {
    case '+':
        std::cout << x + y << '\n';
        return;
    case '-':
        std::cout << x - y << '\n';
        return;
    case '*':
        std::cout << x * y << '\n';
        return;
    case '/':
        if (y == 0.0)
            break;

        std::cout << x / y << '\n';
        return;
    }

    std::cout << "???"; // 健壮性意味着也要处理意外参数，即使在本程序中 getOperator() 已保证 operation 是有效的
}

int main()
{
    double x{getDouble()};
    char operation{getOperator()};
    double y{getDouble()};

    // 处理除以 0 的情况
    while (operation == '/' && y == 0.0)
    {
        std::cout << "The denominator cannot be zero.  Try again.\n";
        y = getDouble();
    }

    printResult(x, operation, y);

    return 0;
}