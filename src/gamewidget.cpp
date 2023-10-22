#include "gamewidget.h"
#include "gameengine.h"
#include "startmenu.h"
#include <QFontDatabase>

#include <QTimer>

GameWidget::GameWidget(QWidget* parent)
    : QWidget(parent), kWidth(1280), kHeight(720), weapon_(0), archive_(-1)
{
    setFixedSize(kWidth, kHeight);
    setWindowTitle("ProjectVoid");
    setWindowIcon(QIcon(":/assets/images/icons/Myicon.ico"));

    setMouseTracking(true);
    QPixmap cursor_img = QPixmap(":/assets/images/ic_cursor.png");
    QCursor cursor(cursor_img, -cursor_img.width() / 4, cursor_img.height() / 4);
    setCursor(cursor);

    // ����ȫ�����壬ֻ�����һ�Σ������ط�����ʹ��
    QFontDatabase::addApplicationFont(":/assets/fonts/fusion-pixel-proportional.ttf");

    // ���ֱ�ԵΪ0,ʹ�ò��ֵķ�ʽ��StatMenu��GamenEngineǶ�뵽GameWidget��
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_);

    InitStartMenu();
}

void GameWidget::InitStartMenu()
{
    startmenu_ = new StartMenu(kWidth, kHeight, this);
    layout_->addWidget(startmenu_);

    connect(startmenu_, &StartMenu::PrepareGame, this, &GameWidget::LaunchGame);
    connect(startmenu_, &StartMenu::ExitGame, this, &GameWidget::ExitGame);
}

void GameWidget::LaunchGame(int weapon, int archive)
{
    startmenu_->close();
    layout_->removeWidget(startmenu_);
    QTimer::singleShot(0, startmenu_, [=]()
        { startmenu_->CloseAll(); });

    gameengine_ = new GameEngine(weapon_, archive_, kWidth, kHeight, this);
    gameengine_->show();
    connect(gameengine_, &GameEngine::RestartGame, this, &GameWidget::RestartGame);
    connect(gameengine_, &GameEngine::ExitGame, this, &GameWidget::ExitGame);
}

void GameWidget::RestartGame()
{
     layout_->removeWidget(gameengine_);
     gameengine_->close();
     QTimer::singleShot(0, gameengine_, [=]()
                        { delete gameengine_; });

    InitStartMenu();
}

void GameWidget::ExitGame()
{
    exit(0);
}
