#pragma once

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QSettings>

#include "qgraphicsbutton.h"

class ArchiveMod : public QObject
{
    Q_OBJECT
public:
    explicit ArchiveMod(int index, int scene_index, QPointF pos, QGraphicsScene *scene, QObject *parent = nullptr);

    void HideAndDestroy();

signals:
    void SaveArchive(int index);
    void LoadArchive(int index);

private:
    QGraphicsScene *scene_;
    const int kSceneIndex; // 调用场景scene的索引，0为开始菜单，1为游戏菜单调用
    const int kIndex;
    const QPointF kPos; // 第一个按钮的基准位置

    QGraphicsButton *button_;
    QGraphicsTextItem *desc_text_;

    void MouseClick();
};