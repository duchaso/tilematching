#ifndef BOARD_H
#define BOARD_H

#include <QAbstractTableModel>
#include <QColor>

class Board : public QAbstractTableModel
{
    Q_OBJECT

    using Colors = QVector<QColor>;
    using Matrix = QVector<Colors>;

public:
    Board();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void generateBoard();

private:
    Matrix m_data;
    Colors m_colors;
    int m_dimension;
};

#endif // BOARD_H
