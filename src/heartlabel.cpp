#include "heartlabel.h"

HeartLabel::HeartLabel(const QPointF &pos, const QPointF &size, QWidget *parent)
    : QLabel(parent), size_(size), pos_(pos)
{
    heart_full_ = QPixmap(":/assets/images/icons/heart_full.png");
    heart_empty_ = QPixmap(":/assets/images/icons/heart_empty.png");
    this->setPixmap(heart_full_);
    this->setGeometry(pos_.x(), pos_.y(), size_.x(), size_.y());
    this->show();
}

void HeartLabel::LoseHeart()
{
    this->setPixmap(heart_empty_);
}

void HeartLabel::GainHeart()
{
    this->setPixmap(heart_full_);
}
