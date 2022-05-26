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

    auto isValid = [this] (QPoint& p)
                    {
                        return ( (p.x() < 0)
                              || (p.y() < 0)
                              || (p.x() > m_dimension - 1)
                              || (p.y() > m_dimension - 1) ? false : true );
                    };


    QPoint current(index.row(), index.column());
    QVector<QPoint> forPoppingH{current};
    QVector<QPoint> forPoppingV{current};
    static const Directions directions = {{-1,0}, {0,1}, {1,0}, {0,-1}};

    for (const auto& direction : directions)
    {
        if (direction == directions[LEFT] || direction == directions[RIGHT])
        {
            for (auto forCheck = current + direction;
                 isValid(forCheck) && m_data[current.x()][current.y()] == m_data[forCheck.x()][forCheck.y()];
                 forCheck += direction)
            {
                forPoppingV.append(forCheck);
            }
        } else {
            for (auto forCheck = current + direction;
                 isValid(forCheck) && m_data[current.x()][current.y()] == m_data[forCheck.x()][forCheck.y()];
                 forCheck += direction)
            {
                forPoppingH.append(forCheck);
            }
        }
    }

    if (forPoppingH.size() > 2) {
        beginResetModel();
        for (auto& ball : forPoppingH)
        {
            m_data[ball.x()][ball.y()] = Qt::transparent;
        }
    }
    if (forPoppingV.size() > 2) {
        beginResetModel();
        for (auto& ball : forPoppingV)
        {
            m_data[ball.x()][ball.y()] = Qt::transparent;
        }
    }
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
    std::random_device rd;
    std::default_random_engine e(rd());

    Colors availableColors = m_colors;
    QColor prevTop;
    QColor prevLeft;

    for (int i = 0; i < m_dimension; ++i)
    {
        QVector<Tile> v;
        for (int j = 0; j < m_dimension; ++j)
        {
            availableColors = m_colors;
            prevTop = (i != 0) ? m_data[i-1][j].color() : QColor();
            prevLeft = (j != 0) ? v[j-1].color() : QColor();
            availableColors.removeOne(prevTop);
            availableColors.removeOne(prevLeft);
            std::uniform_int_distribution<int> uniform_dist(0, availableColors.size()-1);
            v.append(availableColors[uniform_dist(e)]);
        }
        m_data.append(v);
    }
}


