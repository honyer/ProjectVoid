#pragma once

#include <QPixmap>
#include <QtCore>

class Map
{
public:
    static Map &GetInstance()
    {
        static Map instance;
        return instance;
    }

    QPixmap Image()
    {
        return background_img_;
    }
    int Width()
    {
        return background_img_.width();
    }
    int Height()
    {
        return background_img_.height();
    }
    QRectF Bounds()
    {
        return map_bounds_;
    }
    QRectF PlayBounds()
    {
        return play_bounds_;
    }
    QList<QRectF> Barriers()
    {
        QList<QRectF> barriers;
        barriers.append(left_top_barrier_);
        barriers.append(left_bottom_barrier_);
        barriers.append(right_top_barrier_);
        barriers.append(right_bottom_barrier_);
        barriers.append(central_barrier_);
        return barriers;
    }

    bool IsInMap(const QPointF &pos)
    {
        if (!map_bounds_.contains(pos))
            return false;
    }
    bool IsInPlayground(const QPointF &pos)
    {
        if (play_bounds_.contains(pos))
            return true;
        return false;
    }
    bool IsInBarriers(const QPointF &pos)
    {
        if (left_top_barrier_.contains(pos) ||
            left_bottom_barrier_.contains(pos) ||
            right_top_barrier_.contains(pos) ||
            right_bottom_barrier_.contains(pos) ||
            central_barrier_.contains(pos))
            return true;
        return false;
    }

    bool Moveable(const QPointF &pos)
    {
        if (IsInPlayground(pos) && !IsInBarriers(pos))
            return true;
        return false;
    }

private:
    Map()
    {
        background_img_ = QPixmap(":/assets/images/background_gameplay.png");
        map_bounds_ = QRectF(0, 0, background_img_.width(), background_img_.height());
        play_bounds_ = QRectF(680, 603, 3464, 1405);
        left_top_barrier_ = QRectF(878, 721, 172, 308);
        left_bottom_barrier_ = QRectF(884, 1513, 172, 308);
        right_top_barrier_ = QRectF(2320, 1097, 172, 308);
        right_bottom_barrier_ = QRectF(3780, 731, 172, 308);
        central_barrier_ = QRectF(3764, 1509, 172, 308);
    }
    QPixmap background_img_;      // 背景图片
    QRectF map_bounds_;           // 地图范围
    QRectF play_bounds_;          // 游玩范围
    QRectF left_top_barrier_;     // 左上障碍物区域
    QRectF left_bottom_barrier_;  // 左下障碍物区域
    QRectF right_top_barrier_;    // 右上障碍物区域
    QRectF right_bottom_barrier_; // 右下障碍物区域
    QRectF central_barrier_;      // 中心障碍物区域
};
