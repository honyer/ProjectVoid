#pragma once

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QSoundEffect>
#include <QTimer>

#include "player.h"

class Map;

class Enemy : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    enum EnemyType
    {
        Bot,
        BotElite,
        Explosive,
        Float,
        FloatElite
    };

    explicit Enemy(int index, Player *player, QGraphicsScene *scene, QObject *parent = nullptr);

    int GetType();                       // 敌人类型
    qreal GetHealth();                   // 获取敌人血量
    void Hurt(qreal damage);             // 被子弹命中的伤害
    void HurtWithoutSound(qreal damage); // 其他不播放音效的伤害

    void LoadEnemy();         // 加载敌人
    void Move();              // 敌人移动
    void CollideWithPlayer(); // 碰撞角色检测
    void Shoot();             // 敌人发射子弹

signals:
    void EnemyDeath();

private:
    Map &map;                    // 地图
    QGraphicsScene *main_scene_; // 主场景
    Player *player_;             // 角色
    QTimer *timer_;              // 全局计时器
    QMovie *movie_;              // 敌人动画
    QPixmap enemy_img_;          // 敌人图片
    QLineF line_;                // 敌人与玩家的连线
    QSoundEffect *bullet_sound;  // 敌人被子弹命中音效
    QSoundEffect *explode_sound; // 敌人爆炸音效

    const int kIndex;    // 敌人索引
    qreal health_;       // 敌人血量
    qreal speed_;        // 敌人移动速度
    int xp_;             // 敌人经验
    QTimer *hurt_timer_; // 敌人受伤计时器
    bool attacked_;      // 是否被攻击
    bool can_fly_;       // 是否会浮空，无视障碍
    bool can_explode_;   // 是否会爆炸
    bool can_shoot_;     // 是否能够发射子弹

    int round_;            // 子弹数量
    int shoot_rate_;       // 发射频率
    int shoot_rate_count_; // 发射计数
    int shoot_distance_;   // 发射距离
    qreal bullet_speed_;   // 子弹速度

    QMovie *explosion_movie_; // 爆炸动画
    qreal explosion_range_;   // 爆炸范围
    qreal explosion_damage_;  // 爆炸伤害

    void MainHandle(); // 主循环

    void InitEnemy();             // 初始化敌人
    void InitAttributes();        // 初始化敌人属性
    void HurtReload();            // 敌人受伤重置
    void Death();                 // 敌人死亡
    QPointF GenerateXpPosition(); // 生成经验位置
    void Explode();               // 敌人爆炸
    void ExplodedCheck();         // 爆炸检测
};