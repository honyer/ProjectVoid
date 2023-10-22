#include "qgraphicsbutton.h"

QGraphicsButton::QGraphicsButton(QString icon_path, QPointF pos, QObject *parent)
    : QObject(parent), icon_path_(icon_path)
{
    this->setAcceptHoverEvents(true);
    this->setAcceptedMouseButtons(Qt::LeftButton);

    this->setPixmap(QPixmap(icon_path_ + "_idle.png"));
    this->setPos(pos);
}

void QGraphicsButton::ResetSelected()
{
    selected_ = false;
    this->setPixmap(QPixmap(icon_path_ + "_idle.png"));
}

void QGraphicsButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    emit HoverEnter();
    // 如果是选择项，图标边框不用再变化
    if (selected_)
        return;
    this->setPixmap(QPixmap(icon_path_ + "_hover.png"));
    this->hovered_ = true;
}

void QGraphicsButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    emit HoverLeave();
    // 如果是选择项，图标边框不用再变化
    if (selected_)
        return;
    this->setPixmap(QPixmap(icon_path_ + "_idle.png"));
    this->hovered_ = false;
}

void QGraphicsButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    this->pressed_ = true;
}

void QGraphicsButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    if (this->pressed_ && this->hovered_)
    {
        selected_ = true;
        // this->setPixmap(QPixmap(ic_path + "_selected.png"));
        emit MouseClick();
    }
    pressed_ = false;
}
