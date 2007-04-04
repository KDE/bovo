/*******************************************************************
*
* This file is part of the KDE project "Bovo"
*
* Bovo is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* Bovo is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Bovo; see the file COPYING.  If not, write to
* the Free Software Foundation, 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*
********************************************************************/                     

#include "coord.h"

namespace bovo {

Coord::Coord(usi x, usi y): m_x(x), m_y(y) {
}

Coord::Coord(const Coord& c) : m_x(c.m_x), m_y(c.m_y) {
}

usi Coord::x() const {
    return m_x;
}

usi Coord::y() const {
    return m_y;
}

Coord Coord::left() const {
    return Coord(m_x-1, m_y);
}

Coord Coord::right() const {
    return Coord(m_x+1, m_y);
}

Coord Coord::up() const {
    return Coord(m_x, m_y-1);
}

Coord Coord::down() const {
    return Coord(m_x, m_y+1);
}

} /* namespace bovo */
