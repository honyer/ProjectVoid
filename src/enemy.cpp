#include "enemy.h"
#include "map.h"
#include "bullet.h"
#include "exp.h"
#include "globaltimer.h"
#include "soundplayer.h"

#include <QtConcurrent/QtConcurrent>

using namespace QtConcurrent;

Enemy::Enemy(int index, Player *player, QGraphicsScene *scene, QObject *parent)
    : QObject(parent), main_scene_(scene), player_(player), kIndex(index), map(Map::GetInstance())
{
    GlobalTimer &gtimer = GlobalTimer::GetInstance();
    // timer_ = GlobalTimer::GetInstance().GetTimer();
    timer_ = new QTimer(this);
    timer_->setInterval(10);
    timer_->start();
    connect(&gtimer, &GlobalTimer::MainTimerStarted, this, [=]()
            { timer_->start(); });
    connect(&gtimer, &GlobalTimer::MainTimerStopped, this, [=]()
            { timer_->stop(); });
    connect(timer_, &QTimer::timeout, this, &Enemy::MainHandle);

    hurt_timer_ = new QTimer(this);
    connect(hurt_timer_, &QTimer::timeout, this, &Enemy::HurtReload);

    bullet_sound = SoundPlayer::GetInstance().GetBulletSound();
    explode_sound = SoundPlayer::GetInstance().GetExplodeSound();

    InitAttributes();
    InitEnemy();
}

void Enemy::Hurt(qreal damage)
{
    health_ -= damage;
    bullet_sound->play();

    attacked_ = true;
    hurt_timer_->start(100);

    if (health_ <= 0)
        Death();
}

int Enemy::GetType()
{
    return kIndex;
}

void Enemy::HurtWithoutSound(qreal damage)
{
    health_ -= damage;
    if (health_ <= 0)
        Death();
}

qreal Enemy::GetHealth()
{
    return health_;
}

void Enemy::MainHandle()
{

    Move();
    LoadEnemy();
    Shoot();
    CollideWithPlayer();
}

void Enemy::InitEnemy()
{
    movie_ = nullptr;
    explosion_movie_ = nullptr;
    line_ = QLineF(QPointF(0, 0), player_->pos());
    qreal scale = 3;

    switch (kIndex)
    {
    case Bot:
        movie_ = new QMovie(":/assets/images/enemy/en_bot.gif");
        break;
    case BotElite:
        movie_ = new QMovie(":/assets/images/enemy/en_bot_elite.gif");
        scale = 4.5;
        break;
    case Explosive:
        enemy_img_ = QPixmap(":/assets/images/enemy/en_explosive.png");
        explosion_movie_ = new QMovie(":/assets/images/fx/fx_explosion.gif");
        explosion_movie_->setParent(this);
        scale = 4;
        break;
    case Float:
        enemy_img_ = QPixmap(":/assets/images/enemy/en_float.png");
        scale = 5;
        break;
    case FloatElite:
        enemy_img_ = QPixmap(":/assets/images/enemy/en_float_elite.png");
        scale = 6;
        break;
    default:
        break;
    }

    if (movie_)
    {
        movie_->setParent(this);
        movie_->setSpeed(50);
        movie_->start();
        LoadEnemy();
    }
    else
    {
        this->setPixmap(enemy_img_);
    }

    this->setScale(scale);
    this->setTransformOriginPoint(boundingRect().center());
    this->setParentItem(player_->parentItem());
}

void Enemy::InitAttributes()
{
    health_ = 30;
    speed_ = 1;
    xp_ = 1;
    attacked_ = false;
    can_fly_ = false;
    can_explode_ = false;
    can_shoot_ = false;

    round_ = 1;
    shoot_rate_ = 200;
    shoot_rate_count_ = 0;
    shoot_distance_ = 450;
    bullet_speed_ = 3;

    explosion_range_ = 100;
    explosion_damage_ = 50;

    switch (kIndex)
    {
    case Bot:
        break;
    case BotElite:
        health_ = 50;
        speed_ = 1.8;
        xp_ = 3;
        break;
    case Explosive:
        health_ = 40;
        can_explode_ = true;
        break;
    case Float:
        health_ = 40;
        xp_ = 2;
        speed_ = 1.5;
        can_fly_ = true;
        can_shoot_ = true;
        break;
    case FloatElite:
        health_ = 70;
        speed_ = 1.3;
        xp_ = 5;
        round_ = 3;
        can_fly_ = true;
        can_shoot_ = true;
        shoot_distance_ += 100;
        shoot_rate_ += 100;
        break;
    default:
        break;
    }
}

