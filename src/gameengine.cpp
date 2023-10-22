#include "gameengine.h"
#include "map.h"
#include "globaltimer.h"
#include "soundplayer.h"
#include "player.h"
#include "weapon.h"
#include "enemy.h"
#include "heartlabel.h"
#include "upgrademod.h"
#include "gamemenu.h"
#include "gameend.h"
#include "archivemod.h"
#include "log.h"

#include <QSettings>
#include <QPushButton>

GameEngine::GameEngine(int weapon, int archive, int width, int height, QWidget *parent)
    : QGraphicsView(parent), kHeight(height), kWidth(width), kArchive(archive), kWeapon(weapon),
      gtimer(GlobalTimer::GetInstance()), map(Map::GetInstance())
{
    this->setFixedSize(kWidth, kHeight);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 鼠标移动事件 mouseMoveEvent 默认只有在鼠标左键按下时才会触发
    // 启用鼠标追踪功能,无论是否按下鼠标左键都能触发鼠标移动事件
    this->setMouseTracking(true);

    connect(&gtimer, &GlobalTimer::MainTimerStarted, this, [=]()
            { qDebug() << "main timer start"; });
    main_scene_ = new QGraphicsScene(this);
    main_timer_ = gtimer.GetTimer();
    main_timer_->start();
    gtimer.SwitchTimer();

    bgm_ = SoundPlayer::GetInstance().GamingBgmSound();
    bgm_->setLoopCount(QSoundEffect::Infinite);
    bgm_->play();

    button_click_ = SoundPlayer::GetInstance().ButtonSound();

    InitMainScene();
    connect(main_timer_, &QTimer::timeout, this, &GameEngine::MainHandle);
}

void GameEngine::CloseAll()
{
    // main_timer_->stop();
    for (auto item : main_scene_->items())
    {
        for (auto child : item->childItems())
        {
            main_scene_->removeItem(child);
            delete child;
        }
        main_scene_->removeItem(item);
        delete item;
    }
    delete main_scene_;
    delete this;
    // this->deleteLater();
}

void GameEngine::GamePause()
{
    main_timer_->stop();
    gtimer.SwitchTimer();
}

void GameEngine::GameResume(int index)
{
    escape_pressed_ = false;
    // 0:按Esc回到游戏；1：点击存档回到游戏；2：升级界面回到游戏

    switch (index)
    {
    case 0:
        menu_->Close();
        menu_ = nullptr;
        break;
    case 1:
        menu_ = nullptr;
        break;
    case 2:

        break;
    default:
        break;
    }

    main_timer_->start();
    gtimer.SwitchTimer();
}

void GameEngine::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (escape_switch_ && !escape_pressed_)
        {
            escape_pressed_ = true;
            GamePause();
            ShowGameMenu();
        }
        else if (escape_switch_ && escape_pressed_)
        {
            GameResume(0);
        }
    }

    switch (event->key())
    {
    case Qt::Key_W:
        player_->AddVelocity(0, -1);
        break;
    case Qt::Key_S:
        player_->AddVelocity(0, 1);
        break;
    case Qt::Key_A:
        player_->AddVelocity(-1, 0);
        break;
    case Qt::Key_D:
        player_->AddVelocity(1, 0);
        break;
    default:
        break;
    }
}

void GameEngine::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_W:
        player_->AddVelocity(0, 1);
        break;
    case Qt::Key_S:
        player_->AddVelocity(0, -1);
        break;
    case Qt::Key_A:
        player_->AddVelocity(1, 0);
        break;
    case Qt::Key_D:
        player_->AddVelocity(-1, 0);
        break;
    default:
        break;
    }
}

void GameEngine::mousePressEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::MouseButton::LeftButton:
        weapon_->FireOn();
        break;
    case Qt::MouseButton::RightButton:
        weapon_->SwitchAutoFire();
        break;
    default:
        break;
    }
    QGraphicsView::mousePressEvent(event);
}

