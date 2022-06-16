#ifndef BOARD_H
#define BOARD_H

#include <QAbstractListModel>

#include "tile.h"

class Board : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(int steps READ steps NOTIFY stepsChanged)
    Q_PROPERTY(int scoreToWin READ scoreToWin CONSTANT)
    Q_PROPERTY(int stepsToLose READ stepsToLose CONSTANT)

    using Matrix = QVector<QVector<Tile>>;
    using Colors = QVector<QColor>;
    using Directions = QVector<QPoint>;

    enum Direction {
        UP,
        RIGHT,
        DOWN,
        LEFT,
    };

    enum class BoardState {
        Select,
        Swap,
        Pop,
        Shift,
        Fill,
        None,
    };

public:
    Board();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Q_INVOKABLE void doSomething(int inx = -1);
    Q_INVOKABLE void restart();

    void setScore(int score);
    void setSteps(int steps);

    int score() const;
    int steps() const;
    int scoreToWin() const;
    int stepsToLose() const;

signals:
    void finished(bool isWon);
    void scoreChanged();
    void stepsChanged();

private:
    bool move();
    bool pop();
    bool shift();
    void fill();
    bool isMovable(int inx1, int inx2) const;
    bool isValid(const QPoint& p);
    void generateBoard();
    QColor randColor(const QPoint& p) const;
    void addForPopping(QVector<QPoint>& forPopping, int direction);

private:
    const int m_dimension;
    Matrix m_data;
    Colors m_colors;
    const Directions m_directions;
    bool m_isWon;
    int m_score;
    int m_steps;
    const int m_scoreToWin;
    const int m_stepsToLose;
    BoardState m_state;
    QPoint m_firstItem;
    QPoint m_secondItem;

};

#endif // BOARD_H
