#include "upgrademod.h"
#include "player.h"
#include "weapon.h"
#include "log.h"
#include "qgraphicsbutton.h"
#include <QRandomGenerator>

const QList<UpgradeMod::UpgradeType> UpgradeMod::kUniqueList = {
    UpgradeMod::Giant,
    UpgradeMod::Dodge,
    UpgradeMod::Regeneration,
    UpgradeMod::DoubleShot,
    UpgradeMod::Barrage,
    UpgradeMod::Windy,
    UpgradeMod::AutoFire};

QList<UpgradeMod::UpgradeType> UpgradeMod::upgraded_mods = {};

UpgradeMod::UpgradeMod(int index, QPointF pos, UpgradeType type, Player *player, Weapon *weapon, QGraphicsScene *scene, QObject *parent)
    : QObject(parent), scene_(scene), player_(player), weapon_(weapon),
      kType(type), kPos(pos), kIndex(index), log_(Log::GetInstance())
{
    InitUpgradeMod();
    connect(button_, &QGraphicsButton::MouseClick, this, &UpgradeMod::MouseClick);
}

QList<UpgradeMod::UpgradeType> UpgradeMod::GetRandomUpgradeMods()
{
    QList<UpgradeType> all_upgrades = {
        Void, Vitality, Giant, Dodge, Regeneration,
        HolloPoint, DoubleShot, HeavyBarrel, Reaper,
        FastHand, Overload, LightBullet, Barrage,
        StreamLine, Sniper, TakeAim, Piercinground,
        Haste, OverSpeed, RunGun, Windy, AutoFire};

    QList<UpgradeType> upgrades;
    // 排除已经升级过只能升级一次的选项
    for (const auto &upgraded : upgraded_mods)
    {
        if (kUniqueList.contains(upgraded))
            all_upgrades.removeAll(upgraded);
    }

    // 循环获取不重复的随机数
    QList<int> random_indexes;
    while (random_indexes.size() < 3)
    {
        int index = QRandomGenerator::global()->bounded(all_upgrades.size());
        if (!random_indexes.contains(index))
            random_indexes.append(index);
    }

    // random_indexes = {0, 1, 2};
    // 取出这些随机数对应的升级选项
    for (int i = 0; i < 3; i++)
    {
        int index = random_indexes.at(i);
        upgrades.append(all_upgrades.at(index));
    }

    // 重要的升级选项，开自动！
    if (!upgrades.contains(AutoFire) && !upgraded_mods.contains(AutoFire))
    {
        upgrades.removeLast();
        upgrades.append(AutoFire);
    }

    return upgrades;
}

void UpgradeMod::Close()
{
    scene_->removeItem(button_);
    scene_->removeItem(desc_);
    delete button_;
    delete desc_;
    delete this;
}

