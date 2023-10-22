#include "gameend.h"
#include "qgraphicsbutton.h"

GameEnd::GameEnd(bool win, QRectF rect, QGraphicsScene *scene, QObject *parent)
    : QObject(parent), scene_(scene), rect_(rect), game_win_(win)
{
    bg_rect_ = new QGraphicsRectItem(rect);
    QBrush brush(QColor(100, 100, 100));
    bg_rect_->setBrush(brush);
    bg_rect_->setPen(QPen(QColor(0, 0, 0), 4));
    bg_rect_->setOpacity(0.8);
    scene_->addItem(bg_rect_);

    InitButtons();
    InitText();

    QList<QGraphicsItem *> items = bg_rect_->childItems();
}

void GameEnd::Close()
{
    QList<QGraphicsItem *> items = bg_rect_->childItems();
    for (auto item : items)
    {
        for (auto child : item->childItems())
        {
            scene_->removeItem(child);
            delete child;
        }
        scene_->removeItem(item);
        delete item;
    }

    delete bg_rect_;
    this->deleteLater();
}

void GameEnd::InitText()
{
    // 游戏结束位置
    QPoint base_pos(rect_.x() + rect_.width() / 2 - 50, rect_.y() + 60);
    QString text = QString("游戏结束\n\n");
    QGraphicsTextItem *game_over_text = new QGraphicsTextItem();
    game_over_text->setFont(QFont("Fusion Pixel Proportional", 24));
    game_over_text->setDefaultTextColor(Qt::white);
    game_over_text->setPlainText("游戏结束");
    game_over_text->setPos(base_pos);
    game_over_text->setParentItem(bg_rect_);

    // 结果文本位置
    QGraphicsTextItem *result_text = new QGraphicsTextItem();
    QString result_str = "胜利！";
    if (game_win_)
    {
        result_text->setDefaultTextColor(Qt::green);
    }
    else
    {
        result_str = "失败!";
        result_text->setDefaultTextColor(Qt::red);
    }
    result_text->setFont(QFont("Fusion Pixel Proportional", 40));
    result_text->setPlainText(result_str);
    result_text->setPos(base_pos.x() + 10, base_pos.y() + 80);
    result_text->setParentItem(bg_rect_);
}

void GameEnd::InitButtons()
{
    int space = 100; // 按钮间隔

    QString img_path = ":/assets/images/icons/buttons/ic_main_button";
    QPixmap button_img(":/assets/images/icons/buttons/ic_main_button_idle.png");
    qreal scale = 0.6;
    int width = button_img.width() * scale;
    int height = button_img.height() * scale;

    // 第一个按钮位置
    QPoint base_pos(rect_.x() + space + (rect_.width() / 2 - width * 2),
                    rect_.y() + rect_.height() - height - 100);

    for (int i = 0; i < 2; i++)
    {
        QGraphicsButton *button;
        QGraphicsTextItem *desc_text = new QGraphicsTextItem();

        QPointF pos(base_pos.x() + i * (space + width), base_pos.y());
        QPointF desc_pos(40, 25);

        switch (i)
        {
        case Exit:
            button = new QGraphicsButton(img_path, pos, scene_);
            desc_text->setPlainText("退出游戏");
            connect(button, &QGraphicsButton::MouseClick, this, [=]()
                    { ButtonSelected(0); });
            break;
        case Restart:
            button = new QGraphicsButton(img_path, pos, scene_);
            desc_text->setPlainText("重新开始");
            connect(button, &QGraphicsButton::MouseClick, this, [=]()
                    { ButtonSelected(1); });
            break;
        }

        button->setScale(scale);
        button->setZValue(1);
        button->setParentItem(bg_rect_);

        // desc_text->setPos(base_pos.x() + 10 + i * (space + width), base_pos.y() + 10);
        desc_text->setPos(desc_pos);
        desc_text->setDefaultTextColor(Qt::white);
        desc_text->setFont(QFont("fusion pixel proportional", 28));
        desc_text->setAcceptedMouseButtons(Qt::NoButton);
        desc_text->setParentItem(button);
    }
}

void GameEnd::ButtonSelected(int index)
{

    emit ClickedMenuButton(index);
}
