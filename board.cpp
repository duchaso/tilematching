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
    return { {Qt::DecorationRole, "colorBall"} };
}

void Board::generateBoard()
{
    Colors temp;

    for(auto& color : m_colors)
    {
        for(int i = 0; i < m_dimension * m_dimension / m_colors.size(); ++i)
        {
            temp.append(color);
        }
    }

    std::random_device rd;
    auto rng = std::default_random_engine{rd()};
    std::shuffle(temp.begin(), temp.end(), rng);

    int cnt = 0;
    for(int i = 0; i < m_dimension; ++i)
    {
        Colors v;
        for(int j = 0; j < m_dimension; ++j)
        {
            v.append(temp[cnt]);
            ++cnt;
        }
        m_data.append(v);
    }

}
