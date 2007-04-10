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

/** @file move.h declares Move */

#ifndef __MOVE_H__
#define __MOVE_H__

#include "common.h"

class QString;

/** namespace for game engine */
namespace bovo {

class Coord;

/**
 * @brief class representing a move (a Coord and a Player id)
 * @description this class represents a game move, that is a player id (Player)
 * and a coordinate (Coord).
 *
 * @code
 * Move illegalMove(); // represents the move "-1", i.e. the move before the
 *                     // first move in the game
 * Move myMove(X, Coord(x, y));
 * Move identicalMove(myMove.player(), myMove.x(), myMove.y());
 * illegalMove.valid() == false;
 * myMove.valid == true; // if Coord(x, y) is in range
 * @endcode
 */
class Move {
public:
    /**
     * @brief constucts Move
     * @description creates a Move with a Player and a coordinate
     * @param player player
     * @param col x-coordinate
     * @param row y-coordinate
     */
    Move(Player player = No, int col = -1, int row = -1);

    /**
     * @brief constucts Move
     * @description creates a Move with a Player and a coordinate
     * @param player player
     * @param coord coordinate
     */
    Move(Player player, const Coord& coord);

    /**
     * @brief returns the Coord
     * @return the coordinate
     */
    Coord coord() const;

    /**
     * @brief returns the Player
     * @return the player
     */
    Player player() const;

    /**
     * @brief returns whether this is a valid move
     * @description tells if the player of this move is X or O and the coord
     * is within the playing board limits.
     * @return \c true if move is valid, \c false otherwise
     */
    bool valid() const;

    /**
     * @brief x-coordinate
     * @return the x-coordinate
     */
    usi x() const;

    /**
     * @brief y-coordinate
     * @return the y-coordinate
     */
    usi y() const;

private:
    /* the coordinate */
    Coord* m_coord;

    /* the player */
    Player m_player;
};

} /* namespace gui */

#endif /* __MOVE_H__ */
