#include "archivemod.h"
#include <QDateTime>

ArchiveMod::ArchiveMod(int index, int scene_index, QPointF pos, QGraphicsScene *scene, QObject *parent)
    : QObject(parent), scene_(scene), kPos(pos), kSceneIndex(scene_index), kIndex(index)
{

    button_ = new QGraphicsButton(":/assets/images/icons/buttons/ic_blank",
                                  QPointF(kPos.x(), kPos.y() + 120 * kIndex), this);
    button_->setScale(4);
    scene_->addItem(button_);

    // 读取上次保存时间
    QString file_name = QString("./archive_%1.dat").arg(kIndex);
    QSettings archive_settings_ = QSettings(file_name, QSettings::IniFormat, this);
    archive_settings_.beginGroup("Time");
    QString saved_time = archive_settings_.value("saved_time").toString();
    archive_settings_.endGroup();

    // 存档位置显示提示文本,因为背景图片放大了4倍，因此字体缩小4倍
    desc_text_ = new QGraphicsTextItem();
    desc_text_->setParentItem(button_);
    desc_text_->setDefaultTextColor(Qt::white);
    desc_text_->setFont(QFont("fusion pixel proportional", 3));
    desc_text_->setTextWidth(button_->boundingRect().width() * 2);
    if (saved_time.isEmpty())
    {
        desc_text_->setPlainText("Empty");
    }
    else
    {
        desc_text_->setPlainText(saved_time.replace(" ", "\n"));
    }

    connect(button_, &QGraphicsButton::MouseClick, this, &ArchiveMod::MouseClick);
}

void ArchiveMod::HideAndDestroy()
{
    scene_->removeItem(desc_text_);
    scene_->removeItem(button_);
    delete desc_text_;
    delete button_;
    delete this;
}

void ArchiveMod::MouseClick()
{
    if (kSceneIndex == 0)
    {
        emit LoadArchive(kIndex);
        return;
    }

    emit SaveArchive(kIndex);
    HideAndDestroy();
}