void GameEngine::mouseReleaseEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::MouseButton::LeftButton:
        weapon_->FireOff();
        break;
    case Qt::MouseButton::RightButton:
        break;
    default:
        break;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void GameEngine::mouseMoveEvent(QMouseEvent *event)
{
    auto pos = event->pos();
    weapon_->RotateToCursor(pos);
}

void GameEngine::InitMainScene()
{
    this->setRenderHint(QPainter::Antialiasing);
    this->setRenderHint(QPainter::SmoothPixmapTransform);
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    this->setScene(main_scene_);
    main_scene_->setSceneRect(0, 0, kWidth, kHeight);

    background_ = new QGraphicsPixmapItem(map.Image());
    background_->setZValue(-0x3ffffff);
    background_->moveBy(-map.Width() / 2, -map.Height() / 2);
    main_scene_->addItem(background_);

    InitData();
    InitPlayer();
    if (kArchive >= 0 && kArchive < 3)
    {
        LoadArchive(kArchive);
    }
    else
    {
        // InitWeapon();
    }

    InitWeapon();
    InitDataText();
    InitExpBar();
    InitCountDown();
    InitEnemies();
    ShowLog();
}

void GameEngine::InitData()
{
    menu_ = nullptr;
    escape_switch_ = true;
    escape_pressed_ = false;
    levelup_switch_ = false;

    for (int i = 0; i < 3; i++)
    {
        QString file_name = QString("./archive_%1.dat").arg(i);
        archives_settings_[i] = new QSettings(file_name, QSettings::IniFormat, this);
    }

    game_settings_ = new QSettings("./game.ini", QSettings::IniFormat, this);
    data_amount_ = game_settings_->value("data_amount", 0).toInt();
    data_collector_ = game_settings_->value("data_collector", 0).toInt();
    data_count_ = 0;
}

void GameEngine::InitPlayer()
{
    // 先setPos再moveBy，否则位置不在视野范围内
    player_ = new Player(kWidth, kHeight, this);
    player_->setParentItem(background_);
    player_->moveBy(map.Width() / 2, map.Height() / 2);
    SetHealthBar();
    // LevelUp(2);

    connect(player_, &Player::PlayerHurt, this, &GameEngine::SetHealthBar);
    connect(player_, &Player::PlayerRegenerate, this, &GameEngine::SetHealthBar);
    connect(player_, &Player::PlayerExpPicked, this, [=](qreal ratio)
            { SetExpBar(ratio);
            data_amount_++; });
    connect(player_, &Player::PlayerLevelup, this, &GameEngine::LevelUp);
    connect(player_, &Player::PlayerDeadth, this, [=]()
            { ShowGameEnd(0); });
}

void GameEngine::UpdateBackground()
{
    QPointF pos = player_->GetMovedPosition();
    background_->moveBy(pos.x(), pos.y());
}

void GameEngine::UpdateCountDown()
{
    countdown_count_++;
    if (countdown_count_ < 100)
        return;
    countdown_count_ = 0;
    countdown_seconds_--;
    QString time = QTime(0, 0).addSecs(countdown_seconds_).toString("mm:ss");
    countdown_text_->setPlainText(time);

    // timeout,游戏胜利
    if (countdown_seconds_ <= 0)
    {
        ShowGameEnd(1);
    }

    // 获取当前时间
    QString currentTimeString = QTime::currentTime().toString("hh:mm:ss.zzz");
    qDebug() << currentTimeString << __FILE__ << __LINE__;
}

void GameEngine::InitWeapon()
{
    weapon_ = new Weapon(kWeapon, player_, main_scene_);
    weapon_->setParentItem(background_);
}

void GameEngine::InitEnemies()
{
    game_phase_ = 0;
    enemy_spawn_max_ = 5;
    enemy_spawn_rate_ = 200;
    enemy_spawn_count_ = 0;
    enemy_amount_ = 0;
}

