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
  coord::coord(const unsigned short int _x, const unsigned short int _y): x(_x), y(_y) {
  }

  coord::coord(const coord& c) : x(c.x), y(c.y) {
  }

  unsigned short int coord::row() const {
    return x;
  }

  unsigned short int coord::col() const {
    return y;
  }

  coord coord::left() const {
    return coord(x-1, y);
  }

  coord coord::right() const {
    return coord(x+1, y);
  }

  coord coord::leftup() const {
    return coord(x-1, y-1);
  }

  coord coord::leftdown() const {
    return coord(x-1, y+1);
  }

  coord coord::rightup() const {
    return coord(x+1, y-1);
  }

  coord coord::rightdown() const {
    return coord(x+1, y+1);
  }

  coord coord::up() const {
    return coord(x, y-1);
  }

  coord coord::down() const {
    return coord(x, y+1);
  }

} // namespace bovo

