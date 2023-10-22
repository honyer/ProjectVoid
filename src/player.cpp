#include "player.h"
#include "map.h"
#include "soundplayer.h"
#include "log.h"

Player::Player(int width, int height, QGraphicsView *view, QWidget *parent)
    : QObject(parent), main_view_(view), kWidth(width), kHeight(height),
      log(Log::GetInstance()), map(Map::GetInstance())
{
    mv_idle_ = new QMovie(this);
    mv_run_ = new QMovie(this);
    hurt_sound = SoundPlayer::GetInstance().GetHurtSound();
    levelup_sound = SoundPlayer::GetInstance().GetLevelUpSound();

    // 加载动画
    mv_idle_->setFileName(":/assets/images/pl_idle.gif");
    mv_idle_->setSpeed(33);
    mv_idle_->start();
    mv_run_->setFileName(":/assets/images/pl_run.gif");
    mv_run_->setSpeed(67);
    mv_run_->start();

    InitAttributes();
    LoadPlayer();

    // 设置屏幕窗口中心位置
    this->setTransformOriginPoint(boundingRect().right(), boundingRect().center().y());
    this->setPos(kWidth / 2 - boundingRect().width(), kHeight / 2 - boundingRect().height());
    this->setScale(3);
}

void Player::LoadPlayer()
{
    bool is_idle = (velocity_.x() == 0 && velocity_.y() == 0);

    QImage img;
    if (is_idle)
    {
        img = QImage(mv_idle_->currentPixmap().toImage());
        if (main_view_->mapFromGlobal(QCursor::pos()).x() < kWidth / 2)
            img = img.mirrored(true, false);
    }
    else
    {
        img = QImage(mv_run_->currentPixmap().toImage());
        if (velocity_.x() < 0)
            img = img.mirrored(true, false);
    }

    // 无敌状态，设置透明度
    if (invincible_)
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

    QPixmap pix(QPixmap::fromImage(img));
    this->setPixmap(pix);
}

void Player::UpdatePosition()
{
    this->setPos(NewValidPosition());
}

QPointF Player::GetMovedPosition()
{
    return this->pos() - NewValidPosition();
}

void Player::AddVelocity(int dx, int dy)
{
    if (velocity_.x() != dx)
        velocity_.setX(velocity_.x() + dx);
    if (velocity_.y() != dy)
        velocity_.setY(velocity_.y() + dy);
}

QPoint Player::GetVelocity()
{
    return QPoint(velocity_.x(), velocity_.y());
}

void Player::SetVelocityX(int dx)
{
    this->velocity_.setX(dx);
}

void Player::SetVelocityY(int dy)
{
    this->velocity_.setY(dy);
}

bool Player::GetStandStill()
{
    // TODO
    return false;
}

int Player::GetHealth()
{
    return health_;
}

void Player::SetHealth(int health)
{
    this->health_ = health > 10 ? 10 : health;
}

int Player::GetMaxHealth()
{
    return max_health_;
}

void Player::SetMaxHealth(int max_health)
{
    this->max_health_ = max_health > 10 ? 10 : max_health;
}

qreal Player::GetSpeedBoost()
{
    return speed_boost_;
}

void Player::SetSpeedBoost(qreal speed_boost)
{
    this->speed_boost_ = speed_boost;
}

int Player::GetLevel()
{
    return this->level_;
}

void Player::SetLevel(int level)
{
    this->level_ = level;
}

int Player::GetExp()
{
    return exp_;
}

void Player::SetExp(int exp)
{
    this->exp_ = exp;
}

double Player::GetPickupRange()
{
    return pickup_range_;
}

void Player::PickupExp(int exp)
{
    this->exp_ += exp;
    log.AddLog(QString("获得经验: Exp +%1").arg(exp));
    if (level_ >= max_level_)
        return;

    // 等级计算公式,将其定义为浮点类型，否则计算经验比例是0
    qreal levelup_need = 5 + 4.0 * (level_ - 1);

    if (this->exp_ >= levelup_need)
    {
        this->exp_ = 0;
        this->level_++;
        levelup_sound->play();
        log.AddLog(QString("等级提升: Lv %1").arg(level_));
        emit PlayerLevelup();
    }

    level_ratio_ = qMin(1.0, this->exp_ / levelup_need);
    emit PlayerExpPicked(level_ratio_);
}

