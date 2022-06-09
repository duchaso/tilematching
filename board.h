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
    Q_PROPERTY(int score READ score WRITE setScore NOTIFY scoreChanged)
    Q_PROPERTY(int steps READ steps WRITE setSteps NOTIFY stepsChanged)
    Q_PROPERTY(int scoreToWin READ scoreToWin WRITE setScoreToWin NOTIFY scoreToWinChanged)
    Q_PROPERTY(int stepsToLose READ stepsToLose WRITE setStepsToLose NOTIFY stepsToLoseChanged)
    Q_PROPERTY(bool isWon READ isWon NOTIFY isWonChanged)

    using Matrix = QVector<QVector<Tile>>;
    using Colors = QVector<QColor>;

public:
    Board();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE bool move(int inx1, int inx2);

private:
    bool isMovable(int inx1, int inx2) const;
    void generateBoard();

    QColor randColor(const QPoint& p) const;

private:
    const int m_dimension;
    Matrix m_data;
    Colors m_colors;
    QPoint m_selectedItem;
};

#endif // BOARD_H
