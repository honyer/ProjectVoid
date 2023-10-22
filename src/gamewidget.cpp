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

    // 加载全局字体，只需加载一次，其他地方均可使用
    QFontDatabase::addApplicationFont(":/assets/fonts/fusion-pixel-proportional.ttf");

    // 布局边缘为0,使用布局的方式将StatMenu和GamenEngine嵌入到GameWidget中
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

    gameengine_ = new GameEngine(weapon, archive, kWidth, kHeight, this);
    layout_->addWidget(gameengine_);
    connect(gameengine_, &GameEngine::RestartGame, this, &GameWidget::RestartGame);
    connect(gameengine_, &GameEngine::ExitGame, this, &GameWidget::ExitGame);
}

void GameWidget::RestartGame()
{
     layout_->removeWidget(gameengine_);
     gameengine_->close();
     QTimer::singleShot(0, gameengine_, [=]()
                        {  gameengine_->CloseAll(); });

    InitStartMenu();
}

void GameWidget::ExitGame()
{
    exit(0);
}
