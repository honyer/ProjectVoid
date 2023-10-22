#include "shopitem.h"

ShopItem::ShopItem(QString name, int price, int level, int max_level, QPixmap icon, QObject *parent)
    : QObject(parent), kName(name), kIcon(icon), price_(price), level_(level), kMaxLevel(max_level)
{
}

QString ShopItem::GetName()
{
    return kName;
}

QPixmap ShopItem::GetIcon()
{
    return kIcon;
}

int ShopItem::GetPrice()
{
    return this->price_;
}

void ShopItem::SetPrice(int price)
{
}

int ShopItem::GetLevel()
{
    return level_;
}

void ShopItem::SetLevel(int level)
{
    this->level_ = level;
}

int ShopItem::GetMaxLevel()
{
    return kMaxLevel;
}