void GameEngine::CheckCollision()
{
    // connect(this, &GameEngine::SceneItemsChanged, detector_, &CollisionDetector::SetSceneItems);
    // connect(detector_, &CollisionDetector::CheckFinished, this, &GameEngine::GetCheckItems);
    // connect(detector_, &CollisionDetector::BulletCollidePlayer, this, []()
    //         { qDebug() << "BulletCollidePlayer"; });

    // connect(detector_, &CollisionDetector::BulletCollideEnemy, this, []()
    //         { qDebug() << "BulletCollideEnemy"; });

    // connect(detector_, &CollisionDetector::EnemyCollidePlayer, this, []()
    //         { qDebug() << "EnemyCollidePlayer"; });
}

void GameEngine::GetCheckItems()
{

    // QList<Enemy *> enemies;
    // QList<Bullet *> bullets;

    // QList<QGraphicsItem *> copy;
    // for (auto item : copy)
    // {
    //     Enemy *enemy = dynamic_cast<Enemy *>(item);
    //     if (enemy)
    //     {
    //         enemies.append(enemy);
    //         copy.append(item);
    //     }

    //     Bullet *bullet = dynamic_cast<Bullet *>(item);
    //     if (bullet)
    //     {
    //         bullets.append(bullet);
    //         copy.append(item);
    //     }
    // }
    // detector_->SetSceneItems(copy);
}

void GameEngine::LoadArchive(int index)
{
    return; // TODO

    QSettings *archive = archives_settings_[index];
    // 游戏时间
    archive->beginGroup("Time");
    QString saved_time = archive->value("saved_time").toString();
    qDebug() << "saved_time: " << saved_time;
    if (saved_time.length() < 16)
        return;
    countdown_seconds_ = archive->value("countdown_seconds").toInt();
    countdown_count_ = archive->value("countdown_count").toInt();
    archive->endGroup();

    // 玩家属性
    archive->beginGroup("Player");
    QPointF pos = archive->value("pos").toPointF();
    qreal scale = archive->value("scale").toDouble();
    int health = archive->value("health").toInt();
    int max_health = archive->value("max_health").toInt();
    int level = archive->value("level").toInt();
    int exp = archive->value("exp").toInt();
    qreal dodge = archive->value("dodge").toDouble();
    qreal speed_boost = archive->value("speed_boost").toDouble();
    bool regenerate = archive->value("regenerate").toBool();
    bool windy = archive->value("windy").toBool();
    bool auto_fire = archive->value("auto_fire").toBool();
    qDebug() << "load level" << level;

    QPointF delta_pos = pos - player_->pos();
    background_->moveBy(-delta_pos.x(), -delta_pos.y());
    player_->setPos(pos);
    player_->setScale(scale);
    player_->SetHealth(health);
    player_->SetMaxHealth(max_health);
    player_->SetLevel(level);
    player_->SetExp(exp);
    player_->SetDodge(dodge);
    player_->SetSpeedBoost(speed_boost);
    player_->SetRegenerate(regenerate);
    player_->SetWindy(windy);
    player_->SetAutoFire(auto_fire);
    archive->endGroup();

    // // 武器属性
    archive->beginGroup("Weapon");
    int weapon_type = archive->value("weapon_type").toInt();
    qreal fire_rate_boost = archive->value("fire_rate_boost").toDouble();
    qreal bullet_speed_boost = archive->value("speed_boost").toDouble();
    qreal damage_boost = archive->value("damage_boost").toDouble();
    qreal spread_boost = archive->value("spread_boost").toDouble();
    int round_boost = archive->value("round_boost").toInt();
    int penetration_boost = archive->value("penetration_boost").toInt();
    bool barrage = archive->value("barrage").toBool();

    weapon_ = new Weapon(weapon_type, player_, main_scene_);
    weapon_->setParentItem(background_);
    weapon_->SetFireRateBoost(fire_rate_boost);
    weapon_->SetSpeedBoost(bullet_speed_boost);
    weapon_->SetDamageBoost(damage_boost);
    weapon_->SetSpreadBoost(spread_boost);
    weapon_->SetRoundBoost(round_boost);
    weapon_->SetPenetrationBoost(penetration_boost);
    weapon_->SetBarrage(barrage);
    archive->endGroup();
    qDebug() << "load weapon_type" << weapon_type;

    // 升级过的道具
    QVariantList upgraded_list;
    archive->beginGroup("Upgrade");
    upgraded_list = archive->value("upgraded_list").toList();
    for (auto upgraded : upgraded_list)
    {
        UpgradeMod::upgraded_mods.push_back(static_cast<UpgradeMod::UpgradeType>(upgraded.toInt()));
    }
    archive->endGroup();
    qDebug() << "load upgraded_list" << UpgradeMod::upgraded_mods;

    // // 敌人重新生成
    // QList<QGraphicsItem *> item_list = main_scene_->items();
    // QVariantList enemy_list = archive->value("Enemy_list").toList();
    // for (auto p : enemy_list)
    // {
    //     int type = p.toMap().value("type").toInt();
    //     QPointF pos = p.toMap().value("pos").toPointF();
    //     Enemy *enemy = new Enemy(type, player_, main_scene_, this);
    //     enemy->setParentItem(background_);
    //     enemy->setPos(pos);
    //     enemy_amount_++;
    //     connect(enemy, &Enemy::EnemyDeath, this, [=]()
    //             { enemy_amount_--; });
    // }
    // archive->beginGroup("Enemy");
}

