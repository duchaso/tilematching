#include "board.h"

#include <QColor>

Board::Board()
{

}

int Board::rowCount(const QModelIndex &parent) const
{
    return 10;
}

int Board::columnCount(const QModelIndex &parent) const
{
    return 10;
}

QVariant Board::data(const QModelIndex &index, int role) const
{
    //temporary color array (DELETE LATER)
    QColor colors[] = {"red", "blue", "yellow", "black", "green"};
    switch (role) {
    case Qt::DecorationRole:
        return colors[(index.row()+index.column()) % 5];
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> Board::roleNames() const
{
    return { {Qt::DecorationRole, "colorBall"} };
}
