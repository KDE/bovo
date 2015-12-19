/*******************************************************************
*
* Copyright 2007  Aron Boström <c02ab@efd.lth.se>
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

#include "dimension.h"
#include "coord.h"

namespace bovo {

Dimension::Dimension(usi width, usi height) 
  : m_height(height), m_width(width) {
}

Dimension::Dimension(const Dimension& dimension) 
  : m_height(dimension.m_height), m_width(dimension.m_width) {
}

usi Dimension::height() const {
    return m_height;
}

usi Dimension::width() const {
    return m_width;
}

bool Dimension::ok(const Coord* c) const {
    return c->x() < m_width && c->y() < m_height;
}

bool Dimension::ok(const Coord& c) const {
    return c.x() < m_width && c.y() < m_height;
}

} /* namespace bovo */
