#pragma once

#include <QLabel>

class HeartLabel : public QLabel
{
    Q_OBJECT
public:
    explicit HeartLabel(const QPointF &pos, const QPointF &size, QWidget *parent = nullptr);

    void LoseHeart();
    void GainHeart();

private:
    QPointF pos_;
    QPointF size_;
    QPixmap heart_full_;
    QPixmap heart_empty_;
};