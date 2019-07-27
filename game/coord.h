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

#ifndef BOVO_COORD_H
#define BOVO_COORD_H

#include "common.h"

/** @file file declaring the Coord class */

/** namespace for game engine */
namespace bovo {

/**
 * A coordinate
 *
 * This class describes a coordinate on a playing field. It is
 * used as a container of an (x, y) coordinate pair that starts to
 * count with Origo (0, 0) in upper left corner. No negative coordinates
 * are allowed.
 *
 * If both x and y coordinates are static_cast<unsigned short int>(-1),
 * a special case is applied. Then the coordinate is invalid (all
 * coordinates outside of playing area are invalid) but it also means
 * that the coordinate refers to a moment before any moved has been played.
 * This can be queried with null().
 *
 * @code
 * Coord move(const Coord& coord) {
 *     if (coord.null()) {
 *         return computeAiTurn();
 *     } else {
 *         markCoordAsOpposite(coord);
 *         return computeAiTurn();
 *     }
 * }
 * @endcode
 */
class Coord {
public:
    /**
     * @brief standard constructor
     * @description constructs a Coord with given X and Y coordinates
     * @param x X coordinate
     * @param y Y coordinate
     */
    explicit Coord(usi x = -1, usi y = -1);

    /**
     * @brief copy constructor
     * @description constructs a Coord that is a copy of a given Coord
     * @param coord Coord to copy
     */
    Coord(const Coord& coord) = default;
    Coord& operator=(const Coord& coord) = default;

    /**
     * @brief
     * @description
     * @return
     */
    Coord down() const;

    /**
     * @brief
     * @description
     * @return
     */
    Coord left() const;

    /**
     * @brief is null?
     * @description tells if this coord is a null coordinate (-1, -1)
     * @return \c true if x == -1 and y == -1, \c false otherwise
     */
    bool null() const;

    /**
     * @brief
     * @description
     * @return
     */
    Coord right() const;

    /**
     * @brief
     * @description
     * @return
     */
    Coord up() const;

    /**
     * @brief
     * @description
     * @return
     */
    usi x() const;

    /**
     * @brief
     * @description
     * @return
     */
    usi y() const;

private:
    /* X coordinate property */
    usi m_x;

    /* Y coordinate property */
    usi m_y;
};

} /* namespace bovo */

#endif // BOVO_COORD_H
