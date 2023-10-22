#pragma once

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsTextItem>

#include "qgraphicsbutton.h"

class WeaponMod : public QObject
{
    Q_OBJECT
public:
    enum WeaponType
    {
        Smg = 0, // 冲锋枪: 高射速, 突突突突突突就完事了
        Smart,   // 智慧手枪MKⅡ: 自动索敌. 才不是在致敬隔壁ttf2呢!
        Shotgun, // 霰弹枪: 打出一片弹丸雨, 低射速, 高伤害
        Dmr      // 射手步枪: 高精度, 高伤害. 一枪一个!
    };

    static const QList<WeaponType> kWeaponTypeList;

    explicit WeaponMod(int index, QGraphicsScene *scene, QObject *parent = nullptr);

    void ResetSelected();

signals:
    void WeaponSelected(int index);

private:
    QGraphicsScene *scene_;
    QGraphicsButton *button_;
    QGraphicsTextItem *weapon_desc_;

    const int kIndex;   // 武器索引

    void InitWeaponMod(WeaponType type);
    void HoverEnter();
    void HoverLeave();
    void MouseClick();
};
