#include "tile.h"

Tile::Tile() :
    m_color(Qt::transparent)
  , m_pos(-1)
  , m_active(false)
{
}

Tile::Tile(const QColor &color, const int pos) :
    m_color(color)
  , m_pos(pos)
  , m_active(false)
{
}

Tile::Tile(const Tile &other) :
    m_color(other.m_color)
  , m_pos(other.m_pos)
  , m_active(other.m_active)
{
}

Tile::Tile(Tile &&other) :
    m_color(other.m_color)
  , m_pos(other.m_pos)
  , m_active(other.m_active)
{
    other.m_color = Qt::transparent;
    other.m_pos = {-1};
    other.m_active = false;
}

bool Tile::operator==(const Tile &other) const
{
    return m_color == other.m_color ? true : false;
}

void Tile::operator=(const QColor &color)
{
    m_color = color;
}
void Tile::operator=(Tile &&other)
{
    if (this != &other)
    {
        m_color = other.m_color;
        m_pos = other.m_pos;
        m_active = other.m_active;

        other.m_color = Qt::transparent;
        other.m_pos = {-1};
        other.m_active = false;
    }
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

void Tile::setPos(const int pos)
{
    m_pos = pos;
}

int Tile::pos() const
{
    return m_pos;
}

void Tile::swap(Tile &first, Tile &second)
{
    auto tmp = first.pos();
    first.setPos(second.pos());
    second.setPos(tmp);
}
