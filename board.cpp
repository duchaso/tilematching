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
  , m_selectedItem{-1, -1}
{
    generateBoard();
}

int Board::rowCount(const QModelIndex &parent) const
{
    return m_dimension*m_dimension;
}

QVariant Board::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DecorationRole:
        return m_data[index.row()/m_dimension][index.row()%m_dimension].color();
    case Qt::UserRole:
        return m_data[index.row()/m_dimension][index.row()%m_dimension].isActive();
    case Qt::UserRole + 1:
        return m_data[index.row()/m_dimension][index.row()%m_dimension].pos().x();
    case Qt::UserRole + 2:
        return m_data[index.row()/m_dimension][index.row()%m_dimension].pos().y();
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> Board::roleNames() const
{
    return { {Qt::DecorationRole, "color"}, {Qt::UserRole, "active"}, {Qt::UserRole + 1, "x"}, {Qt::UserRole + 2, "y"} };
}

bool Board::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int n = value.toInt();
    QPoint p = m_data[index.row()/m_dimension][index.row()%m_dimension].pos();
    switch (role) {
    case Qt::UserRole + 1:
        p.setX(n);
        m_data[index.row()/m_dimension][index.row()%m_dimension].setPos(p);
        return true;
    case Qt::UserRole + 2:
        p.setY(n);
        m_data[index.row()/m_dimension][index.row()%m_dimension].setPos(p);
        return true;
    default:
        return false;
    }
}

void Board::move(int inx)
{
    if (m_selectedItem == QPoint{-1, -1})
    {
        m_selectedItem = {inx / m_dimension, inx % m_dimension};
    } else {
        QPoint p(inx / m_dimension, inx % m_dimension);
        std::swap(m_data[m_selectedItem.x()][m_selectedItem.y()], m_data[p.x()][p.y()]);
        emit dataChanged(this->index(m_selectedItem.x() * m_dimension + m_selectedItem.y()), this->index(m_selectedItem.x() * m_dimension + m_selectedItem.y()));
        emit dataChanged(this->index(p.x() * m_dimension + p.y()), this->index(p.x() * m_dimension + p.y()));
        m_selectedItem = {-1, -1};
    }
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
                m_data[i][j] = Tile(randColor(QPoint(i, j)), QPoint(i,j));
            }
        }
        isCalled = true;
    } else {
        for (int i = 0; i < m_dimension; ++i)
        {
            for (int j = 0; j < m_dimension; ++j)
            {
                m_data[i][j] = randColor(QPoint(i, j));
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
