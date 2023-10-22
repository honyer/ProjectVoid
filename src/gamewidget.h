#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

class StartMenu;
class GameEngine;

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    GameWidget(QWidget* parent = nullptr);

private:
    const int kWidth;  // ���ڿ�
    const int kHeight; // ���ڸ�

    QVBoxLayout* layout_;    // ����
    StartMenu* startmenu_;   // ��ʼ�˵�
    GameEngine* gameengine_; // ��Ϸ����

    int weapon_;  // �������ͣ�Ĭ��0
    int archive_; // �浵��ţ�Ĭ��-1��ʾû�д浵

    void InitStartMenu();                     // ��ʼ����ʼ�˵�
    void LaunchGame(int weapon, int archive); // ��ʼ��Ϸ
    void RestartGame();                       // ������Ϸ
    void ExitGame();                          // �˳���Ϸ
};
#endif // GAMEWIDGET_H
