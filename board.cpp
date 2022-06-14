#include "board.h"

#include <QQueue>
#include <QDebug>

#include <utility>
#include <random>
#include <cmath>

Board::Board() :
    m_dimension{6}
  ,	m_data{Matrix(m_dimension, QVector<Tile>(m_dimension))}
  , m_colors{"red", "blue", "yellow", "grey", "green"}
  , m_directions{{-1,0}, {0,1}, {1,0}, {0,-1}}
  , m_isWon{false}
  , m_score{0}
  , m_steps{0}
  , m_scoreToWin{10}
  , m_stepsToLose{3}
{
    generateBoard();
}

int Board::rowCount(const QModelIndex &parent) const
{
    return m_dimension * m_dimension;
}

QVariant Board::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DecorationRole:
        return m_data[index.row()/m_dimension][index.row()%m_dimension].color();
    case Qt::UserRole:
        return m_data[index.row()/m_dimension][index.row()%m_dimension].isActive();
    case Qt::UserRole + 1:
        return m_data[index.row()/m_dimension][index.row()%m_dimension].pos();
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> Board::roleNames() const
{
    return { {Qt::DecorationRole, "color"}, {Qt::UserRole, "active"}, {Qt::UserRole + 1, "index"} };
}

bool Board::move(int inx1, int inx2)
{
    if (!isMovable(inx1, inx2))
    {
        return false;
    } else {
        QPoint m_firstMovedItem = QPoint{inx1 / m_dimension, inx1 % m_dimension};
        QPoint m_secondMovedItem = QPoint{inx2 / m_dimension, inx2 % m_dimension};
        Tile::swapPosition(m_data[m_firstMovedItem.x()][m_firstMovedItem.y()], m_data[m_secondMovedItem.x()][m_secondMovedItem.y()]);
        emit dataChanged(this->index(m_firstMovedItem.x() * m_dimension + m_firstMovedItem.y()),
                         this->index(m_firstMovedItem.x() * m_dimension + m_firstMovedItem.y()),
                         {Qt::UserRole + 1});
        emit dataChanged(this->index(m_secondMovedItem.x() * m_dimension + m_secondMovedItem.y()),
                         this->index(m_secondMovedItem.x() * m_dimension + m_secondMovedItem.y()),
                         {Qt::UserRole + 1});
        std::swap(m_data[m_firstMovedItem.x()][m_firstMovedItem.y()], m_data[m_secondMovedItem.x()][m_secondMovedItem.y()]);
        return true;
    }
}

bool Board::shift()
{
    bool waitForFill = true;
    for (int i = 1; i < m_dimension; ++i)
    {
        for (int j = 0; j < m_dimension; ++j)
        {
            QPoint current(i, j);
            if (m_data[i][j].color() == Qt::transparent)
            {
                QPoint top = current + m_directions[UP];
                QPoint bottom;
                if (m_data[top.x()][top.y()].color() == Qt::transparent)
                {
                    continue;
                }
                for (;isValid(current) && m_data[current.x()][current.y()].color() == Qt::transparent; current += m_directions[DOWN])
                {
                    bottom = current;
                }
                for (; isValid(top); top += m_directions[UP], bottom += m_directions[UP])
                {
                    Tile::swapPosition(m_data[top.x()][top.y()], m_data[bottom.x()][bottom.y()]);
                    emit dataChanged(this->index(top.x() * m_dimension + top.y()),
                                     this->index(top.x() * m_dimension + top.y()),
                                     {Qt::UserRole + 1});
                    std::swap(m_data[top.x()][top.y()], m_data[bottom.x()][bottom.y()]);
                    waitForFill = false;
                }
            }
        }
    }
    return waitForFill;
}

void Board::fill()
{
    QVector<QPoint> v;
    beginResetModel();
    for (int i = 0; i < m_dimension; ++i)
    {
        for (int j = 0; j < m_dimension; ++j)
        {
            if (m_data[i][j].color() == Qt::transparent)
            {
                m_data[i][j].setColor(randColor({i, j}));
                v.append({i, j});
            }
        }
    }
    endResetModel();
    for (const auto& i : v)
    {
        m_data[i.x()][i.y()].setActive(true);
        emit dataChanged(this->index(i.x() * m_dimension + i.y()),
                 this->index(i.x() * m_dimension + i.y()),
                 {Qt::UserRole});
        m_data[i.x()][i.y()].setActive(false);
    }
}

