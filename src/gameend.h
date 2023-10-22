#pragma once

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

class QGraphicsButton;

class GameEnd : public QObject
{
    Q_OBJECT
public:
    // 顺序与GameMenu.h中的MenuButton一致
    enum MenuButton
    {
        Exit,
        Restart

    };

    GameEnd(bool win, QRectF rect, QGraphicsScene *scene, QObject *parent = nullptr);

    void Close();
signals:
    void ClickedMenuButton(int index);

private:
    QGraphicsScene *scene_; // 主场景
    QRectF rect_;           // 矩形
    bool game_win_;         // 游戏是否胜利

    QGraphicsRectItem *bg_rect_; // 背景矩形

    void InitText();    // 设置提示文字
    void InitButtons(); // 设置按钮
    void ButtonSelected(int index);
};