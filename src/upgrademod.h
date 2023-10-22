#pragma once

#include <QObject>
#include <QGraphicsScene>

class Player;
class Weapon;
class QGraphicsButton;
class Log;

class UpgradeMod : public QObject
{
    Q_OBJECT
public:
    enum UpgradeType
    {
        Void = 0,     // ???: o(╥﹏╥)o
        Vitality,     // 活力: +2HP
        Giant,        // 巨人化: +4HP, +25%角色大小
        Dodge,        // 规避动作: +30%闪避
        Regeneration, // 自我修理: 每30s回复1HP

        HolloPoint,  // 空尖子弹: +25%伤害
        DoubleShot,  // 二重射击: +1子弹数量, -10%伤害
        HeavyBarrel, // 重型枪管: +40%伤害, -10%射速
        Reaper,      // 死神收割: +15%伤害, +1穿透

        FastHand,    // 快速射击: +15%射速
        Overload,    // 超载: +25%射速, +30%扩散
        LightBullet, // 轻型子弹: +20%射速, -10%伤害
        Barrage,     // 弹雨: 站立不动时+35%射速

        StreamLine,    // 流线构型: +30%子弹速度
        Sniper,        // 狙击: +15%子弹速度, -20%扩散
        TakeAim,       // 精确打击: -50%扩散, -10%射速
        Piercinground, // 穿甲子弹: +2穿透

        Haste,     // 急促脚步: +20%移动速度
        OverSpeed, // 超速驾驶: +25%移动速度, +20%扩散
        RunGun,    // RunN'Gun: +15%移动速度, +10%射速
        Windy,     // 风之行者: 若在15s内未受到伤害, +30%移动速度, +30%射速
        AutoFire   // 自动射击: 鼠标右键，用了的都说好！

    };

    explicit UpgradeMod(int index, QPointF pos, UpgradeType type, Player *player, Weapon *weapon, QGraphicsScene *scene, QObject *parent = nullptr);

    static QList<UpgradeMod::UpgradeType> GetRandomUpgradeMods(); // 获取随机升级选项

    static const QList<UpgradeMod::UpgradeType> kUniqueList; // 只能升级一次的选项
    static QList<UpgradeMod::UpgradeType> upgraded_mods;     // 已升级的选项

    void Close(); // 关闭升级选项

signals:
    void UpgradeModSelected(UpgradeType kType);

private:
    Log &log_;
    QGraphicsScene *scene_; // 游戏场景
    Player *player_;        // 玩家
    Weapon *weapon_;        // 武器

    const int kIndex;         // 位置索引
    const QPointF kPos;       // 图标按钮位置
    const UpgradeType kType;  // 升级类型
    QGraphicsButton *button_; // 按钮
    QGraphicsTextItem *desc_; // 描述

    void InitUpgradeMod(); // 初始化升级选项
    void MouseClick();     // 鼠标点击该项目
};