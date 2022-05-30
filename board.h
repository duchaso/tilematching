#ifndef BOARD_H
#define BOARD_H

#include <QAbstractTableModel>
#include <QColor>
#include <QPoint>

#include "tile.h"

class Board : public QAbstractTableModel
{
    Q_OBJECT

    using Colors = QVector<QColor>;
    using Matrix = QVector<QVector<Tile>>;
    using Directions = QVector<QPoint>;


    enum Direction {
        UP,
        RIGHT,
        DOWN,
        LEFT,
    };

public:
    Board();


    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void moveTile(const QModelIndex& tile);
    Q_INVOKABLE void update();

signals:
    void startAnimation(int index, int direction);

private:
    void generateBoard();
    bool isMovable(const QModelIndex& index);
    bool popTiles(const QModelIndex& index);
    bool isValid(QPoint& p);
    void shift(QVector<QPoint>& poppedTiles);
    void fillEmpty();
    QColor randColor(const QPoint& p) const;

private:
    const int m_dimension;
    Matrix m_data;
    Colors m_colors;
    QModelIndex m_selectedTileIndex;
    const Directions m_directions;
};

#endif // BOARD_H