void Enemy::LoadEnemy()
{
    line_ = QLineF(this->pos(), player_->pos());
    qreal distance = qAbs(line_.length());
    if (distance > 640 + 10)
        return;

    QImage img;
    if (movie_)
    {
        img = movie_->currentImage();
    }
    else
    {
        img = enemy_img_.toImage();
    }

    // 受到攻击时，图片变白
    if (attacked_)
    {
        uchar *pixels = img.bits();
        int width = img.width();
        int height = img.height();
        int bytesPerLine = img.bytesPerLine();
        for (int y = 0; y < height; ++y)
        {
            uchar *line = pixels + y * bytesPerLine;
            for (int x = 0; x < width; ++x)
            {
                uchar alpha = line[x * 4 + 3];
                if (alpha != 0)
                {
                    line[x * 4] = 255;
                    line[x * 4 + 1] = 255;
                    line[x * 4 + 2] = 255;
                }
            }
        }
    }

    qreal angle = qAbs(line_.angle());
    if (angle > 90 && angle < 270)
        img = img.mirrored(true, false);
    this->setPixmap(QPixmap::fromImage(img));
}

void Enemy::Move()
{
    line_ = QLineF(this->pos(), player_->pos());
    qreal radians = qDegreesToRadians(line_.angle());
    qreal dx = qCos(radians) * speed_;
    qreal dy = -qSin(radians) * speed_;

    QPointF new_pos = this->pos() + QPointF(dx, dy);

    // 飞行敌人保持攻击距离
    if (can_fly_)
    {
        if (line_.length() < shoot_distance_ - 100)
        {
            new_pos = this->pos() + QPointF(-dx, -dy);
        }

        this->setPos(new_pos);
        return;
    };

    // 地面敌人不能穿越障碍物
    if (map.IsInBarriers(new_pos))
    {
        new_pos -= QPointF(speed_, 0);
        if (map.IsInBarriers(new_pos))
        {
            new_pos -= QPointF(0, speed_);
            if (map.IsInBarriers(new_pos))
            {
                new_pos = this->pos();
            }
        }
    }

    this->setPos(new_pos);
}

void Enemy::HurtReload()
{
    attacked_ = false;
    hurt_timer_->stop();
}

void Enemy::CollideWithPlayer()
{
    qreal distance = qAbs(line_.length());
    if (distance > 100)
        return;
    if (this->collidesWithItem(player_))
    {
        player_->Hurt();
        // 敌人撞到玩家，也会受到伤害
        qreal damage = qMax(20.0, health_ / 2);
        this->HurtWithoutSound(damage);
    }
}

void Enemy::Shoot()
{
    if (!can_shoot_)
        return;
    shoot_rate_count_++;
    qreal distance = qAbs(line_.length());
    if (distance > shoot_distance_ || shoot_rate_count_ < shoot_rate_)
        return;

    shoot_rate_count_ = 0;
    qreal angle = -line_.angle();
    int gap = 15;
    if (round_ > 1)
        angle -= int((round_ - 1) / 2) * gap;
    for (int i = 0; i < round_; i++)
    {
        Bullet *bullet = new Bullet(4, this->pos(), angle + gap * i, bullet_speed_, explosion_damage_,
                                    explosion_range_, main_scene_, player_);
        bullet->setParentItem(this->parentItem());
    }
}

void Enemy::Death()
{
    disconnect(timer_, &QTimer::timeout, this, &Enemy::MainHandle);

    QPointF xp_pos = GenerateXpPosition();
    Exp *exp = new Exp(xp_, xp_pos, player_, main_scene_);
    exp->setParentItem(this->parentItem());

    if (kIndex == Explosive)
    {
        Explode();
        return;
    }
    emit EnemyDeath();
    main_scene_->removeItem(this);
    this->deleteLater();
}

void Enemy::Explode()
{
    explode_sound->play();
    explosion_movie_->start();
    this->setPixmap(explosion_movie_->currentPixmap());
    this->setTransformOriginPoint(boundingRect().center());

    connect(explosion_movie_, &QMovie::frameChanged, this, [=]()
            { this->setPixmap(explosion_movie_->currentPixmap()); });
    connect(explosion_movie_, &QMovie::finished, this, &Enemy::ExplodedCheck);
}

void Enemy::ExplodedCheck()
{
    QList<QGraphicsItem *> items = main_scene_->items();

    // 爆炸范围内的敌人和玩家都受到伤害
    for (auto item : items)
    {
        qreal distance = QLineF(this->pos(), item->pos()).length();
        if (distance > explosion_range_ || item == this)
            continue;
        Enemy *enemy = dynamic_cast<Enemy *>(item);
        if (enemy)
        {
            enemy->HurtWithoutSound(explosion_damage_);
        }
    }
    qreal distance = QLineF(this->pos(), player_->pos()).length();
    if (distance < explosion_range_)
    {
        player_->Hurt();
    }

    emit EnemyDeath();
    main_scene_->removeItem(this);
    this->deleteLater();
}

QPointF Enemy::GenerateXpPosition()
{
    QPointF pos = this->pos();

    // 不在地图边界外生成经验
    while (!map.IsInPlayground(pos))
    {
        if (pos.x() < map.PlayBounds().left())
        {
            pos.setX(pos.x() + 10);
        }
        else if (pos.x() > map.PlayBounds().right())
        {
            pos.setX(pos.x() - 10);
        }

        if (pos.y() < map.PlayBounds().top())
        {
            pos.setY(pos.y() + 5);
        }
        else if (pos.y() > map.PlayBounds().bottom())
        {
            pos.setY(pos.y() - 5);
        }
    }

    return pos;
}
