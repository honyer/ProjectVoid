#include "exp.h"
#include "player.h"
#include "map.h"
#include "globaltimer.h"

Exp::Exp(int xp, QPointF pos, Player *player, QGraphicsScene *scene, QObject *parent)
    : QObject(parent), main_scene_(scene), player_(player), pos_(pos), kXp(xp)
{
    timer = GlobalTimer::GetInstance().GetTimer();
    this->setPixmap(QPixmap(":/assets/images/ic_energyunit.png"));
    this->setPos(pos_);
    this->setZValue(1);
    this->setScale(3);

    connect(timer, &QTimer::timeout, this, &Exp::CheckPickUp);
}

void Exp::CheckPickUp()
{
    QLineF line = QLineF(this->pos(), player_->pos());

    if (line.length() > player_->GetPickupRange())
        return;

    qreal radian = line.angle() * M_PI / 180;
    qreal dx = cos(radian) * 5;
    qreal dy = -sin(radian) * 5;
    moveBy(dx, dy);

    if (this->collidesWithItem(player_))
    {
        player_->PickupExp(kXp);
        main_scene_->removeItem(this);
        delete this;
    }
}