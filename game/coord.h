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

#ifndef __COORD_H__
#define __COORD_H__

#include "common.h"

namespace bovo {

/** 
 * Coordinate
 */
class Coord {
public:
    Coord(usi x, usi y);
    Coord(const Coord& c);
    usi x() const;
    usi y() const;
    Coord left() const;
    Coord right() const;
    Coord up() const;
    Coord down() const;

private:
    usi m_x;
    usi m_y;
};

} /* namespace bovo */

#endif /* __COORD_H__ */