void Board::restart()
{
    beginResetModel();
    generateBoard();
    endResetModel();
    setScore(0);
    setSteps(0);
}

bool Board::isMovable(int inx1, int inx2) const
{
    return ( inx1 + m_dimension == inx2 ||
             inx1 - m_dimension == inx2 ||
             inx1 + 1			== inx2 ||
             inx1 - 1			== inx2	) ? true : false;
}

bool Board::isValid(const QPoint &p)
{
    return ( (p.x() < 0)
              || (p.y() < 0)
              || (p.x() > m_dimension - 1)
              || (p.y() > m_dimension - 1) ? false : true );
}

bool Board::pop(int inx1, int inx2)
{
    setSteps(m_steps + 1);
    QVector<QPoint> points = {{inx1 / m_dimension, inx1 % m_dimension},
                              {inx2 / m_dimension, inx2 % m_dimension}};
    bool isPopped = false;

    beginResetModel();
    for (const auto& p : points)
    {
        QVector<QPoint> forPoppingH{p};
        QVector<QPoint> forPoppingV{p};

        for (int i = 0; i < m_directions.length(); i += 2)
        {
            auto direction = m_directions[i];
            for (auto forCheck = p + direction;
                 isValid(forCheck) && m_data[p.x()][p.y()] == m_data[forCheck.x()][forCheck.y()];
                 forCheck += direction)
            {
                forPoppingH.append(forCheck);
            }
        }
        for (int i = 1; i < m_directions.length(); i += 2)
        {
            auto direction = m_directions[i];
            for (auto forCheck = p + direction;
                 isValid(forCheck) && m_data[p.x()][p.y()] == m_data[forCheck.x()][forCheck.y()];
                 forCheck += direction)
            {
                forPoppingV.append(forCheck);
            }
        }

        if (forPoppingH.size() > 2)
        {
            setScore(score() + forPoppingH.size());
            for (auto& ball : forPoppingH)
            {
                m_data[ball.x()][ball.y()].setColor(Qt::transparent);
            }
        }
        if (forPoppingV.size() > 2)
        {
            setScore(score() + forPoppingV.size());
            for (auto& ball : forPoppingV)
            {
                m_data[ball.x()][ball.y()].setColor(Qt::transparent);
            }
        }

        if (m_score >= m_scoreToWin)
        {
            m_isWon = true;
            emit finished();
        } else if (m_steps == m_stepsToLose) {
            m_isWon = false;
            emit finished();
        }
        isPopped |= (forPoppingH.size() > 2 || forPoppingV.size() > 2) ? true : false;
    }
    endResetModel();

    return isPopped;
}

void Board::generateBoard()
{
    static bool isCalled = false;

    if (!isCalled)
    {
        for (int i = 0; i < m_dimension; ++i)
        {
            for (int j = 0; j < m_dimension; ++j)
            {
                m_data[i][j] = Tile(randColor(QPoint(i, j)), i * m_dimension + j);
            }
        }
        isCalled = true;
    } else {
        for (int i = 0; i < m_dimension; ++i)
        {
            for (int j = 0; j < m_dimension; ++j)
            {
                m_data[i][j].setColor(randColor(QPoint(i, j)));
            }
        }
    }
}

QColor Board::randColor(const QPoint &p) const
{
    std::random_device rd;
    std::default_random_engine e(rd());

    Colors availableColors = m_colors;
    QColor prevTop;
    QColor prevLeft;

    prevTop = (p.x() != 0) ? m_data[p.x()-1][p.y()].color() : QColor();
    prevLeft = (p.y() != 0) ? m_data[p.x()][p.y()-1].color() : QColor();
    availableColors.removeOne(prevTop);
    availableColors.removeOne(prevLeft);
    std::uniform_int_distribution<int> uniform_dist(0, availableColors.size() - 1);
    return availableColors[uniform_dist(e)];
}

void Board::setScore(int score)
{
    m_score = score;
    emit scoreChanged();
}

void Board::setSteps(int steps)
{
    m_steps = steps;
    emit stepsChanged();
}

bool Board::isWon() const
{
    return m_isWon;
}

int Board::score() const
{
    return m_score;
}

int Board::steps() const
{
    return m_steps;
}

int Board::scoreToWin() const
{
    return m_scoreToWin;
}

int Board::stepsToLose() const
{
    return m_stepsToLose;
}
