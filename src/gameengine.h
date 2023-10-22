#pragma once

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSoundEffect>
#include <QTimer>
#include <QTime>
#include <QSettings>
#include <QThread>
#include <QMutex>

class Map;
class Player;
class Weapon;
class HeartLabel;
class GameMenu;
class GameEnd;
class GlobalTimer;
class CollisionDetector;

class GameEngine : public QGraphicsView
{
    Q_OBJECT
public:
    GameEngine(int weapon, int archive, int width, int height, QWidget *parent = nullptr);

    void CloseAll(); // 关闭GameEngine

signals:
    void RestartGame();
    void ExitGame();
    void SceneItemsChanged(QList<QGraphicsItem *> items);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    const int kWidth;   // 窗口宽
    const int kHeight;  // 窗口高
    const int kWeapon;  // 武器索引
    const int kArchive; // 存档索引

    GlobalTimer &gtimer;           // 全局计时器
    QThread *collision_thread_;    // 碰撞线程
    CollisionDetector *detector_;  // 碰撞检测
    QList<QGraphicsItem *> items_; // 场景的所有item

    Map &map;                         // 地图
    QGraphicsScene *main_scene_;      // 主场景
    QTimer *main_timer_;              // 定时器
    QGraphicsPixmapItem *background_; // 背景
    Player *player_;                  // 玩家
    Weapon *weapon_;                  // 武器

    QGraphicsRectItem *log_rect_; // 日志矩形
    QGraphicsTextItem *log_text_; // 日志文本

    QSettings *archives_settings_[3]; // 存档
    QSettings *game_settings_;        // 游戏设置
    QSoundEffect *bgm_;               // 背景音乐
    QSoundEffect *button_click_;      // 按钮点击音效

    GameMenu *menu_;      // Esc游戏菜单界面
    GameEnd *end_;        // 游戏结束界面
    bool escape_switch_;  // Escape键开关,是否响应Esc
    bool escape_pressed_; // Esc键按下的奇数次

    QGraphicsTextItem *data_text_; // Data文本
    int data_amount_;              // Data数量
    int data_count_;               // Data计数
    int data_collector_;           // Data速度加成

    QGraphicsTextItem *countdown_text_; // 倒计时文本
    int countdown_seconds_;             // 倒计时秒
    int countdown_count_;               // 计时器计数

    QList<HeartLabel *> heart_list_; // 血条

    QRectF exp_bg_rect_;            // 经验条背景矩形
    QGraphicsRectItem *exp_bar_bg_; // 经验条背景
    QGraphicsRectItem *exp_bar_;    // 经验条
    QGraphicsTextItem *level_text_; // 等级文本
    bool levelup_switch_;           // 是否正在升级

    int game_phase_;         // 游戏阶段
    int enemy_spawn_max_;    // 敌人最大生成数量
    qreal enemy_spawn_rate_; // 敌人生成频率
    int enemy_spawn_count_;  // 敌人生成计数
    int enemy_amount_;       // 敌人数量

    void InitData();      // 初始化数据
    void InitMainScene(); // 初始化主场景
    void InitPlayer();    // 初始化玩家
    void InitExpBar();    // 初始化经验条
    void InitDataText();  // 初始化数据文本
    void InitCountDown(); // 初始化倒计时
    void InitWeapon();    // 初始化武器
    void InitEnemies();   // 初始化敌人

    void CheckCollision(); // 检查碰撞
    void GetCheckItems();  // 获取需要检查碰撞的Item

    void LoadArchive(int index); // 加载存档
    void SaveArchive(int index); // 保存存档

    void MainHandle();           // 主循环
    void LevelUp();              // 角色升级
    void SetHealthBar();         // 初始化生命条
    void SetExpBar(qreal ratio); // 设置经验条
    void DataIncrement();        // 数据收集
    void UpdateBackground();     // 更新背景
    void UpdateCountDown();      // 更新倒计时
    void GenerateEnemies();      // 生成敌人
    void ShowLog();              // 显示日志
    void UpdateLog(QString str); // 更新日志

    void ShowGameMenu();            // 游戏菜单
    void ShowGameEnd(int index);    // 游戏结束 0:失败 1:胜利
    void ShowArchive();             // 显示存档菜单
    void ChooseGameMenu(int index); // 选择游戏菜单
    void ChooseGameEnd(int index);  // 选择游戏结束

    void GamePause();           // 游戏暂停
    void GameResume(int index); // 游戏恢复
};
