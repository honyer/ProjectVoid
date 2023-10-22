#include "weaponmod.h"

const QList<WeaponMod::WeaponType> WeaponMod::kWeaponTypeList = {WeaponMod::Smg, WeaponMod::Smart,
                                                                 WeaponMod::Shotgun, WeaponMod::Dmr};
WeaponMod::WeaponMod(int index, QGraphicsScene *scene, QObject *parent)
    : QObject(parent), kIndex(index), scene_(scene)
{
    weapon_desc_ = new QGraphicsTextItem();
    weapon_desc_->setDefaultTextColor(Qt::white);
    weapon_desc_->setFont(QFont("Fusion Pixel Proportional", 16));

    InitWeaponMod(kWeaponTypeList.at(kIndex));

    connect(button_, &QGraphicsButton::HoverEnter, this, &WeaponMod::HoverEnter);
    connect(button_, &QGraphicsButton::HoverLeave, this, &WeaponMod::HoverLeave);
    connect(button_, &QGraphicsButton::MouseClick, this, &WeaponMod::MouseClick);
}

void WeaponMod::ResetSelected()
{
    button_->ResetSelected();
}

void WeaponMod::InitWeaponMod(WeaponType type)
{
    QString icon_path;            // 图标路径
    QString desc_text;            // 描述文本
    QPointF base_pos(102.4, 160); // 第一个图标位置
    qreal excurse_x = 294.4;      // 图标间隔偏移量
    QPointF pos;

    switch (type)
    {
    case WeaponMod::Smg:
        icon_path = ":/assets/images/icons/weapons/ic_smg";
        desc_text = "冲锋枪: 高射速, 突突突突突突就完事了";
        break;
    case WeaponMod::Smart:
        icon_path = ":/assets/images/icons/weapons/ic_smart";
        desc_text = "智慧手枪MKⅡ: 自动索敌. 才不是在致敬隔壁ttf2呢!";
        break;
    case WeaponMod::Shotgun:
        icon_path = ":/assets/images/icons/weapons/ic_shotgun";
        desc_text = "霰弹枪: 打出一片弹丸雨, 低射速, 高伤害";
        break;
    case WeaponMod::Dmr:
        icon_path = ":/assets/images/icons/weapons/ic_dmr";
        desc_text = "射手步枪: 高精度, 高伤害. 一枪一个!";
        break;
    default:
        break;
    }

    pos = QPointF(base_pos.x() + excurse_x * kIndex, base_pos.y());
    button_ = new QGraphicsButton(icon_path, pos, this);
    button_->setScale(6);

    weapon_desc_->setPlainText(desc_text);
    weapon_desc_->setPos(640 - weapon_desc_->boundingRect().width() / 2, base_pos.y() + 180);

    scene_->addItem(button_);
}

void WeaponMod::HoverEnter()
{
    scene_->addItem(weapon_desc_);
}

void WeaponMod::HoverLeave()
{
    scene_->removeItem(weapon_desc_);
}

void WeaponMod::MouseClick()
{
    emit WeaponSelected(kIndex);
}
