#pragma once

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>

class Player;
class Enemy;

class Bullet : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    // 子弹类型
    enum BulletType
    {
        SmgBullet,
        SmartBullet,
        ShotgunBullet,
        DmrBullet,
        EnemyBullet
    };
    Bullet(int index, QPointF pos, qreal angle, qreal speed, qreal damage, int penetration,
           QGraphicsScene *scene, Player *player, QObject *parent = nullptr);

    int GetType();     // 获取子弹类型
    qreal GetDamage(); // 获取子弹伤害

private:
    QGraphicsScene *main_scene_; // 主场景
    Player *player_;             // 角色
    QTimer *timer_;              // 全局计时器

    // QMutex *mutex_;                    // 互斥锁
    // std::atomic<bool> deleted_{false}; // 是否被删除
    // Enemy *enemy_;

    const int kIndex;       // 子弹类型
    QPointF pos_;           // 子弹位置
    qreal angle_;           // 子弹角度
    const qreal kSpeed;     // 子弹速度
    const qreal kDamage;    // 子弹伤害
    const int kPenetration; // 子弹穿透
    int hurt_count_;        // 伤害计数

    Enemy *last_hit_enemy_; // 上一次子弹命中的敌人

    void MainHandler();       // 主计时器处理
    void InitBullet();        // 初始化子弹
    QString GetBulletImage(); // 获取子弹图片
    void Move();              // 子弹移动
    void CollideWithEnemy();  // 碰撞敌人检测
    void TrackEnemy();        // 跟踪敌人
    void CollideWithPlayer(); // 碰撞角色检测
    void Close();             // 关闭子弹
};