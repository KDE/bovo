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

#ifndef BOVO_DIMENSION_H
#define BOVO_DIMENSION_H

#include "common.h"

/** @file file declaring the Dimension class */

/** namespace for game engine */
namespace bovo {

class Coord;

/** 
 * Dimension is logic container of the width and height of a playing board
 *
 * Dimension keeps the size data of a playing board, that is how wide and how
 * high the board is. It can also be used to verify a given Coord is inside
 * the limits of the Dimension (i.e. the Coord is a valid Coord on the playing
 * board).
 *
 * @code
 * Dimension dim1(15, 15);
 * Dimension dim2(dim1);
 * Coord coord(18, 7);
 * dim1.ok(coord); // <i>returns false</i>
 * for (int i = 0; i < dim2.width(); ++i) {
 * }
 * @endcode
 */
class Dimension {
public:
    /**
     * @brief standard constructor
     * @description constructs a Dimension with a certain width and height
     * @param width the number of columns of this Dimension 
     * @param height the number of rows of this Dimension
     */
    Dimension(usi width, usi height);

    /**
     * @brief copy constructor
     * @description constructs a Dimension that is a copy of a given
     * dimension
     * @param dimension the dimension to copy
     */
    Dimension(const Dimension& dimension);

    /**
     * @brief height of Dimension
     * @description the height (number of rows) of this Dimension
     * @return the height of this Dimension
     */
    usi height() const;

    /**
     * @brief width of Dimension
     * @description the width (number of columns) of this Dimension
     * @return the width of this Dimension
     */
    usi width() const;

    /**
     * @brief decides if a Coord is legal
     * @description decides if a given Coord is legal
     * (i.e. inside the bounds of this Dimension)
     * @param coord the coord to test
     * @return @c true if coord is inside bounds, $c false otherwise
     */
    bool ok(const Coord* c) const;

    /**
     * @brief decides if a Coord is legal
     * @description decides if a given Coord is legal
     * (i.e. inside the bounds of this Dimension)
     * @param coord the coord to test
     * @return @c true if coord is inside bounds, $c false otherwise
     */
    bool ok(const Coord& coord) const;

private:
    /* height property */
    usi m_height;

    /* width property */
    usi m_width;
};

} /* namespace bovo */

#endif // BOVO_DIMENSION_H
