#include "board.h"

#include <QQueue>
#include <QDebug>

#include <utility>
#include <random>
#include <cmath>

Board::Board() :
    m_dimension{6}
  , m_data{Matrix(m_dimension, QVector<Tile>(m_dimension))}
  , m_selectedTileIndex{QModelIndex()}
  , m_colors{"red", "blue", "yellow", "grey", "green"}
  , m_directions{{-1,0}, {0,1}, {1,0}, {0,-1}}
{
    generateBoard();
}

int Board::rowCount(const QModelIndex &parent) const
{
    return m_dimension;
}

int Board::columnCount(const QModelIndex &parent) const
{
    return m_dimension;
}

QVariant Board::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DecorationRole:
        return m_data[index.row()][index.column()].color();
    case Qt::UserRole:
        return m_data[index.row()][index.column()].isActive();
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> Board::roleNames() const
{
    return { {Qt::DecorationRole, "colour"}, {Qt::UserRole, "active"} };
}

bool Board::isMovable(const QModelIndex &index)
{
    return ( (index.row() == m_selectedTileIndex.row() &&
              std::abs(index.column() - m_selectedTileIndex.column()) == 1) ||
             (index.column() == m_selectedTileIndex.column() &&
              std::abs(index.row() - m_selectedTileIndex.row()) == 1) ) ? true : false;
}

bool Board::popTiles(const QModelIndex& index)
{
    QPoint current(index.row(), index.column());
    QVector<QPoint> forPopping{current};

    for (const auto& direction : m_directions)
    {
        for (auto forCheck = current + direction;
             isValid(forCheck) && m_data[current.x()][current.y()] == m_data[forCheck.x()][forCheck.y()];
             forCheck += direction)
        {
            forPopping.append(forCheck);
        }
    }

    if (forPopping.size() > 2) {
        for (auto& ball : forPopping)
        {
            m_data[ball.x()][ball.y()] = Qt::transparent;
        }
        shift(forPopping);
    }
    return (forPopping.size() > 2 ) ? true : false;
}

bool Board::isValid(QPoint &p)
{
    return ( (p.x() < 0)
              || (p.y() < 0)
              || (p.x() > m_dimension - 1)
              || (p.y() > m_dimension - 1) ? false : true );
}

void Board::shift(QVector<QPoint> &poppedTiles)
{
    for (auto cur : poppedTiles) {
        for (; isValid(cur) && m_data[cur.x()][cur.y()].color() == Qt::transparent; cur += m_directions[UP]);
        for (; isValid(cur); cur += m_directions[UP])
        {
            for (auto next = cur + m_directions[DOWN];
                 isValid(next) && m_data[next.x()][next.y()].color() == Qt::transparent;
                 next += m_directions[DOWN])
            {
                auto prev = next + m_directions[UP];
                std::swap(m_data[next.x()][next.y()], m_data[prev.x()][prev.y()]);
            }
        }
    }
}

void Board::fillEmpty()
{
    QPoint current;
    QVector<QPoint> toFill;
    QVector<QVector<bool>> visited(m_dimension, QVector<bool>(m_dimension, false));
    QQueue<QPoint> queue;

    bool found = false;
    for (int i = 0; !found && i < m_data.size(); ++i)
    {
        for (int j = 0; j < m_data.first().size(); ++j)
        {
            if (m_data[i][j].color() == Qt::transparent)
            {
                found = true;
                current = {i, j};
                break;
            }
        }
    }

    visited[current.x()][current.y()] = true;
    queue.enqueue(current);
    toFill.append(current);

    while (!queue.empty())
    {
        current = queue.head();
        queue.dequeue();

        for (const auto& direction : m_directions)
        {
            auto check = current + direction;
            if (	isValid(check)
                    && !visited[check.x()][check.y()]
                    && m_data[check.x()][check.y()].color() == Qt::transparent)
            {
                visited[check.x()][check.y()] = true;
                toFill.append(check);
                queue.enqueue(check);
            }
        }
    }

    for (const auto& cur : toFill)
    {
        m_data[cur.x()][cur.y()] = randColor(cur);
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

void Board::moveTile(const QModelIndex &tile)
{
    if (!m_selectedTileIndex.isValid())
    {
        m_selectedTileIndex = tile;
        m_data[m_selectedTileIndex.row()][m_selectedTileIndex.column()].setActive(true);
        emit dataChanged(m_selectedTileIndex, m_selectedTileIndex);
    } else if (isMovable(tile)) {
        m_data[m_selectedTileIndex.row()][m_selectedTileIndex.column()].setActive(false);
        std::swap(m_data[tile.row()][tile.column()], m_data[m_selectedTileIndex.row()][m_selectedTileIndex.column()]);

        emit dataChanged(m_selectedTileIndex, m_selectedTileIndex);
        emit dataChanged(tile, tile);

        if (m_selectedTileIndex.row() == tile.row()) {
            if (m_selectedTileIndex.column() - tile.column() == 1) {
                emit startAnimation(m_dimension * m_selectedTileIndex.column() + m_selectedTileIndex.row(), LEFT);
                emit startAnimation(m_dimension * tile.column() + tile.row(), RIGHT);
            } else {
                emit startAnimation(m_dimension * m_selectedTileIndex.column() + m_selectedTileIndex.row(), RIGHT);
                emit startAnimation(m_dimension * tile.column() + tile.row(), LEFT);
            }
        } else if (m_selectedTileIndex.column() == tile.column()) {
            if (m_selectedTileIndex.row() - tile.row() == 1) {
                emit startAnimation(m_dimension * m_selectedTileIndex.column() + m_selectedTileIndex.row(), UP);
                emit startAnimation(m_dimension * tile.column() + tile.row(), DOWN);
            } else {
                emit startAnimation(m_dimension * m_selectedTileIndex.column() + m_selectedTileIndex.row(), DOWN);
                emit startAnimation(m_dimension * tile.column() + tile.row(), UP);
            }
        }

        popTiles(tile);
        popTiles(m_selectedTileIndex);
        fillEmpty();

        m_selectedTileIndex = QModelIndex();

    } else {
        m_data[m_selectedTileIndex.row()][m_selectedTileIndex.column()].setActive(false);
        emit dataChanged(m_selectedTileIndex, m_selectedTileIndex);
        m_selectedTileIndex = tile;
        m_data[m_selectedTileIndex.row()][m_selectedTileIndex.column()].setActive(true);
        emit dataChanged(m_selectedTileIndex, m_selectedTileIndex);
    }
}

void Board::update()
{
    beginResetModel();
    endResetModel();
}

void Board::generateBoard()
{
    for (int i = 0; i < m_dimension; ++i)
    {
        for (int j = 0; j < m_dimension; ++j)
        {
            m_data[i][j] = randColor(QPoint(i, j));
        }
    }
}
