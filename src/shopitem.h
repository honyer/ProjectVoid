#pragma once

#include <QObject>
#include <QPixmap>

class ShopItem : public QObject
{
    Q_OBJECT
public:
    ShopItem(QString name, int price, int level, int max_level,QPixmap icon, QObject *parent = nullptr);

    QString GetName();
    QPixmap GetIcon();

    int GetPrice();
    void SetPrice(int price);

    int GetLevel();
    void SetLevel(int level);

    int GetMaxLevel();

private:
    const QString kName; // 商品名
    const QPixmap kIcon; // 商品图片
    int price_;          // 商品价格
    int level_;          // 商品等级
    const int kMaxLevel; // 商品最高等级
};