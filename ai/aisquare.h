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

/** 
 * @file Declares the square class used by the AI internally
 */

#ifndef AISQUARE_H
#define AISQUARE_H

#include "square.h"

/** namespace for AI stuff */
namespace ai {

/**
 * aisquare is used by the AI to represent a square in a playing board.
 *
 * This class is used internally by the AI to represent a square in a 
 * playing board. It extends the normal @c square class with possibilities
 * to keep a point attached to the square, as well as a bool marker that
 * tells whether this square needs a recalculation of its points.
 *
 * Examples construction: (create a playing board)
 * @code
 * aisquare** board = new aisquare*[width];
 * for (int x = 0; x < width; ++x) {
 *     board[x] = new aisquare[height];
 * }
 * @endcode
 * 
 * Example point use:
 * @code
 * board[x][y].point = point;
 * @endcode
 *
 * @author bostrom (Aron Boström) <aron bostrom gmail com>
 */
class aisquare : public bovo::square {
public:
    /**
     * @brief a game square in the AI
     * @description this class extends the ordinary game square with the addition of a point attribute and a bool attribute, for AI purposes
     * @param
     * @return
     * @exception
     * @see
     * @author
     * @since
     */
    aisquare();
    unsigned long int point;
    bool grey;
};

} // namespace ai

#endif