void GameEngine::SaveArchive(int index)
{
    QSettings *archive = archives_settings_[index];
    // 玩家属性
    QPointF pos = player_->pos();
    qreal scale = player_->scale();
    int health = player_->GetHealth();
    int max_health = player_->GetMaxHealth();
    int level = player_->GetLevel();
    int exp = player_->GetExp();
    qreal dodge = player_->GetDodge();
    qreal speed_boost = player_->GetSpeedBoost();
    bool regenerate = player_->GetRegenerate();
    bool windy = player_->GetWindy();
    bool auto_fire = player_->GetAutoFire();
    archive->beginGroup("Player");
    archive->setValue("pos", pos);
    archive->setValue("scale", scale);
    archive->setValue("health", health);
    archive->setValue("max_health", max_health);
    archive->setValue("level", level);
    archive->setValue("exp", exp);
    archive->setValue("dodge", dodge);
    archive->setValue("speed_boost", speed_boost);
    archive->setValue("regenerate", regenerate);
    archive->setValue("windy", windy);
    archive->setValue("auto_fire", auto_fire);
    archive->endGroup();

    // 武器属性
    qreal fire_rate_boost = weapon_->GetFireRateBoost();
    qreal bullet_speed_boost = weapon_->GetSpeedBoost();
    qreal damage_boost = weapon_->GetDamageBoost();
    qreal spread_boost = weapon_->GetSpreadBoost();
    int round_boost = weapon_->GetRoundBoost();
    int penetration_boost = weapon_->GetPenetrationBoost();
    bool barrage = weapon_->GetBarrage();
    archive->beginGroup("Weapon");
    archive->setValue("weapon_type", kWeapon);
    archive->setValue("fire_rate_boost", fire_rate_boost);
    archive->setValue("speed_boost", bullet_speed_boost);
    archive->setValue("damage_boost", damage_boost);
    archive->setValue("spread_boost", spread_boost);
    archive->setValue("round_boost", round_boost);
    archive->setValue("penetration_boost", penetration_boost);
    archive->setValue("barrage", barrage);
    archive->endGroup();

    // 升级过的道具
    QVariantList upgraded_list;
    QList<UpgradeMod::UpgradeType> upgraded_mods;
    for (auto mod : upgraded_mods)
    {
        upgraded_list.push_back(static_cast<int>(mod));
    }
    archive->beginGroup("Upgrade");
    archive->setValue("upgraded_list", upgraded_list);
    archive->endGroup();

    // 敌人
    QList<QGraphicsItem *> item_list = main_scene_->items();
    QVariantList enemy_list;
    QVariantMap enemy_map;
    for (auto item : item_list)
    {
        if (dynamic_cast<Enemy *>(item))
        {
            Enemy *enemy = dynamic_cast<Enemy *>(item);
            enemy_map.insert("type", enemy->GetType());
            enemy_map.insert("pos", enemy->pos());
            enemy_list.push_back(enemy_map);
        }
    }
    archive->beginGroup("Enemy");
    archive->setValue("enemy_list", enemy_list);
    archive->endGroup();
    // 游戏时间
    archive->beginGroup("Time");
    archive->setValue("countdown_seconds", countdown_seconds_);
    archive->setValue("countdown_count", countdown_count_);
    QString saved_time = QDateTime::currentDateTime().toString("yy/MM/dd hh:mm:ss");
    archive->setValue("saved_time", saved_time);
    archive->endGroup();

    archive->sync();
    // Escape_1:ON
    escape_switch_ = true;
    GameResume(1);
    // _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    // _CrtDumpMemoryLeaks();
}

