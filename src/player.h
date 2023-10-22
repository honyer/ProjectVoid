#pragma once

#include <QGraphicsPixmapItem>
#include <QVector2D>
#include <QSoundEffect>
#include <QGraphicsView>
#include <QMovie>
#include <QTimer>

class Map;
class Log;

class Player : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Player(int width, int height, QGraphicsView *view, QWidget *parent = nullptr);

    void LoadPlayer();          // 加载玩家模型
    void UpdatePosition();      // 更新玩家位置
    QPointF GetMovedPosition(); // 获取移动了的差量

    void AddVelocity(int dx, int dy); // 添加速度
    QPoint GetVelocity();             // 获取速度向量
    void SetVelocityX(int dx);        // 设置x速度
    void SetVelocityY(int dy);        // 设置y速度
    bool GetStandStill();             // 获取静止状态

    int GetHealth();            // 获取生命值
    void SetHealth(int health); // 设置生命值

    int GetMaxHealth();                // 获取最大生命值
    void SetMaxHealth(int max_health); // 设置最大生命值

    qreal GetSpeedBoost();                 // 获取速度加成
    void SetSpeedBoost(qreal speed_boost); // 设置速度加成

    int GetLevel();           // 获取等级
    void SetLevel(int level); // 设置等级
    int GetExp();             // 获取经验值
    void SetExp(int exp);     // 设置经验值

    double GetPickupRange(); // 获取拾取范围
    void PickupExp(int exp); // 拾取经验

    qreal GetDodge();           // 获取闪避值
    void SetDodge(qreal dodge); // 设置闪避值

    qreal GetInvincibleTime();                     // 获取无敌时间
    void SetInvincibleTime(qreal invincible_time); // 设置无敌时间
    void ResetInvincible();                        // 重置无敌状态

    bool GetRegenerate();                // 获取再生状态
    void SetRegenerate(bool regenerate); // 再生状态设置
    void Regenerate();                   // 再生
    void Hurt();                         // 受到攻击

    bool GetWindy();           // 获取风之行者状态
    void SetWindy(bool windy); // 设置风之行者状态
    bool GetWindyOnState();    // 风之行者开启状态
    void OnWindyState();       // 风之行者开启

    bool GetAutoFire();               // 获取自动开火状态
    void SetAutoFire(bool auto_fire); // 设置自动开火状态

signals:
    void PlayerHurt();                 // 玩家受到攻击
    void PlayerDeadth();               // 玩家死亡
    void PlayerRegenerate();           // 玩家再生
    void PlayerLevelup();              // 玩家升级,传递给主窗口和武器
    void PlayerExpPicked(qreal ratio); // 玩家当前等级经验比例
protected:
private:
    Log &log; // 日志
    Map &map; // 地图

    QGraphicsView *main_view_;      // 游戏场景
    const int kWidth;               // 窗口宽度
    const int kHeight;              // 窗口高度
    QGraphicsTextItem *health_bar_; // 血条
    QSoundEffect *hurt_sound;       // 受击音效
    QSoundEffect *levelup_sound;    // 升级音效
    QMovie *mv_idle_;               // 待机状态动画
    QMovie *mv_run_;                // 运动状态动画

    QVector2D velocity_;       // 玩家速度
    int level_;                // 等级
    qreal level_ratio_;        // 当前等级经验比例
    int max_level_;            // 最大等级
    int health_;               // 生命值
    int max_health_;           // 最大生命值
    int speed_;                // 基础移动速度
    qreal speed_boost_;        // 移动速度加成
    int exp_;                  // 经验值
    double pickup_range_;      // 拾取范围
    qreal dodge_;              // 闪避值
    bool invincible_;          // 无敌状态
    qreal invincible_time_;    // 无敌时间
    QTimer *invincible_timer_; // 无敌计时器
    bool regenerate_;          // 再生状态
    int regenerate_count_;     // 再生计数
    bool windy_;               // 风之行者状态
    int windy_count_;          // 风之行者计数，开启加速状态
    bool windy_on_;            // 风之行者开启状态
    bool auto_fire_;           // 自动开火状态

    void InitAttributes();      // 初始化属性
    QPointF NewValidPosition(); // 新的有效位置
};
