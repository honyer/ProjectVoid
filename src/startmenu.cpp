#include "startmenu.h"
#include "archivemod.h"
#include "shopitem.h"
#include "weaponmod.h"
#include "soundplayer.h"

#include <QFile>
#include <QGraphicsPixmapItem>
#include <QUrl>
#include <QTimer>

StartMenu::StartMenu(int width, int height, QWidget *parent)
    : QGraphicsView(parent), kWidth(width), kHeight(height)
{
    // 设置窗口大小，取消滚动条
    setFixedSize(kWidth, kHeight);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    main_scene_ = new QGraphicsScene(this);
    prepare_scene_ = new QGraphicsScene(this);
    shop_scene_ = new QGraphicsScene(this);
    archive_scene_ = new QGraphicsScene(this);
    this->setScene(main_scene_);

    LoadData();
    InitMainScene();
    InitPrepareScene();
    InitShopScene();
    InitArchiveScene();
}

void StartMenu::CloseAll()
{
    for (auto item : main_scene_->items())
    {

        main_scene_->removeItem(item);
        delete item;
    }

    for (auto item : prepare_scene_->items())
    {

        prepare_scene_->removeItem(item);
        delete item;
    }

    for (auto item : archive_scene_->items())
    {

        archive_scene_->removeItem(item);
        delete item;
    }

    for (auto item : shop_scene_->items())
    {

        shop_scene_->removeItem(item);
        delete item;
    }

    for (auto item : shop_items_)
    {
        delete item;
    }
    delete main_scene_;
    delete prepare_scene_;
    delete archive_scene_;
    delete shop_scene_;
    delete this;
}

void StartMenu::LoadData()
{
    weapon_ = 0;
    archive_ = -1;

    bakcground_ = QPixmap(":/assets/images/ABackground.png");
    settings_ = new QSettings("./game.ini", QSettings::IniFormat, this);
    data_amount_ = settings_->value("data_amount", 500).toInt();

    bgm_ = SoundPlayer::GetInstance().StartBgmSound();
    bgm_->setLoopCount(QSoundEffect::Infinite);
    bgm_->play();

    button_click_ = SoundPlayer::GetInstance().ButtonSound();

    // 加载按钮样式
    QFile file_main_pb(":/assets/stylesheet/main_pb.qss");
    if (file_main_pb.open(QFile::ReadOnly))
    {
        style_main_ = file_main_pb.readAll();
    }
    QFile file_sqr_pb(":/assets/stylesheet/sqr_pb.qss");
    if (file_sqr_pb.open(QFile::ReadOnly))
    {
        style_sqr_ = file_sqr_pb.readAll();
    }
}

void StartMenu::InitMainScene()
{
    main_scene_->setSceneRect(0, 0, kWidth, kHeight);
    main_scene_->setBackgroundBrush(bakcground_);

    // 添加logo
    QGraphicsPixmapItem *logo = new QGraphicsPixmapItem(QPixmap(":/assets/images/icons/ic_logo.png"));
    logo->setPos(480, 200);
    logo->setTransformOriginPoint(logo->boundingRect().center());
    logo->setScale(1.5);
    main_scene_->addItem(logo);

    // 添加左叶子图片
    QGraphicsPixmapItem *leaf_left = new QGraphicsPixmapItem(QPixmap(":/assets/images/T_TitleLeaves.png"));
    leaf_left->setScale(1.6);
    main_scene_->addItem(leaf_left);
    // 添加右叶子图片,是左叶子图片的镜像
    QGraphicsPixmapItem *leaf_right = new QGraphicsPixmapItem();
    leaf_right->setPixmap(QPixmap::fromImage(QImage(":/assets/images/T_TitleLeaves.png").mirrored(true, false)));
    leaf_right->setTransformOriginPoint(leaf_right->boundingRect().right(), leaf_right->boundingRect().top());
    leaf_right->setPos(1034, 0);
    leaf_right->setScale(1.6);
    main_scene_->addItem(leaf_right);

    // 添加开始按钮
    QPushButton *prepare_button = new QPushButton("准备游戏");
    prepare_button->setStyleSheet(style_main_);
    prepare_button->move(550, 450);
    prepare_button->resize(220, 100);
    connect(prepare_button, &QPushButton::clicked, this, [=]()
            {
                button_click_->play();
                this->setScene(prepare_scene_);
                scene_stack_.push(main_scene_); });
    main_scene_->addWidget(prepare_button);
    buttons_.push_back(prepare_button);

    // 添加退出按钮
    QPushButton *exit_button = new QPushButton("退出游戏");
    exit_button->setStyleSheet(style_main_);
    exit_button->move(550, kHeight - 140);
    exit_button->resize(220, 100);
    connect(exit_button, &QPushButton::clicked, this, [=]()
            { button_click_->play();
                emit ExitGame(); });
    main_scene_->addWidget(exit_button);
}

