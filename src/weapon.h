#pragma once

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QSoundEffect>

#include "player.h"

class Weapon : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Weapon(int index, Player *player, QGraphicsScene *scene, QWidget *parent = nullptr);

    void LoadWeapon();                       // 加载武器模型
    void UpdatePosition();                   // 更新武器位置
    void RotateToCursor(const QPointF &pos); // 旋转到鼠标方向
    void Fire();                             // 开火
    void FireOn();                           // 开火
    void FireOff();                          // 关火

    qreal GetFireRateBoost();           // 获取发射频率加成
    void SetFireRateBoost(qreal boost); // 设置发射频率加成

    qreal GetSpeedBoost();           // 获取子弹速度加成
    void SetSpeedBoost(qreal boost); // 设置子弹速度加成

    qreal GetDamageBoost();           // 获取伤害加成
    void SetDamageBoost(qreal boost); // 设置伤害加成

    qreal GetSpreadBoost();           // 获取子弹扩散加成
    void SetSpreadBoost(qreal boost); // 设置子弹扩散加成

    int GetRoundBoost();           // 获取子弹弹道数量
    void SetRoundBoost(int boost); // 设置子弹弹道数量

    int GetPenetrationBoost();           // 获取子弹穿透加成
    void SetPenetrationBoost(int boost); // 设置子弹穿透加成

    bool GetBarrage();             // 获取弹雨状态
    void SetBarrage(bool barrage); // 设置弹雨状态
    bool GetAutoFire();            // 获取自动开火状态
    void SwitchAutoFire();         // 自动开火开关

private:
    const int kIndex;            // 武器索引
    QGraphicsScene *main_scene_; // 主场景
    Player *player_;             // 玩家
    QSoundEffect *fire_sound;    // 开枪音效
    int sfx_play_count_;         // 音效播放计数
    QPixmap weapon_img_;         // 武器图片

    // 武器属性
    qreal fire_rate_;       // 发射频率
    qreal fire_rate_boost_; // 发射频率加成
    int fire_rate_count_;   // 发射计数
    qreal damage_;          // 伤害
    qreal damage_boost_;    // 伤害加成
    qreal speed_;           // 子弹速度
    qreal speed_boost_;     // 子弹速度加成
    int round_;             // 子弹数量
    int round_boost_;       // 子弹数量加成
    qreal spread_;          // 子弹扩散
    qreal spread_boost_;    // 子弹扩散加成
    int penetration_;       // 子弹穿透
    int penetration_boost_; // 子弹穿透加成
    bool barrage_;          // 弹雨
    bool fire_switch_;      // 武器开关
    bool auto_fire_;        // 自动开火

    void InitWeapon();        // 初始化武器
    void InitAttributes();    // 初始化武器属性
    QString GetWeaponImage(); // 获取武器图片
    void LevelUp();           // 升级武器
};
