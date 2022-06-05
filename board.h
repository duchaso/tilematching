#ifndef BOARD_H
#define BOARD_H

#include <QAbstractListModel>
#include <QAbstractTableModel>
#include <QColor>
#include <QPoint>
#include <QDebug>
#include <QStack>

#include <utility>

#include "tile.h"

class Board : public QAbstractListModel
{
    Q_OBJECT

    using Matrix = QVector<QVector<Tile>>;
    using Colors = QVector<QColor>;

public:
    Board();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Q_INVOKABLE void move(int inx);

private:
    void generateBoard();
    QColor randColor(const QPoint& p) const;

private:
    const int m_dimension;
    Matrix m_data;
    Colors m_colors;
    QPoint m_selectedItem;
};

#endif // BOARD_H
