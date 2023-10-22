#include "bullet.h"
#include "player.h"
#include "enemy.h"
#include "globaltimer.h"

Bullet::Bullet(int type, QPointF pos, qreal angle, qreal speed, qreal damage, int penetration, QGraphicsScene *scene, Player *player, QObject *parent)
    : QObject(parent), kIndex(type), pos_(pos), angle_(angle), kSpeed(speed), kDamage(damage), kPenetration(penetration), main_scene_(scene), player_(player)
{
    // GlobalTimer &gtimer = GlobalTimer::GetInstance();
    timer_ = GlobalTimer::GetInstance().GetTimer();
    // timer->setInterval(10);
    // timer->start();
    // connect(&gtimer, &GlobalTimer::MainTimerStarted, this, [=]()
    //         { timer->start(); });
    // connect(&gtimer, &GlobalTimer::MainTimerStopped, this, [=]()
    //         { timer->stop(); });
    connect(timer_, &QTimer::timeout, this, &Bullet::MainHandler);

    this->setPos(pos_);
    this->setRotation(angle_);

    InitBullet();
}
int Bullet::GetType()
{
    return kIndex;
}
qreal Bullet::GetDamage()
{
    return kDamage;
}
void Bullet::MainHandler()
{
    Move();
    TrackEnemy();
}
void Bullet::InitBullet()
{
    hurt_count_ = 0;
    setPixmap(QPixmap(GetBulletImage()));
    setScale(3);
    setTransformOriginPoint(boundingRect().center());

    switch (kIndex)
    {
    case SmgBullet:

        break;
    case SmartBullet:

        break;
    case ShotgunBullet:
        break;
    case DmrBullet:
        break;
    case EnemyBullet:

        this->setScale(2);
        this->setZValue(114);
        break;
    default:
        break;
    }
}

QString Bullet::GetBulletImage()
{
    switch (kIndex)
    {
    case SmgBullet:
        return ":/assets/images/weapon/wp_smg_bullet.png";
    case SmartBullet:
        return ":/assets/images/weapon/wp_smart_bullet.png";
    case ShotgunBullet:
        return ":/assets/images/weapon/wp_shotgun_bullet.png";
    case DmrBullet:
        return ":/assets/images/weapon/wp_dmr_bullet.png";
    case EnemyBullet:
        return ":/assets/images/enemy/en_bullet.png";
    default:
        return "";
    }
}

void Bullet::Move()
{
    TrackEnemy();
    qreal angle = rotation();
    qreal radian = qDegreesToRadians(angle);

    qreal dx = kSpeed * qCos(radian);
    qreal dy = kSpeed * qSin(radian);
    this->moveBy(dx, dy);

    // 将超过视野范围的子弹删除
    int out_of_range = 800;
    qreal distance = QLineF(this->pos(), player_->pos()).length();
    if (distance > out_of_range)
    {
        main_scene_->removeItem(this);
        this->deleteLater();
        return;
    }
    if (kIndex == EnemyBullet)
    {
        CollideWithPlayer();
        return;
    }

    CollideWithEnemy();
}

void Bullet::CollideWithEnemy()
{
    if (kIndex == EnemyBullet)
        return;

    QList<QGraphicsItem *> colliding_items = this->collidingItems();

    // 子弹穿透碰撞判断
    for (auto item : colliding_items)
    {
        Enemy *enemy = dynamic_cast<Enemy *>(item);
        if (enemy)
        {
            if (enemy == last_hit_enemy_)
            {
                continue;
            }
            enemy->Hurt(kDamage);
            hurt_count_++;
            last_hit_enemy_ = enemy;
            if (hurt_count_ >= kPenetration)
            {
                main_scene_->removeItem(this);
                this->deleteLater();
                return;
            }
        }
    }
}

void Bullet::TrackEnemy()
{
    if (kIndex != SmartBullet)
        return;

    // 获取场景敌人列表
    QList<QGraphicsItem *> item_list = main_scene_->items();
    QList<Enemy *> enemy_list;
    for (auto item : item_list)
    {
        Enemy *enemy = dynamic_cast<Enemy *>(item);
        if (enemy)
            enemy_list.append(enemy);
    }

    // 搜寻最近的敌人
    qreal min_distance = 5000;
    Enemy *target = nullptr;
    for (auto enemy : enemy_list)
    {
        qreal distance = QLineF(this->pos(), enemy->pos()).length();
        if (distance < min_distance)
        {
            min_distance = distance;
            target = enemy;
        }
    }

    if (!target)
        return;

    qreal target_angle = QLineF(this->pos(), target->pos()).angle();
    qreal bullet_angle = this->rotation();

    qreal angle = target_angle + bullet_angle;
    while (angle > 180)
        angle -= 360;
    while (angle < -180)
        angle += 360;

    qreal angle_diff = this->rotation() - angle;
    if (angle > 4)
    {
        angle_diff = this->rotation() - 4;
    }
    else if (angle < -4)
    {
        angle_diff = this->rotation() + 4;
    }
    this->setRotation(angle_diff);
}

void Bullet::CollideWithPlayer()
{
    if (kIndex != EnemyBullet)
        return;
    QLineF line = QLineF(this->pos(), player_->pos());
    if (line.length() > 20)
        return;

    if (this->collidesWithItem(player_))
    {
        player_->Hurt();
        main_scene_->removeItem(this);
        this->deleteLater();
        return;
    }
}

void Bullet::Close()
{
    main_scene_->removeItem(this);
    this->deleteLater();
}
