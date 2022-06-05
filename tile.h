#ifndef TILE_H
#define TILE_H

#include <QColor>
#include <QPoint>
#include <QDebug>

class Tile
{
public:
    Tile();
    Tile(const QColor& color, const QPoint& pos);
    Tile(const Tile& other);
    Tile(Tile&& other);

    bool operator==(const Tile& other) const;
    void operator=(const QColor& color);
    void operator=(Tile&& other);

    void setColor(const QColor& color);
    QColor color() const;

    void setActive(bool state);
    bool isActive() const;

    void setPos(const QPoint& pos);
    QPoint pos() const;

    static void swap(Tile& first, Tile& second);
private:
    QColor m_color;
    QPoint m_pos;
    bool m_active;
};

#endif // TILE_H
