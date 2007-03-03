/*******************************************************************
*
* Copyright 2002,2007 Aron Boström <aron.bostrom@gmail.com>
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

#ifndef COORD_H
#define COORD_H

namespace bovo {
  /** 
   * Coordinate
   */
  class coord {
  public:
    coord(const unsigned short int col, const unsigned short int row) : x(col), y(row) {}
    coord(const coord& c) : x(c.x), y(c.y) {}
    unsigned short int x, y;
    unsigned short int row() const { return x; }
    unsigned short int col() const { return y; }
    coord left() const { return coord(x-1, y); }
    coord right() const { return coord(x+1, y); }
    coord leftup() const { return coord(x-1, y-1); }
    coord leftdown() const { return coord(x-1, y+1); }
    coord rightup() const { return coord(x+1, y-1); }
    coord rightdown() const { return coord(x+1, y+1); }
    coord up() const { return coord(x, y-1); }
    coord down() const { return coord(x, y+1); }
  };
}

#endif
