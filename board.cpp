#include "board.h"

#include <QQueue>
#include <QDebug>
#include <QPoint>

#include <utility>
#include <random>
#include <cmath>

Board::Board() :
    m_dimension{6}
  , m_selectedTileIndex{QModelIndex()}
  , m_colors{"red", "blue", "yellow", "grey", "green"}
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

void Board::popTiles(const QModelIndex& index)
{
    using Directions = QVector<QPoint>;

    QVector<QPoint> forPopping;
    static const Directions directions = {{-1,0}, {0,1}, {1,0}, {0,-1}};
    QPoint current(index.row(), index.column());
    auto inx = [this] (const QPoint& current) -> int
                {
                    return (m_dimension * current.x() + current.y());
                }; // index 2d to 1d array
    auto isValid = [this] (QPoint& p)
                    {
                        return ( (p.x() < 0)
                              || (p.y() < 0)
                              || (p.x() > m_dimension - 1)
                              || (p.y() > m_dimension - 1) ? false : true );
                    };
    QVector<bool> visited(m_dimension * m_dimension, false);
    QQueue<QPoint> queue;

    visited[inx(current)] = true;
    queue.enqueue(current);
    forPopping.append(current);

    while (!queue.empty())
    {
        current = queue.dequeue();
        qDebug() << current.x() << " " << current.y() << "\n";
        queue.dequeue();

        for (const auto& direction : directions)
        {
            auto forCheck = current + direction;
            if(isValid(forCheck) && !visited[inx(forCheck)] &&  m_data[current.x()][current.y()] == m_data[forCheck.x()][forCheck.y()])
            {
                visited[inx(forCheck)] = true;
                forPopping.append(forCheck);
                queue.enqueue(forCheck);
            }
        }
    }

    beginResetModel();
    for (auto& ball : forPopping)
    {
        m_data[ball.x()][ball.y()] = Qt::transparent;
    }
    endResetModel();
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

        if(m_selectedTileIndex.row() == tile.row()) {
            if(m_selectedTileIndex.column() - tile.column() == 1) {
                emit startAnimation(m_dimension * m_selectedTileIndex.column() + m_selectedTileIndex.row(), LEFT);
                emit startAnimation(m_dimension * tile.column() + tile.row(), RIGHT);
            } else {
                emit startAnimation(m_dimension * m_selectedTileIndex.column() + m_selectedTileIndex.row(), RIGHT);
                emit startAnimation(m_dimension * tile.column() + tile.row(), LEFT);
            }
        } else if(m_selectedTileIndex.column() == tile.column()) {
            if(m_selectedTileIndex.row() - tile.row() == 1) {
                emit startAnimation(m_dimension * m_selectedTileIndex.column() + m_selectedTileIndex.row(), UP);
                emit startAnimation(m_dimension * tile.column() + tile.row(), DOWN);
            } else {
                emit startAnimation(m_dimension * m_selectedTileIndex.column() + m_selectedTileIndex.row(), DOWN);
                emit startAnimation(m_dimension * tile.column() + tile.row(), UP);
            }
        }

        emit dataChanged(m_selectedTileIndex, m_selectedTileIndex);
        emit dataChanged(tile, tile);
        //        popTiles(tile);
        //        popTiles(m_selectedTileIndex);
        m_selectedTileIndex = QModelIndex();
    }
}

void Board::generateBoard()
{
    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<int> uniform_dist(0, m_colors.size()-1);

    for (int i = 0; i < m_dimension; ++i)
    {
        QVector<Tile> v;
        for (int j = 0; j < m_dimension; ++j)
        {
            v.append(m_colors[uniform_dist(e)]);
        }
        m_data.append(v);
    }
}


