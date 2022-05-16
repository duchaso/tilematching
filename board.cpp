#include "board.h"

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
        return m_data[index.row()][index.column()];
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> Board::roleNames() const
{
    return { {Qt::DecorationRole, "colour"} };
}

bool Board::isMovable(const QModelIndex &index)
{
    auto abs = [](int n) { return (n > 0) ? n : -n; };

    return ( (index.row() == forMove.row() && abs(index.column() - forMove.column()) == 1)
          || (index.column() == forMove.column() && abs(index.row() - forMove.row()) == 1) ) ? true : false;
}

void Board::moveTile(const QModelIndex &tile)
{
    if (!forMove.isValid())
    {
        forMove = tile;
    }
    else if (isMovable(tile)) {
        std::swap(m_data[tile.row()][tile.column()], m_data[forMove.row()][forMove.column()]);
        emit dataChanged(forMove, forMove);
        emit dataChanged(tile, tile);
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
        Colors v;
        for (int j = 0; j < m_dimension; ++j)
        {
            v.append(m_colors[uniform_dist(e)]);
        }
        m_data.append(v);
    }
}


