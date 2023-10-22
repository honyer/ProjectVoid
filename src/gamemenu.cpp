#include "gamemenu.h"
#include "qgraphicsbutton.h"

GameMenu::GameMenu(QRectF rect, QGraphicsScene *scene, QObject *parent)
    : QObject(parent), scene_(scene), rect_(rect)
{
    bg_rect_ = new QGraphicsRectItem(rect);
    QBrush brush(QColor(100, 100, 100));
    bg_rect_->setBrush(brush);
    bg_rect_->setPen(QPen(QColor(0, 0, 0), 4));
    bg_rect_->setOpacity(0.8);
    scene_->addItem(bg_rect_);

    InitButtons();
}

void GameMenu::Close()
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

void GameMenu::InitButtons()
{
    // 按钮顺序
    QList<MenuButton> buttons = {Archive, Restart, Exit};
    int num = buttons.size();
    int space = 25; // 按钮间隔

    QString img_path = ":/assets/images/icons/buttons/ic_main_button";
    QPixmap button_img(":/assets/images/icons/buttons/ic_main_button_idle.png");
    qreal scale = 0.6;
    int width = button_img.width() * scale;
    int height = button_img.height() * scale;

    // 第一个按钮位置
    QPoint base_pos(rect_.x() + (rect_.width() - width) / 2,
                    rect_.y() + space);

    for (int i = 0; i < num; i++)
    {
        QGraphicsButton *button;
        QPointF pos(base_pos.x(), base_pos.y() + i * (space + height));
        QGraphicsTextItem *desc_text = new QGraphicsTextItem();
        QPointF desc_pos(40, 25);

        switch (buttons.at(i))
        {
        case Archive:
            button = new QGraphicsButton(img_path, pos, scene_);
            desc_text->setPlainText("存档游戏");
            connect(button, &QGraphicsButton::MouseClick, this, [=]()
                    { ButtonSelected(Archive); qDebug()<<"archive"; });
            break;
        case Restart:
            button = new QGraphicsButton(img_path, pos, scene_);
            desc_text->setPlainText("重新开始");
            connect(button, &QGraphicsButton::MouseClick, this, [=]()
                    { ButtonSelected(Restart); qDebug()<<"restart"; });
            break;
        case Exit:
            button = new QGraphicsButton(img_path, pos, scene_);
            desc_text->setPlainText("退出游戏");
            connect(button, &QGraphicsButton::MouseClick, this, [=]()
                    { ButtonSelected(Exit);qDebug()<<"exit"; });
            break;
        }
        button->setScale(scale);
        button->setParentItem(bg_rect_);
        button->setZValue(1);

        // 父item设置到button上才能正常响应到鼠标事件，
        // 如果和button同时设置父对象到bg_rect上，不会响应到鼠标事件
        desc_text->setParentItem(button);
        // desc_text->setPos(base_pos.x() + 10, base_pos.y() + 10 + i * (space + height));
        desc_text->setPos(desc_pos);
        desc_text->setDefaultTextColor(Qt::white);
        desc_text->setFont(QFont("fusion pixel proportional", 26));
        desc_text->setAcceptedMouseButtons(Qt::NoButton);
    }
}

void GameMenu::ButtonSelected(MenuButton button)
{
    emit ClickedMenuButton(button);
    // 重新开始按钮GameEngine::CloseAll()来清理
    if (button == Archive)
        Close();
}
