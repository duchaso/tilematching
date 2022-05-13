#include "board.h"

#include <utility>
#include <random>

Board::Board() : m_dimension(15)
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
