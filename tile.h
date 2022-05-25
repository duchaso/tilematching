#ifndef TILE_H
#define TILE_H

#include <QColor>

class Tile
{
public:
    Tile();
    Tile(const QColor& color);
    Tile(const Tile& other);

    bool operator==(const Tile& other) const;
    void operator=(const Qt::GlobalColor& globalColor);

    void setColor(const QColor& color);
    QColor color() const;

    void setActive(bool state);
    bool isActive() const;
private:
    QColor m_color;
    bool m_active;
};

#endif // TILE_H
