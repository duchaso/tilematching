#ifndef TILE_H
#define TILE_H

#include <QColor>
#include <QPoint>
#include <QDebug>

class Tile
{
public:
    Tile();
    Tile(const QColor& color, const int pos);
    Tile(const Tile& other);
    Tile(Tile&& other);

    bool operator==(const Tile& other) const;
    void operator=(Tile&& other);

    void setColor(const QColor& color);
    QColor color() const;

    void setActive(bool state);
    bool isActive() const;

    void setPos(const int pos);
    int pos() const;

    static void swapPosition(Tile& first, Tile& second);

private:
    QColor m_color;
    int m_pos;
    bool m_active;
};

#endif // TILE_H