void GameEngine::GenerateEnemies()
{
    enemy_spawn_count_++;
    if (enemy_spawn_count_ < enemy_spawn_rate_ || enemy_amount_ >= enemy_spawn_max_)
        return;

    enemy_spawn_count_ = 0;
    enemy_amount_++;
    enemy_spawn_rate_ += game_phase_ * 10;

    QPointF enemy_pos;
    qreal distance;

    do
    {
        enemy_pos.setX(QRandomGenerator::global()->bounded(0, map.Width()));
        enemy_pos.setY(QRandomGenerator::global()->bounded(0, map.Height()));
        distance = QLineF(enemy_pos, player_->pos()).length();
    } while (distance < 500 || distance > 700 || map.IsInBarriers(enemy_pos));

    int index = QRandomGenerator::global()->bounded(0, 5);

    Enemy *enemy = new Enemy(index, player_, main_scene_, this);
    if (!enemy)
        return;
    enemy->setParentItem(background_);
    enemy->setPos(enemy_pos);

    connect(enemy, &Enemy::EnemyDeath, this, [&]()
            { enemy_amount_--; });
}

void GameEngine::ShowLog()
{
    QRectF rect(0, kHeight - 250 - 20, 300, 250);
    log_rect_ = new QGraphicsRectItem(rect);
    QBrush brush(QColor(100, 100, 100));
    log_rect_->setBrush(brush);
    log_rect_->setPen(QPen(QColor(0, 0, 0), 2));
    log_rect_->setOpacity(0.5);
    main_scene_->addItem(log_rect_);

    log_text_ = new QGraphicsTextItem();
    log_text_->setDefaultTextColor(Qt::white);
    log_text_->setFont(QFont("fusion pixel proportional", 12));
    log_text_->setPos(5, kHeight - 250 - 10);
    log_text_->setTextWidth(290);
    log_text_->setParentItem(log_rect_);

    Log &log = Log::GetInstance();
    connect(&log, &Log::LogAdded, this, &GameEngine::UpdateLog);
    log.AddLog("Game Start");
}

void GameEngine::UpdateLog(QString str)
{
    QStringList logs = Log::GetInstance().GetLogs();
    QString text = "";
    for (const QString &log : logs)
    {
        text += log + "\n";
    }
    log_text_->setPlainText(text);
}

void GameEngine::ShowGameMenu()
{
    QRectF rect(kWidth / 2 - 150, 150, 300, 300);
    menu_ = new GameMenu(rect, main_scene_, this);

    connect(menu_, &GameMenu::ClickedMenuButton, this, &GameEngine::ChooseGameMenu);
}

