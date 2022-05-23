#include "board.h"

#include <QQueue>
#include <QDebug>
#include <QPoint>

#include <utility>
#include <random>

Board::Board() : m_dimension(15), forMove(QModelIndex())
{
    m_colors = {"red", "blue", "yellow", "grey", "green"};
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
    auto abs = [](int n) { return (n > 0) ? n : -n; };

    return ( (index.row() == forMove.row() &&
              abs(index.column() - forMove.column()) == 1) ||
             (index.column() == forMove.column() &&
              abs(index.row() - forMove.row()) == 1) ) ? true : false;
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
                        return (p.x() < 0) || (p.y() < 0) ||
                                (p.x() > m_dimension - 1) ||
                                (p.y() > m_dimension - 1) ? false : true;
                    };
    QVector<bool> visited(m_dimension * m_dimension, false);
    QQueue<QPoint> queue;

    visited[inx(current)] = true;
    queue.enqueue(current);
    forPopping.append(current);

    while (!queue.empty())
    {
        current = queue.head();
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

    if (!forMove.isValid())
    {
        forMove = tile;
        m_data[forMove.row()][forMove.column()].setActive(true);
        emit dataChanged(forMove, forMove);
    }
    else if (isMovable(tile)) {
        m_data[forMove.row()][forMove.column()].setActive(false);
        std::swap(m_data[tile.row()][tile.column()], m_data[forMove.row()][forMove.column()]);

        if(forMove.row() == tile.row()) {
            if(forMove.column() - tile.column() == 1) {
                emit startAnimation(m_dimension * forMove.column() + forMove.row(), LEFT);
                emit startAnimation(m_dimension * tile.column() + tile.row(), RIGHT);
            } else {
                emit startAnimation(m_dimension * forMove.column() + forMove.row(), RIGHT);
                emit startAnimation(m_dimension * tile.column() + tile.row(), LEFT);
            }
        } else if(forMove.column() == tile.column()) {
            if(forMove.row() - tile.row() == 1) {
                emit startAnimation(m_dimension * forMove.column() + forMove.row(), UP);
                emit startAnimation(m_dimension * tile.column() + tile.row(), DOWN);
            } else {
                emit startAnimation(m_dimension * forMove.column() + forMove.row(), DOWN);
                emit startAnimation(m_dimension * tile.column() + tile.row(), UP);
            }
        }

        emit dataChanged(forMove, forMove);
        emit dataChanged(tile, tile);
//        popTiles(tile);
//        popTiles(forMove);
        forMove = QModelIndex();
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


