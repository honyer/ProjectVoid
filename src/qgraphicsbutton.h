#pragma once

#include <QGraphicsPixmapItem>
#include <QObject>

class QGraphicsButton : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit QGraphicsButton(QString icon_path, QPointF pos, QObject *parent = nullptr);

    void ResetSelected();

signals:
    void HoverEnter();
    void HoverLeave();
    void MouseClick();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString icon_path_;
    bool hovered_ = false;
    bool pressed_ = false;
    bool selected_ = false;
};