void GameEngine::ShowGameEnd(int gameover)
{
    // 游戏结束 0:失败 1:胜利
    escape_switch_ = false;
    GamePause();

    QRectF rect(kWidth / 2 - 250, 150, 560, 500);
    end_ = new GameEnd(gameover, rect, main_scene_, this);

    connect(end_, &GameEnd::ClickedMenuButton, this, &GameEngine::ChooseGameEnd);
}

void GameEngine::ShowArchive()
{
    QPointF pos(kWidth / 2 - 40, 180);
    QList<ArchiveMod *> archive_mods;
    for (int i = 0; i < 3; ++i)
    {
        ArchiveMod *archive_button = new ArchiveMod(i, 1, pos, main_scene_, this);
        archive_mods.push_back(archive_button);
    }

    for (auto p : archive_mods)
    {
        for (auto q : archive_mods)
        {
            if (p != q)
            {
                connect(p, &ArchiveMod::SaveArchive, q, &ArchiveMod::HideAndDestroy);
            }
        }

        connect(p, &ArchiveMod::SaveArchive, this, &GameEngine::SaveArchive);
    }

    // QGraphicsTextItem *archive_tips = new QGraphicsTextItem();
    // archive_tips->setDefaultTextColor(Qt::white);
    // archive_tips->setPlainText("点击任意一个存档图标，立即开始游戏！");
    // archive_tips->setFont(QFont("fusion pixel proportional", 16));
    // archive_tips->setPos(640 - archive_tips->boundingRect().width() / 2, 30);
    // archive_scene_->addItem(archive_tips);
}

void GameEngine::ChooseGameMenu(int index)
{
    // 0 退出游戏 1 重新开始 2 存档

    button_click_->play();
    switch (index)
    {
    case 0:
        emit ExitGame();
        return;
    case 1:
        bgm_->stop();
        Log::GetInstance().Clear();
        menu_->Close();
        // CloseAll();
        emit RestartGame();
        return;
    case 2:
        // Escape_1:OFF
        escape_switch_ = false;
        ShowArchive();
        return;
    default:
        break;
    }

    // GameResume(1);
}
void GameEngine::ChooseGameEnd(int index)
{
    bgm_->stop();
    if (index)
    {
        Log::GetInstance().Clear();
        end_->Close();
        emit RestartGame();
    }
    else
    {
        emit ExitGame();
    }
}

void GameEngine::InitDataText()
{
    QString text = QString("Data: %1").arg(data_amount_);

    data_text_ = new QGraphicsTextItem(text);
    data_text_->setDefaultTextColor(Qt::white);
    data_text_->setFont(QFont("Fusion Pixel Proportional", 24));
    data_text_->setPos(40, 40);
    main_scene_->addItem(data_text_);
}

void GameEngine::InitCountDown()
{
    // 10分钟
    countdown_seconds_ = 1 * 60;
    countdown_count_ = 0;

    countdown_text_ = new QGraphicsTextItem();
    countdown_text_->setDefaultTextColor(Qt::white);
    countdown_text_->setFont(QFont("Fusion Pixel Proportional", 24));
    countdown_text_->setPos(kWidth - 120, 40);
    // countdown_text_->setTransformOriginPoint(countdown_text_->boundingRect().topRight());
    main_scene_->addItem(countdown_text_);
}

void GameEngine::InitExpBar()
{
    // 设置经验条背景
    exp_bg_rect_ = QRectF(440, 40, 400, 20);
    exp_bar_bg_ = new QGraphicsRectItem(exp_bg_rect_);
    exp_bar_bg_->setBrush(QColor(100, 100, 100));
    exp_bar_bg_->setPen(QPen(QColor(0, 0, 0), 4));
    exp_bar_bg_->setZValue(1);
    main_scene_->addItem(exp_bar_bg_);

    // 设置经验条,初始宽度为0
    int border = 2;
    QRectF bar_rect(exp_bg_rect_.x(), exp_bg_rect_.y() + border, 0, exp_bg_rect_.height() - 2 * border);
    exp_bar_ = new QGraphicsRectItem(bar_rect);
    exp_bar_->setBrush(QColor(0, 200, 255));
    exp_bar_->setParentItem(exp_bar_bg_);

    // 设置等级提示,位于经验条下边
    QPointF text_pos = QPointF(exp_bg_rect_.x() + exp_bg_rect_.width() / 2,
                               exp_bg_rect_.y() + exp_bg_rect_.height() + 5);
    level_text_ = new QGraphicsTextItem("Level 1");
    level_text_->setDefaultTextColor(Qt::white);
    level_text_->setFont(QFont("Fusion Pixel Proportional", 16));
    level_text_->setTransformOriginPoint(level_text_->boundingRect().center());
    level_text_->setPos(text_pos);
    main_scene_->addItem(level_text_);
}

