#ifndef BOARD_H
#define BOARD_H

#include <QAbstractTableModel>

class Board : public QAbstractTableModel
{
    Q_OBJECT
public:
    Board();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
};

#endif // BOARD_H
