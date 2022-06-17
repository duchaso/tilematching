#include "board.h"

#include <QQueue>

#include <utility>
#include <random>
#include <cmath>

namespace {
    constexpr int cVerticalDirection = 0;
    constexpr int cHorizontalDirection = 1;
    constexpr QPoint cInvalidPoint = {-1, -1};
}

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
  ,	m_state{BoardState::None}
  , m_firstItem{-1, -1}
  , m_secondItem{-1, -1}
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
    case Qt::UserRole + 2:
        return m_data[index.row()/m_dimension][index.row()%m_dimension].selected();
    default:
        break;
    }
    return QVariant();
}

QHash<int, QByteArray> Board::roleNames() const
{
    return { {Qt::DecorationRole, "color"}, {Qt::UserRole, "active"}, {Qt::UserRole + 1, "index"}, {Qt::UserRole + 2, "selected"} };
}

bool Board::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role) {
    case Qt::UserRole:
        m_data[index.row()/m_dimension][index.row()%m_dimension].setActive(value.toBool());
        emit dataChanged(index, index, {Qt::UserRole});
        return true;
    default:
        return false;
    }
}

void Board::doSomething(int inx)
{
    if (m_firstItem == cInvalidPoint && inx != -1)
    {
        m_state = BoardState::Select;
    }

    switch (m_state) {
    case BoardState::Select:
        m_firstItem = {inx / m_dimension, inx % m_dimension};
        m_data[m_firstItem.x()][m_firstItem.y()].setSelected(true);
        emit dataChanged(this->index(m_firstItem.x() * m_dimension + m_firstItem.y()),
                         this->index(m_firstItem.x() * m_dimension + m_firstItem.y()),
                         {Qt::UserRole + 2});
        m_state = BoardState::Swap;
        break;
    case BoardState::Swap:
        m_secondItem = {inx / m_dimension, inx % m_dimension};
        if (!isMovable(m_firstItem.x() * m_dimension + m_firstItem.y(),
                   m_secondItem.x() * m_dimension + m_secondItem.y()))
        {
            m_data[m_firstItem.x()][m_firstItem.y()].setSelected(false);
            emit dataChanged(this->index(m_firstItem.x() * m_dimension + m_firstItem.y()),
                             this->index(m_firstItem.x() * m_dimension + m_firstItem.y()),
                             {Qt::UserRole + 2});
            m_firstItem = m_secondItem;
            m_data[m_firstItem.x()][m_firstItem.y()].setSelected(true);
            emit dataChanged(this->index(m_firstItem.x() * m_dimension + m_firstItem.y()),
                             this->index(m_firstItem.x() * m_dimension + m_firstItem.y()),
                             {Qt::UserRole + 2});
        } else {
            m_data[m_firstItem.x()][m_firstItem.y()].setSelected(false);
            emit dataChanged(this->index(m_firstItem.x() * m_dimension + m_firstItem.y()),
                             this->index(m_firstItem.x() * m_dimension + m_firstItem.y()),
                             {Qt::UserRole + 2});
            move();
            m_state = BoardState::Pop;
        }
        break;
    case BoardState::Pop:
        if (!pop())
        {
            move();
            m_state = BoardState::None;
        } else {
            m_state = BoardState::Shift;
            doSomething();
        }
        break;
    case BoardState::Shift:
        m_state = BoardState::Fill;
        if (!shift())
        {
            doSomething();
        }
        break;
    case BoardState::Fill:
        fill();
        m_state = BoardState::None;
        break;
    default:
        m_firstItem = cInvalidPoint;
        break;
    }
}

bool Board::move()
{
    Tile::swapPosition(m_data[m_firstItem.x()][m_firstItem.y()], m_data[m_secondItem.x()][m_secondItem.y()]);
    emit dataChanged(this->index(m_firstItem.x() * m_dimension + m_firstItem.y()),
                     this->index(m_firstItem.x() * m_dimension + m_firstItem.y()));
    emit dataChanged(this->index(m_secondItem.x() * m_dimension + m_secondItem.y()),
                     this->index(m_secondItem.x() * m_dimension + m_secondItem.y()));
    std::swap(m_data[m_firstItem.x()][m_firstItem.y()], m_data[m_secondItem.x()][m_secondItem.y()]);
    return true;
}

bool Board::shift()
{
    bool isShifted = false;
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
                while (isValid(current) && m_data[current.x()][current.y()].color() == Qt::transparent)
                {
                    bottom = current;
                    current += m_directions[DOWN];
                }
                while (isValid(top))
                {
                    Tile::swapPosition(m_data[top.x()][top.y()], m_data[bottom.x()][bottom.y()]);
                    emit dataChanged(this->index(top.x() * m_dimension + top.y()),
                                     this->index(top.x() * m_dimension + top.y()),
                                     {Qt::UserRole + 1});
                    std::swap(m_data[top.x()][top.y()], m_data[bottom.x()][bottom.y()]);
                    isShifted = true;

                    top += m_directions[UP];
                    bottom += m_directions[UP];
                }
            }
        }
    }
    return isShifted;
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
    qDebug() << v;
    for (const auto& i : v)
    {
        m_data[i.x()][i.y()].setActive(true);
    }
    emit dataChanged(this->index(v.first().x() * m_dimension + v.first().y()),
                     this->index(v.last().x() * m_dimension + v.last().y()),
                     {Qt::UserRole});
}

void Board::restart()
{
    beginResetModel();
    generateBoard();
    endResetModel();
    m_state = BoardState::None;
    m_firstItem = cInvalidPoint;
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
    return !( (p.x() < 0)
              || (p.y() < 0)
              || (p.x() > m_dimension - 1)
              || (p.y() > m_dimension - 1) );
}

void Board::addForPopping(QVector<QPoint>& forPopping, int direction)
{
    QVector<QPoint> addForPopping;
    for (int i = direction; i < m_directions.length(); i += 2)
    {
        auto dir = m_directions[i];
        for (auto forCheck = forPopping[0] + dir;
             isValid(forCheck) && m_data[forPopping[0].x()][forPopping[0].y()] == m_data[forCheck.x()][forCheck.y()];
             forCheck += dir)
        {
            addForPopping.append(forCheck);
        }
    }
    if (addForPopping.size() > 1)
    {
        forPopping.append(addForPopping);
    }
}
bool Board::pop()
{
    setSteps(m_steps + 1);
    QVector<QPoint> points = {m_firstItem, m_secondItem};
    bool isPopped = false;

    beginResetModel();
    for (const auto& p : points)
    {
        QVector<QPoint> forPopping{p};
        addForPopping(forPopping, cVerticalDirection);
        addForPopping(forPopping, cHorizontalDirection);

        if (forPopping.size() > 2)
        {
            setScore(m_score + forPopping.size());
            for (const auto& ball : forPopping)
            {
                m_data[ball.x()][ball.y()].setColor(Qt::transparent);
            }
        }
        isPopped |= (forPopping.size() > 2) ? true : false;
    }
    endResetModel();

    if (m_score >= m_scoreToWin)
    {
        m_isWon = true;
        emit finished(m_isWon);
    } else if (m_steps == m_stepsToLose) {
        m_isWon = false;
        emit finished(m_isWon);
    }

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