void GameEngine::SetExpBar(qreal ratio)
{
    int border = 2;
    QRectF exp_bar_rect(exp_bg_rect_.x(), exp_bg_rect_.y() + border, 0, exp_bg_rect_.height() - 2 * border);
    exp_bar_rect.setWidth(ratio * exp_bg_rect_.width());

    exp_bar_->setRect(exp_bar_rect);
    int level = player_->GetLevel();
    level_text_->setPlainText(QString("Level %1").arg(level));
}

void GameEngine::DataIncrement()
{
    data_count_++;
    int rate = qMax(20.0, 100 * (1 - 0.15 * data_collector_));
    if (data_count_ < rate)
        return;

    data_count_ = 0;
    data_amount_++;
    data_text_->setPlainText(QString("Data: %1").arg(data_amount_));
}

void GameEngine::MainHandle()
{
    UpdateBackground();
    DataIncrement();
    UpdateCountDown();
    GenerateEnemies();
    player_->UpdatePosition();
    player_->LoadPlayer();
    player_->OnWindyState();
    player_->Regenerate();
    weapon_->UpdatePosition();
    weapon_->LoadWeapon();
    weapon_->Fire();
}
void GameEngine::LevelUp()
{
    int level = player_->GetLevel();
    level_text_->setPlainText(QString("Level %1").arg(level));

    // // Escape_2:OFF
    escape_switch_ = false;
    GamePause();

    QList<UpgradeMod::UpgradeType> uptype_list = UpgradeMod::GetRandomUpgradeMods();
    const QPointF first_pos(kWidth / uptype_list.size() - 80, 160);
    QList<UpgradeMod *> upgrade_list;

    // 生成三个升级选项图标
    for (int i = 0; i < uptype_list.size(); i++)
    {
        QPointF pos = QPointF(first_pos.x() * i + 280, first_pos.y());
        UpgradeMod *upgrade = new UpgradeMod(i, pos, uptype_list.at(i), player_, weapon_, main_scene_, this);
        upgrade_list.append(upgrade);
    }

    // 点击其中一个图标后，删除其他图标
    for (auto p : upgrade_list)
    {
        for (auto q : upgrade_list)
        {
            if (p != q)
            {
                connect(p, &UpgradeMod::UpgradeModSelected, q, &UpgradeMod::Close);
            }
        }

        connect(p, &UpgradeMod::UpgradeModSelected, this, [=]()
                { 
                    // Escape_2:ON
                escape_switch_ = true;
                SetHealthBar();
                GameResume(2); });
    }
}

void GameEngine::SetHealthBar()
{
    int health = player_->GetHealth();
    int max_health = player_->GetMaxHealth();

    // 清空血条列表
    for (auto *p : heart_list_)
    {
        if (p)
            delete p;
    }
    heart_list_.clear();

    // 起始位置，屏幕右上角
    QPointF pos(kWidth - 80, 120);
    QPointF size(40, 40);
    for (int i = 0; i < max_health; i++)
    {
        HeartLabel *heart = new HeartLabel(pos, size, this);
        if (i > health - 1)
            heart->LoseHeart();
        // 多余的血条向左移动
        heart->move(heart->x() - i * (size.x() + 4), heart->y());
        heart_list_.append(heart);
    }
}