void UpgradeMod::InitUpgradeMod()
{

    QString icon_path;
    QString desc_text;

    switch (kType)
    {
    case UpgradeMod::Void:
        icon_path = ":/assets/images/icons/upgrades/u_void";
        desc_text = "???: o(╥﹏╥)o";
    case UpgradeMod::Vitality:
        icon_path = ":/assets/images/icons/upgrades/u_vitality";
        desc_text = "活力: +2HP";
        break;
    case UpgradeMod::Giant:
        icon_path = ":/assets/images/icons/upgrades/u_giant";
        desc_text = "巨人化: +4HP, +25%角色大小";
        break;
    case UpgradeMod::Dodge:
        icon_path = ":/assets/images/icons/upgrades/u_dodge";
        desc_text = "规避动作: +30%闪避";
        break;
    case UpgradeMod::Regeneration:
        icon_path = ":/assets/images/icons/upgrades/u_regeneration";
        desc_text = "自我修理: 每30s回复1HP";
        break;
    case UpgradeMod::HolloPoint:
        icon_path = ":/assets/images/icons/upgrades/u_hollowpoint";
        desc_text = "空尖子弹: +25%伤害";
        break;
    case UpgradeMod::DoubleShot:
        icon_path = ":/assets/images/icons/upgrades/u_doubleshot";
        desc_text = "双子弹: +25%伤害";
        break;
    case UpgradeMod::HeavyBarrel:
        icon_path = ":/assets/images/icons/upgrades/u_heavybarrel";
        desc_text = "重型枪管: +40%伤害, -10%射速";
        break;
    case UpgradeMod::Reaper:
        icon_path = ":/assets/images/icons/upgrades/u_reaper";
        desc_text = "死神收割: +15%伤害, +1穿透";
        break;
    case UpgradeMod::FastHand:
        icon_path = ":/assets/images/icons/upgrades/u_fasthand";
        desc_text = "快速射击: +15%射速";
        break;
    case UpgradeMod::Overload:
        icon_path = ":/assets/images/icons/upgrades/u_overload";
        desc_text = "超载: +25%射速, +30%扩散";
        break;
    case UpgradeMod::LightBullet:
        icon_path = ":/assets/images/icons/upgrades/u_lightbullet";
        desc_text = "轻型子弹: +20%射速, -10%伤害";
        break;
    case UpgradeMod::Barrage:
        icon_path = ":/assets/images/icons/upgrades/u_barrage";
        desc_text = "弹雨: 站立不动时+35%射速";
        break;
    case UpgradeMod::StreamLine:
        icon_path = ":/assets/images/icons/upgrades/u_streamline";
        desc_text = "流线构型: +30%子弹速度";
        break;
    case UpgradeMod::Sniper:
        icon_path = ":/assets/images/icons/upgrades/u_sniper";
        desc_text = "狙击: +15%子弹速度, -20%扩散";
        break;
    case UpgradeMod::TakeAim:
        icon_path = ":/assets/images/icons/upgrades/u_takeaim";
        desc_text = "精确打击: -50%扩散, -10%射速";
        break;
    case UpgradeMod::Piercinground:
        icon_path = ":/assets/images/icons/upgrades/u_piercinground";
        desc_text = "穿甲子弹: +2穿透";
        break;
    case UpgradeMod::Haste:
        icon_path = ":/assets/images/icons/upgrades/u_haste";
        desc_text = "加速: +10%移动速度";
        break;
    case UpgradeMod::OverSpeed:
        icon_path = ":/assets/images/icons/upgrades/u_overspeed";
        desc_text = "超速驾驶: +25%移动速度, +20%扩散";
        break;
    case UpgradeMod::RunGun:
        icon_path = ":/assets/images/icons/upgrades/u_runngun";
        desc_text = "RunN'Gun: +15%移动速度, +10%射速";
        break;
    case UpgradeMod::Windy:
        icon_path = ":/assets/images/icons/upgrades/u_windy";
        desc_text = "风之行者: 若在15s内未受到伤害, +30%移动速度, +30%射速";
        break;
    case UpgradeMod::AutoFire:
        icon_path = ":/assets/images/icons/upgrades/u_void"; // u_autofire
        desc_text = "自动射击: 鼠标右键，用了的都说好！";
        break;
    default:
        break;
    }

    button_ = new QGraphicsButton(icon_path, kPos, scene_);
    button_->setScale(3);

    desc_ = new QGraphicsTextItem();
    desc_->setDefaultTextColor(Qt::white);
    desc_->setFont(QFont("funshion pixel proportional", 14));
    desc_->setPos(kPos.x() - 40, kPos.y() + 100);
    desc_->setTransformOriginPoint(desc_->boundingRect().center());
    desc_->setPlainText(desc_text);

    scene_->addItem(desc_);
    scene_->addItem(button_);
}

