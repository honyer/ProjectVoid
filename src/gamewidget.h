#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

class StartMenu;
class GameEngine;

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    GameWidget(QWidget* parent = nullptr);

private:
    const int kWidth;  // 窗口宽
    const int kHeight; // 窗口高

    QVBoxLayout* layout_;    // 布局
    StartMenu* startmenu_;   // 开始菜单
    GameEngine* gameengine_; // 游戏引擎

    int weapon_;  // 武器类型：默认0
    int archive_; // 存档编号：默认-1表示没有存档

    void InitStartMenu();                     // 初始化开始菜单
    void LaunchGame(int weapon, int archive); // 开始游戏
    void RestartGame();                       // 重启游戏
    void ExitGame();                          // 退出游戏
};
#endif // GAMEWIDGET_H
