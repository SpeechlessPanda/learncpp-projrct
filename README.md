# LearnCpp 小项目集

<p align="center">
  <img src="https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++17" />
  <img src="https://img.shields.io/badge/g++-MinGW-success?style=for-the-badge&logo=gnu&logoColor=white" alt="g++" />
  <img src="https://img.shields.io/badge/LearnCpp-教程练习-orange?style=for-the-badge" alt="LearnCpp" />
</p>

<p align="center">
  从 <a href="https://www.learncpp.com/">LearnCpp</a> / <a href="https://learncpp.cn/">learncpp.cn</a>
  教程中整理出的<strong>可独立编译、可直接游玩</strong>的控制台小项目。
</p>

---

每个子目录都是一个完整程序：源码、依赖头文件都在目录内，进入后一条命令即可编译运行。

需要随机数的项目已各自附带 LearnCpp 官方的 `Random.h`（自播种 Mersenne Twister，要求 **C++17**）。

## 目录

- [项目一览](#项目一览)
- [环境要求](#环境要求)
- [编译与运行](#编译与运行)
- [项目说明](#项目说明)
- [未收录的内容](#未收录的内容)
- [致谢](#致谢)

## 项目一览

| 目录 | 来源章节 | 类型 | 要点 |
| :--- | :--- | :--- | :--- |
| [`calculator/`](calculator/) | 9.5 | 工具 | 带输入校验的四则运算器 |
| [`hangman/`](hangman/) | 16.x | 游戏 | 猜单词（C++man） |
| [`potion-shop/`](potion-shop/) | 17.x Q2 | 游戏 | 药水商店：买药、管金币 |
| [`blackjack/`](blackjack/) | 17.x Q4 | 游戏 | 二十一点：洗牌、Ace 计分 |
| [`square-guess/`](square-guess/) | 20.7 | 游戏 | 平方数 × 随机乘数，逐个猜 |
| [`puzzle-15/`](puzzle-15/) | 21.x | 游戏 | 15 数字推盘，WASD 移动 |
| [`monster-hunter/`](monster-hunter/) | 24.x | 游戏 | 打怪升级 RPG，喝药水到 20 级 |

## 环境要求

- **C++17** 或更新（`Random.h` 依赖 inline 变量）
- **g++**（本仓库在 MinGW-w64 上验证通过）
- 终端 / PowerShell / cmd 均可

## 编译与运行

在任意项目目录下：

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o main.exe main.cpp
```

Windows 下直接运行：

```bat
main.exe
```

一次性编译全部项目（PowerShell）：

```powershell
Get-ChildItem -Directory | ForEach-Object {
    Push-Location $_.FullName
    Write-Host "Building $($_.Name) ..."
    g++ -std=c++17 -O2 -Wall -Wextra -o main.exe main.cpp
    if ($LASTEXITCODE -eq 0) { Write-Host "  OK" -ForegroundColor Green }
    else { Write-Host "  FAIL" -ForegroundColor Red }
    Pop-Location
}
```

## 项目说明

### calculator — 健壮计算器

对应教程 9.5：处理 `std::cin` 提取失败、多余输入、流关闭，以及除数为 0。

```
Enter a decimal number: 3
Enter one of the following: +, -, *, or /: +
Enter a decimal number: 4
3 + 4 is 7
```

### hangman — C++man 猜单词

从词库随机抽一个英文单词。猜对字母揭开，猜错消耗一次机会（屏幕上的 `+`）。机会用尽即负。

```
Welcome to C++man (a variant of Hangman)
The word: ____a___   Wrong guesses: +++++b
Enter your next letter:
```

### potion-shop — Roscoe 的药水铺

输入角色名，随机获得 80–120 金币。按编号购买 healing / mana / speed / invisibility，输入 `q` 结账离店。

```
Welcome to Roscoe's potion emporium!
0) healing costs 20
1) mana costs 30
2) speed costs 12
3) invisibility costs 50
```

### blackjack — 二十一点

标准 52 张牌，`std::shuffle` + `Random::mt` 洗牌。A 默认 11 点，爆牌时自动降为 1 点。庄家不到 17 必须要牌。

```
(h) to hit, or (s) to stand:
```

### square-guess — 平方数竞猜

指定起始整数和个数，程序生成连续平方再乘上 2–6 的随机乘数。逐个猜中这些数；猜错时提示最接近的正确答案。

```
Start where? 2
How many? 3
I generated 3 square numbers. Do you know what each number is after multiplying it by 4?
```

### puzzle-15 — 十五数字推盘

4×4 棋盘，空格旁的数字可滑入。随机打乱 1000 次，保证可解。

| 按键 | 动作 |
| :---: | :--- |
| `W` | 上 |
| `A` | 左 |
| `S` | 下 |
| `D` | 右 |
| `Q` | 退出 |

排成 `1 … 15` 且空格在右下角即胜。

### monster-hunter — 打怪升级

输入名字后遭遇 dragon / orc / slime。战斗或逃跑（50% 成功）；击杀升级、掉金币，30% 概率掉落血瓶 / 力量 / 毒药。升到 20 级获胜，血量归零失败。

```
(R)un or (F)ight:
```

## 未收录的内容

下列文件只是教程片段，不算独立小项目，因此没有放进本仓库：

| 原目录 | 原因 |
| :--- | :--- |
| `2_8/` | 多文件编译入门（`add` 函数声明） |
| `7_2/` | 命名空间 + 头文件示例 |
| `8_15/` | 仅有 `Random.h`，已按需复制到各游戏目录 |
| `17_x/3.cpp` | 空文件 |
| `21_x/` | `FixedPoint2` 定点类单元测试，不是可玩程序 |

## 致谢

源码来自 [LearnCpp](https://www.learncpp.com/) 综合测验与章节练习，中文对照见 [learncpp.cn](https://learncpp.cn/)。

`Random.h` 来自教程 [Global random numbers (Random.h)](https://www.learncpp.com/cpp-tutorial/global-random-numbers-random-h/)，可按原文许可自由再分发。