void StartMenu::InitPrepareScene()
{
    prepare_scene_->setSceneRect(0, 0, kWidth, kHeight);
    prepare_scene_->setBackgroundBrush(bakcground_);

    // 添加存档按钮
    QPushButton *archive_button = new QPushButton("存档");
    archive_button->setStyleSheet(style_sqr_);
    archive_button->move(300, 450);
    archive_button->resize(120, 110);
    prepare_scene_->addWidget(archive_button);
    connect(archive_button, &QPushButton::clicked, this, [=]()
            {
                button_click_->play();
        this->setScene(archive_scene_);
        scene_stack_.push(prepare_scene_); });
    buttons_.push_back(archive_button);

    // 添加开始按钮
    QPushButton *start_button = new QPushButton("开始游戏");
    start_button->setStyleSheet(style_main_);
    start_button->move(550, 450);
    start_button->resize(220, 110);
    prepare_scene_->addWidget(start_button);
    connect(start_button, &QPushButton::clicked, this, &StartMenu::LaunchGame);
    buttons_.push_back(start_button);

    // 添加商店按钮
    QPushButton *shop_button = new QPushButton("商店");
    shop_button->setStyleSheet(style_sqr_);
    shop_button->move(900, 450);
    shop_button->resize(120, 110);
    prepare_scene_->addWidget(shop_button);
    connect(shop_button, &QPushButton::clicked, this, [=]()
            {
                button_click_->play();
        this->setScene(shop_scene_);
        scene_stack_.push(prepare_scene_); });
    buttons_.push_back(shop_button);

    InitPrepareWeapon();
}

void StartMenu::InitPrepareWeapon()
{
    QList<WeaponMod *> weapon_buttons;
    for (int i = 0; i < WeaponMod::kWeaponTypeList.size(); ++i)
    {
        WeaponMod *weapon_button = new WeaponMod(i, prepare_scene_, this);
        weapon_buttons.append(weapon_button);
        connect(weapon_button, &WeaponMod::WeaponSelected, this, [=](int index)
                { button_click_->play();
                weapon_ = index; });
    }

    // 循环遍历所有的武器按钮，与其他按钮建立选择连接
    for (auto p : weapon_buttons)
    {
        for (auto q : weapon_buttons)
        {
            if (p != q)
            {
                connect(p, &WeaponMod::WeaponSelected, q, &WeaponMod::ResetSelected);
            }
        }
    }
}

