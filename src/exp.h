#pragma once

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>

class Player;

class Exp : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Exp(int xp, QPointF pos, Player *player, QGraphicsScene *scene, QObject *parent = nullptr);

    void CheckPickUp();

private:
    QTimer *timer;
    QGraphicsScene *main_scene_;
    Player *player_;
    QPointF pos_;
    const int kXp;
};