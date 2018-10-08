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

#ifndef BOVO_SQUARE_H
#define BOVO_SQUARE_H

#include "common.h"

/** @file file declaring the Square class */

/** namespace for game engine */
namespace bovo {

/**
 * A class representing a square in a playing board
 *
 * This class represents a Square in a playing board. It knows
 * if it is empty or if it is marked by a player.
 * You can set it to a player, if it is empty, otherwise it will
 * throw an exception @c busy.
 *
 * @code
 * Square square;
 * if (square.empty()) { // <i>this will be true</i>
 *     square.setPlayer(X);
 * }
 * Player player = square.player(); // <i>X</i>
 * @endcode
 */
class Square {
public:
    /**
     * @brief Empty constructor
     * @description Thie constructs an empty square,
     * occupied by no player at all.
     */
    Square();

    /**
     * @brief Is this Square empty?
     * @description Tells whether this square is empty (@c true)
     * or occupied by a player (@c false)
     * @return @c true if square is empty, @c false otherwise
     */
    bool empty() const;

    /**
     * @brief player of this square
     * @description tells whether this square is occupied by
     * player 1, player 2 och no player at all (empty).
     * @return player id (X or O) or (No) if empty
     */
    Player player() const;

    /**
     * @brief sets player id of this square
     * @description sets the player id of this square to X or O
     */
    void setPlayer(Player player);

private:
    /* player property of this Square */
    Player m_player;
};

} /* namespace bovo */

#endif // BOVO_SQUARE_H
