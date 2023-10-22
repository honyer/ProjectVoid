#include "weapon.h"
#include "weaponmod.h"
#include "bullet.h"
#include "soundplayer.h"
#include <QRandomGenerator>

Weapon::Weapon(int index, Player *player, QGraphicsScene *scene, QWidget *parent)
    : QObject(parent), main_scene_(scene), player_(player), kIndex(index)
{
    fire_sound = SoundPlayer::GetInstance().GetFireSound();

    InitWeapon();
    connect(player_, &Player::PlayerLevelup, this, &Weapon::LevelUp);
}

void Weapon::LoadWeapon()
{
    QImage img(weapon_img_.toImage());
    int angle = qAbs(rotation());
    if (angle > 90 && angle < 270)
    {
        img.mirror(false, true);
    }
    this->setPixmap(QPixmap::fromImage(img));
}

void Weapon::UpdatePosition()
{
    this->setPos(player_->pos());
}

void Weapon::RotateToCursor(const QPointF &pos)
{
    QPointF weapon_pos = this->mapToScene(this->transformOriginPoint());
    double dx = pos.x() - weapon_pos.x();
    double dy = pos.y() - weapon_pos.y();
    double angle = qRadiansToDegrees(qAtan2(dy, dx));
    this->setRotation(angle);
}
void Weapon::Fire()
{
    fire_rate_count_++;
    sfx_play_count_++;

    if (!fire_switch_ && !auto_fire_)
        return;
    qreal speed_factor = fire_rate_boost_ + 0.35 * barrage_ * player_->GetStandStill() +
                         0.3 * player_->GetWindyOnState();
    qreal fire_rate = fire_rate_ / speed_factor;
    if (fire_rate_count_ < fire_rate)
        return;

    fire_rate_count_ = 0;
    if (sfx_play_count_ > 10)
    {
        fire_sound->play();
        sfx_play_count_ = 0;
    }

    for (int i = 0; i < round_ + round_boost_; i++)
    {
        Bullet *bullet = new Bullet(kIndex, this->pos(), this->rotation(), speed_, damage_,
                                    penetration_, main_scene_, player_);

        int spread = qMax(10 * spread_ * spread_boost_, 1.0);
        bullet->setRotation(this->rotation() + QRandomGenerator::global()->bounded(-spread, spread) * 0.1);

        bullet->setParentItem(this->parentItem());
    }
}
void Weapon::FireOn()
{
    fire_switch_ = true;
}
void Weapon::FireOff()
{
    fire_switch_ = false;
    //    fire_sound->stop();
}

qreal Weapon::GetFireRateBoost()
{
    return fire_rate_boost_;
}

void Weapon::SetFireRateBoost(qreal boost)
{
    this->fire_rate_boost_ = boost;
}

qreal Weapon::GetSpeedBoost()
{
    return speed_boost_;
}

void Weapon::SetSpeedBoost(qreal boost)
{
    this->speed_boost_ = boost;
}

qreal Weapon::GetDamageBoost()
{
    return damage_boost_;
}

void Weapon::SetDamageBoost(qreal boost)
{
    this->damage_boost_ = boost;
}

qreal Weapon::GetSpreadBoost()
{
    return spread_boost_;
}

void Weapon::SetSpreadBoost(qreal boost)
{
    this->spread_boost_ = boost;
}

int Weapon::GetRoundBoost()
{
    return round_boost_;
}

void Weapon::SetRoundBoost(int boost)
{
    this->round_boost_ = boost;
}

int Weapon::GetPenetrationBoost()
{
    return penetration_boost_;
}

void Weapon::SetPenetrationBoost(int boost)
{
    this->penetration_boost_ = boost;
}

bool Weapon::GetBarrage()
{
    return this->barrage_;
}

void Weapon::SetBarrage(bool barrage)
{
    this->barrage_ = barrage;
}
void Weapon::SwitchAutoFire()
{
    if (player_->GetAutoFire())
        auto_fire_ = !auto_fire_;
}

void Weapon::InitWeapon()
{
    this->weapon_img_.load(GetWeaponImage());
    this->setPixmap(weapon_img_);
    this->setTransformOriginPoint(boundingRect().center());
    this->setScale(3);

    InitAttributes();
}

void Weapon::InitAttributes()
{
    sfx_play_count_ = 0;

    fire_rate_ = 10;
    fire_rate_boost_ = 1;
    fire_rate_count_ = 100;
    damage_ = 6;
    damage_boost_ = 1;
    speed_ = 15;
    speed_boost_ = 1;
    round_ = 1;
    round_boost_ = 0;
    spread_ = 15;
    spread_boost_ = 1;
    penetration_ = 1;
    penetration_boost_ = 0;
    barrage_ = false;
    fire_switch_ = false;
    auto_fire_ = false;

    switch (WeaponMod::kWeaponTypeList.at(kIndex))
    {
    case WeaponMod::Smg:
        break;
    case WeaponMod::Smart:
        spread_ = 20;
        fire_rate_ = 20;
        damage_ = 5;
        break;
    case WeaponMod::Shotgun:
        spread_ = 30;
        round_boost_ = 7;
        fire_rate_ = 75;
        break;
    case WeaponMod::Dmr:
        spread_ = 3;
        speed_ = 60;
        damage_ = 40;
        fire_rate_ = 100;
        break;
    default:
        break;
    }
}

QString Weapon::GetWeaponImage()
{
    QString path;
    switch (WeaponMod::kWeaponTypeList.at(kIndex))
    {
    case WeaponMod::Smg:
        path = ":/assets/images/weapon/wp_smg.png";
        break;
    case WeaponMod::Smart:
        path = ":/assets/images/weapon/wp_smart.png";
        break;
    case WeaponMod::Shotgun:
        path = ":/assets/images/weapon/wp_shotgun.png";
        break;
    case WeaponMod::Dmr:
        path = ":/assets/images/weapon/wp_dmr.png";
        break;
    default:
        break;
    }
    return path;
}

void Weapon::LevelUp()
{
    int level = player_->GetLevel();
    int damage = level % 5 > 0 ? 0 : 1;
    int round = level % 10 > 0 ? 0 : 1;
    damage_ += damage;
    round_ += round;
}
