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
    coord(const unsigned short int col, const unsigned short int row);
    coord(const coord& c);
    unsigned short int x, y;
    unsigned short int row() const;
    unsigned short int col() const;
    coord left() const;
    coord right() const;
    coord leftup() const;
    coord leftdown() const;
    coord rightup() const;
    coord rightdown() const;
    coord up() const;
    coord down() const;
  };
} // namespace bovo

#endif