void StartMenu::InitShopScene()
{
    shop_scene_->setSceneRect(0, 0, kWidth, kHeight);
    shop_scene_->setBackgroundBrush(bakcground_);

    // 商城左上角显示收集数量
    QGraphicsTextItem *data_text = new QGraphicsTextItem();
    data_text->setPlainText("Data: " + QString::number(data_amount_));
    data_text->setPos(20, 0);
    data_text->setFont(QFont("fusion pixel proportional", 24));
    data_text->setDefaultTextColor(Qt::white);
    shop_scene_->addItem(data_text);

    // 读取存档的等级数据
    int hp_increment_level = settings_->value("data_collector", 0).toInt();
    int muzzle_ckoke_level = settings_->value("hp_increment", 0).toInt();
    int data_collector_level = settings_->value("muzzle_choke", 0).toInt();
    int respawn_udisk_level = settings_->value("respawn_udisk", 0).toInt();
    // 添加商品
    shop_items_.append(new ShopItem("HP Increment", 500, hp_increment_level, 5, QPixmap(":/assets/images/icons/ic_hp_increment.png")));
    shop_items_.append(new ShopItem("Muzzle Choke", 1000, muzzle_ckoke_level, 5, QPixmap(":/assets/images/icons/ic_muzzle_choke.png")));
    shop_items_.append(new ShopItem("Data Collector", 500, data_collector_level, 3, QPixmap(":/assets/images/icons/ic_data_collector.png")));
    shop_items_.append(new ShopItem("Respawn Udisk", 500, respawn_udisk_level, 1, QPixmap(":/assets/images/icons/ic_respawn_udisk.png")));

    // 将商品展示在商店场景中
    int index = 0;
    QPointF base_pos(220, 200);
    for (auto item : shop_items_)
    {
        QGraphicsPixmapItem *item_pixmap = new QGraphicsPixmapItem(item->GetIcon());
        item_pixmap->setPos(base_pos.x() * (index + 1) + 40, base_pos.y());
        item_pixmap->setScale(5);
        shop_scene_->addItem(item_pixmap);

        // 在图片下方添加购买按钮
        QPushButton *buy_button = new QPushButton();
        buy_button->setText(item->GetName() + "\nPrice: " + QString::number(item->GetPrice()));
        buy_button->move(base_pos.x() * (index + 1), base_pos.y() + 150);
        buy_button->setFont(QFont("fusion pixel proportional", 16));
        shop_scene_->addWidget(buy_button);
        buttons_.push_back(buy_button);

        // 检测是否可买
        if (data_amount_ >= item->GetPrice() &&
            item->GetLevel() < item->GetMaxLevel())
        {
            buy_button->setEnabled(true);
        }
        else
        {
            buy_button->setEnabled(false);
        }

        // 点击购买
        connect(buy_button, &QPushButton::clicked, this, [=]()
                {   button_click_->play();
                    ShopButtonClicked(buy_button, item, data_text); });
        index++;
    }
}

void StartMenu::InitArchiveScene()
{
    archive_scene_->setSceneRect(0, 0, kWidth, kHeight);
    archive_scene_->setBackgroundBrush(bakcground_);

    QPointF pos(kWidth / 2 - 40, 180);

    for (int i = 0; i < 3; ++i)
    {
        ArchiveMod *archive_button = new ArchiveMod(i, 0, pos, archive_scene_, this);
        connect(archive_button, &ArchiveMod::LoadArchive, this, [&](int index)
                { archive_=index;
                button_click_->play();
                LaunchGame(); });
    }

    QGraphicsTextItem *archive_tips = new QGraphicsTextItem();
    archive_tips->setDefaultTextColor(Qt::white);
    archive_tips->setPlainText("点击任意一个存档图标，立即开始游戏！");
    archive_tips->setFont(QFont("fusion pixel proportional", 16));
    archive_tips->setPos(640 - archive_tips->boundingRect().width() / 2, 30);
    archive_scene_->addItem(archive_tips);
}

void StartMenu::ShopButtonClicked(QPushButton *button, ShopItem *item, QGraphicsTextItem *data_text)
{
    data_amount_ -= item->GetPrice();
    item->SetLevel(item->GetLevel() + 1);
    data_text->setPlainText("Data: " + QString::number(data_amount_));

    // 写入配置
    QString item_name = item->GetName();
    item_name = item_name.replace(" ", "_");
    item_name = item_name.toLower();
    settings_->setValue("data_amount", data_amount_);
    settings_->setValue(item_name, item->GetLevel());
    settings_->sync();

    if (data_amount_ < item->GetPrice() ||
        item->GetLevel() >= item->GetMaxLevel())
    {
        button->setEnabled(false);
    }
}

void StartMenu::LaunchGame()
{

    bgm_->stop();
    settings_->sync();
    emit PrepareGame(weapon_, archive_);
}

void StartMenu::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (scene_stack_.isEmpty())
        {
            emit ExitGame();
            this->deleteLater();
            return;
        }

        QGraphicsScene *scene = scene_stack_.pop();
        this->setScene(scene);
    }

    QGraphicsView::keyPressEvent(event);
}
