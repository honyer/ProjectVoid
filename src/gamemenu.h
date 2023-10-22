#pragma once

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

class GameMenu : public QObject
{
    Q_OBJECT
public:
    // 顺序与GameEnd.h中的MenuButton一致
    enum MenuButton
    {
        Exit,
        Restart,
        Archive

    };

    GameMenu(QRectF rect, QGraphicsScene *scene, QObject *parent = nullptr);

    void Close();
signals:
    void ClickedMenuButton(int index);

private:
    QGraphicsScene *scene_;      // 主场景
    QRectF rect_;                // 矩形
    QGraphicsRectItem *bg_rect_; // 背景矩形

    void InitButtons();
    void ButtonSelected(MenuButton button);
};