qreal Player::GetDodge()
{
    return dodge_;
}

void Player::SetDodge(qreal dodge)
{
    this->dodge_ = dodge;
}

qreal Player::GetInvincibleTime()
{
    return invincible_time_;
}

void Player::SetInvincibleTime(qreal invincible_time)
{
    this->invincible_time_ = invincible_time;
}

void Player::ResetInvincible()
{
    this->invincible_ = false;
}

bool Player::GetRegenerate()
{
    return this->regenerate_;
}

void Player::SetRegenerate(bool regenerate)
{
    this->regenerate_ = regenerate;
}

void Player::Regenerate()
{
    if (!regenerate_)
        return;

    regenerate_count_++;
    if (regenerate_count_ < 3000)
        return;

    regenerate_count_ = 0;
    health_ >= max_health_ ? health_ = max_health_ : health_++;
    emit PlayerRegenerate();
    log.AddLog("再生触发: HP +1");
}

void Player::Hurt()
{
    if (invincible_)
        return;
    int dodge_chance = QRandomGenerator::global()->bounded(0, 100);
    if (dodge_chance < dodge_)
        return;

    hurt_sound->play();
    health_--;
    invincible_ = true;
    invincible_timer_->start(1000 * invincible_time_);
    windy_count_ = 0;

    emit PlayerHurt();
    log.AddLog("受到攻击: HP -1");

    if (health_ <= 0)
    {
        health_ = 0;
        emit PlayerDeadth();
    }
}

bool Player::GetWindy()
{
    return windy_;
}

void Player::SetWindy(bool windy)
{
    this->windy_ = windy;
}

bool Player::GetWindyOnState()
{
    return windy_on_;
}

void Player::OnWindyState()
{
    if (!windy_)
        return;

    windy_count_++;
    if (windy_count_ <= 1500)
    {
        windy_on_ = false;
        return;
    }
    windy_on_ = true;
}

bool Player::GetAutoFire()
{
    return this->auto_fire_;
}

void Player::SetAutoFire(bool auto_fire)
{
    this->auto_fire_ = auto_fire;
}

void Player::InitAttributes()
{
    level_ = 1;
    max_level_ = 30;
    level_ratio_ = 0;
    velocity_ = QVector2D(0, 0);
    health_ = 3;
    max_health_ = 3;
    speed_ = 2;
    speed_boost_ = 1;
    exp_ = 0;
    pickup_range_ = 100;
    dodge_ = 0;
    invincible_ = false;
    invincible_time_ = 1;
    invincible_timer_ = new QTimer(this);
    regenerate_ = false;
    regenerate_count_ = 0;
    windy_ = false;
    windy_count_ = 0;
    windy_on_ = false;
    auto_fire_ = false;

    // 无敌时间结束后重置
    connect(invincible_timer_, &QTimer::timeout, this, &Player::ResetInvincible);
}

QPointF Player::NewValidPosition()
{
    // 设置默认除数为1。如果速度向量大于0，则使用速度向量长度作为除数。
    qreal speed_reduce = velocity_.length() > 0 ? velocity_.length() : 1.0;

    // 如果玩家处于风行者状态，加速度增加0.3。
    qreal speed_factor = speed_boost_ + 0.3 * windy_on_;

    // 根据向量速度、基础速度、速度因子来计算x和y方向上的移动量。
    qreal x_movement = velocity_.x() * speed_ * speed_factor / speed_reduce;
    qreal y_movement = velocity_.y() * speed_ * speed_factor / speed_reduce;

    // 通过将x和y移动量添加到当前位置来计算新位置。
    QPointF new_pos = this->pos() + QPointF(x_movement, y_movement);

    // 检查新位置是否合法。如果不合法，则尝试仅在x方向上移动。
    if (!map.Moveable(new_pos))
    {
        new_pos = this->pos() + QPointF(x_movement, 0);
        // 如果新位置仍然不合法，则尝试仅在y方向上移动。
        if (!map.Moveable(new_pos))
        {
            new_pos = this->pos() + QPointF(0, y_movement);
            // 如果新位置仍然不合法，则保持原始位置不变。
            if (!map.Moveable(new_pos))
            {
                new_pos = this->pos();
            }
        }
    }

    return new_pos;
}
