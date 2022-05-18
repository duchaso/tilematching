#include "tile.h"

Tile::Tile() : m_color(Qt::transparent), m_active(false)
{
}

Tile::Tile(const QColor &color) : m_color(color), m_active(false)
{
}

Tile::Tile(const Tile &other)
{
    m_color = other.m_color;
    m_active = other.m_active;
}

bool Tile::operator==(const Tile &other) const
{
    return m_color == other.m_color ? true : false;
}

void Tile::operator=(const Qt::GlobalColor &globalColor)
{
    m_color = globalColor;
}

void Tile::setColor(const QColor &color)
{
    m_color = color;
}

QColor Tile::color() const
{
    return m_color;
}

void Tile::setActive(bool state)
{
    m_active = state;
}

bool Tile::isActive() const
{
    return m_active;
}