void UpgradeMod::MouseClick()
{
    QString desc_text; // 这些文本可以删除
    switch (kType)
    {
    case UpgradeMod::Void:
        desc_text = "???: o(╥﹏╥)o";
    case UpgradeMod::Vitality:
        desc_text = "活力: +2HP";
        player_->SetHealth(player_->GetHealth() + 2);
        player_->SetMaxHealth(player_->GetMaxHealth() + 2);
        break;
    case UpgradeMod::Giant:
        desc_text = "巨人化: +4HP, +25%角色大小";
        player_->SetHealth(player_->GetHealth() + 4);
        player_->SetMaxHealth(player_->GetMaxHealth() + 4);
        player_->setScale(player_->scale() * 1.25);
        break;
    case UpgradeMod::Dodge:
        desc_text = "规避动作: +30%闪避";
        player_->SetDodge(player_->GetDodge() + 30);
        break;
    case UpgradeMod::Regeneration:
        desc_text = "自我修理: 每30s回复1HP";
        player_->SetRegenerate(true);
        break;
    case UpgradeMod::HolloPoint:
        desc_text = "空尖子弹: +25%伤害";
        weapon_->SetDamageBoost(weapon_->GetDamageBoost() + 0.25);
        break;
    case UpgradeMod::DoubleShot:
        desc_text = "双子弹: +25%伤害";
        weapon_->SetRoundBoost(weapon_->GetRoundBoost() + 1);
        weapon_->SetDamageBoost(weapon_->GetDamageBoost() + 0.25);
        break;
    case UpgradeMod::HeavyBarrel:
        desc_text = "重型枪管: +40%伤害, -10%射速";
        weapon_->SetDamageBoost(weapon_->GetDamageBoost() + 0.4);
        weapon_->SetSpeedBoost(weapon_->GetSpeedBoost() - 0.1);
        break;
    case UpgradeMod::Reaper:
        desc_text = "死神收割: +15%伤害, +1穿透";
        weapon_->SetDamageBoost(weapon_->GetDamageBoost() + 0.15);
        weapon_->SetPenetrationBoost(weapon_->GetPenetrationBoost() + 1);
        break;
    case UpgradeMod::FastHand:
        desc_text = "快速射击: +15%射速";
        weapon_->SetSpeedBoost(weapon_->GetSpeedBoost() + 0.15);
        break;
    case UpgradeMod::Overload:
        desc_text = "超载: +25%射速, +30%扩散";
        weapon_->SetSpeedBoost(weapon_->GetSpeedBoost() + 0.25);
        weapon_->SetSpreadBoost(weapon_->GetSpreadBoost() + 0.3);
        break;
    case UpgradeMod::LightBullet:
        desc_text = "轻型子弹: +20%射速, -10%伤害";
        weapon_->SetSpeedBoost(weapon_->GetSpeedBoost() + 0.2);
        weapon_->SetDamageBoost(weapon_->GetDamageBoost() - 0.1);
        break;
    case UpgradeMod::Barrage:
        desc_text = "弹雨: 站立不动时+35%射速";
        weapon_->SetBarrage(true);
        break;
    case UpgradeMod::StreamLine:
        desc_text = "流线构型: +30%子弹速度";
        weapon_->SetSpeedBoost(weapon_->GetSpeedBoost() + 0.3);
        break;
    case UpgradeMod::Sniper:
        desc_text = "狙击: +15%子弹速度, -20%扩散";
        weapon_->SetSpeedBoost(weapon_->GetSpeedBoost() + 0.15);
        weapon_->SetSpreadBoost(weapon_->GetSpreadBoost() - 0.2);
        break;
    case UpgradeMod::TakeAim:
        desc_text = "精确打击: -50%扩散, -10%射速";
        weapon_->SetSpreadBoost(weapon_->GetSpreadBoost() - 0.5);
        weapon_->SetSpeedBoost(weapon_->GetSpeedBoost() - 0.1);
        break;
    case UpgradeMod::Piercinground:
        desc_text = "穿甲子弹: +2穿透";
        weapon_->SetPenetrationBoost(weapon_->GetPenetrationBoost() + 2);
        break;
    case UpgradeMod::Haste:
        desc_text = "加速: +10%移动速度";
        player_->SetSpeedBoost(player_->GetSpeedBoost() + 0.1);
        break;
    case UpgradeMod::OverSpeed:
        desc_text = "超速驾驶: +25%移动速度, +20%扩散";
        player_->SetSpeedBoost(player_->GetSpeedBoost() + 0.25);
        weapon_->SetSpreadBoost(weapon_->GetSpreadBoost() + 0.2);
        break;
    case UpgradeMod::RunGun:
        desc_text = "RunN'Gun: +15%移动速度, +10%射速";
        player_->SetSpeedBoost(player_->GetSpeedBoost() + 0.15);
        weapon_->SetSpeedBoost(weapon_->GetSpeedBoost() + 0.1);
        break;
    case UpgradeMod::Windy:
        desc_text = "风之行者: 若在15s内未受到伤害, +30%移动速度, +30%射速";
        player_->SetWindy(true);
        break;
    case UpgradeMod::AutoFire:
        desc_text = "自动射击: 鼠标右键，用了的都说好！";
        player_->SetAutoFire(true);
        break;
    default:
        break;
    }

    upgraded_mods.push_back(kType);
    log_.AddLog(desc_text);
    emit UpgradeModSelected(kType);

    Close();
}
