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

/** 
 * @file Declares the Square class used by the AI internally (AiSquare)
 */

#ifndef BOVO_AISQUARE_H
#define BOVO_AISQUARE_H

#include "square.h"

#include "common.h"

using namespace bovo;

/** namespace for AI stuff */
namespace ai {

/**
 * AiSquare is used by the AI to represent a square in a playing board.
 *
 * This class is used internally by the AI to represent a square in a 
 * playing board. It extends the normal @c Square class with possibilities
 * to keep a point attached to the square, as well as a bool marker that
 * tells whether this square needs a recalculation of its points.
 *
 * Examples construction: (create a playing board)
 * @code
 * AiSquare** board = new AiSquare*[width];
 * for (int x = 0; x < width; ++x) {
 *     board[x] = new AiSquare[height];
 * }
 * @endcode
 * 
 * Example status use:
 * @code
 * if (board[x][y].status()) {
 *     unsigned long points = score(board, x, y );
 *     board[x][y].setPoints(points);
 *     board[x][y].setStatus(false);
 * }
 * @endcode
 *
 * Example mark needs to be repainted:
 * @code
 * // a neighbour of (x, y) has been marked as belonging to a player,
 * // so (x, y) needs to be recalculated.
 * board[x][y].setPoints(0);
 * board[x][y].setStatus(true);
 * @endcode
 *
 * @author bostrom (Aron Boström) <aron bostrom gmail com>
 */
class AiSquare : public Square {
public:
    /**
     * @brief constructor of this AiSquare
     * @description this constructor creates an AiSquare
     */
    AiSquare();

    /**
     * @brief square points
     * @description AI points of this square
     * @return points of this square
     * @see setPoints
     */
    uli points() const;

    /**
     * @brief sets points
     * @description sets the AI points of this square
     * @param points the points to set
     * @see points
     */
    void setPoints(unsigned long int points);

    /**
     * @brief square status
     * @description status represents whether this square is in neef of a 
     * recalculation of its points or not.
     * @return @c true if this square needs a recalculation, @c false otherwise
     * @see @c setStatus
     */
    bool status() const;

    /**
     * @brief set status of this square
     * @description sets this square's need to get its score recalculated.
     * @param status the status to set. $c true means square is in need of a 
     * recalculation, $c false means it doesn't need to be recalculated.
     * @see @c status
     */
    void setStatus(bool status);

private:
    uli m_points; /* unsigned long int points property */
    bool m_status; /* bool status property */
};

} /* namespace ai */

#endif // BOVO_AISQUARE_H
