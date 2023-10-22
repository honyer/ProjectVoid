#ifndef STARTMENU_H
#define STARTMENU_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSettings>
#include <QStack>
#include <QSoundEffect>
#include <QKeyEvent>
#include <QPushButton>

class ShopItem;
class StartMenu : public QGraphicsView
{
    Q_OBJECT
public:
    StartMenu(int width, int height, QWidget *parent = nullptr);
    void CloseAll(); // 清理场景

signals:
    void PrepareGame(int weapon, int archive);
    void ExitGame();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QGraphicsScene *main_scene_;           // 主场景
    QGraphicsScene *prepare_scene_;        // 准备场景
    QGraphicsScene *shop_scene_;           // 商城场景
    QGraphicsScene *archive_scene_;        // 商城场景
    QStack<QGraphicsScene *> scene_stack_; // 场景栈
    QList<QPushButton *> buttons_;         // 按钮
    QList<ShopItem *> shop_items_;         // 商城物品

    QSoundEffect *bgm_;          // 背景音乐
    QSoundEffect *button_click_; // 按钮点击
    QSettings *settings_;        // 设置
    int weapon_;                 // 武器索引
    int archive_;                // 存档索引

    const int kWidth;    // 窗口宽
    const int kHeight;   // 窗口高
    QPixmap bakcground_; // 背景图片
    int data_amount_;    // 游戏金币，Data数量
    QString style_main_; // 主按钮样式
    QString style_sqr_;  // 方形按钮样式

    void LoadData();          // 加载数据
    void InitMainScene();     // 初始化主场景
    void InitPrepareScene();  // 初始化准备场景
    void InitPrepareWeapon(); // 初始化武器
    void InitShopScene();     // 初始化商城场景
    void InitArchiveScene();  // 初始化存档场景

    void ShopButtonClicked(QPushButton *button, ShopItem *item, QGraphicsTextItem *data_text); // 购买商城物品

    void LaunchGame(); // 开始游戏
};

#endif // STARTMENU_H
