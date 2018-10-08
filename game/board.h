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

#ifndef BOVO_BOARD_H
#define BOVO_BOARD_H

#include "common.h"

/** @file file declaring class Board */

/** namespace for game engine */
namespace bovo {

class Coord;
class Dimension;
class Move;
class Square;

/**
 * A playing board
 *
 * This class might be somewhat missnamed. It doesn't just keep track of a 
 * playing board. It also keeps track of a game history, if a player has won,
 * and in that case how it has won.
 *
 * Maybe this class should be renamed to Game, or a lot of its code moved into
 * gui/Game. On the other hand, maybe gui/Game should be moved into game,
 * making it game/Game. But as gui/Game is dependent on Qt4, which I have tried
 * to make sure ai/ and game/ isn't, it would break that design decision.
 *
 * However, maybe that is a stupid design decision which deserves to be broken.
 * After all, this is a KDE 4 project, right? Did we have in mind some other
 * project reusing our background code?
 *
 * @code
 * Dimension dimension(width, height);
 * Board board(dimension);
 * board.setPlayer(Coord(x, y), X);
 * @endcode
 */
class Board {
public:
    /**
     * @brief Constructs a Board with width and height
     * @description Constructs a Board object with a width and height specified
     * by a Dimension
     * @param dimension the Dimension containing the width and height
     */
    explicit Board(const Dimension& dimension);

    /**
     * @brief destructs this Board
     * @description destructs this Board object
     */
    ~Board();

    /**
     * @brief is a Coord empty or set?
     * @description tells whether a given Coord is marked as empty or 
     * marked by a player
     * @param coord Coord to check
     * @return @c true if coord is empty, @c false otherwise
     */
    bool empty(const Coord& coord) const;

    /**
     * @brief is Game Over?
     * @description tells whether game is over (someone has won)
     * @return @c true if someone has won, @c false if game is still on
     */
    bool gameOver() const;

    /**
     * @brief is a coord in board?
     * @description tells whether a given coordinate is within the limits of
     * this playing board.
     * @param coord coordinate to verify
     * @return \c true if coord exist, \c false otherwise
     */
    bool ok(const Coord& coord) const;

    /**
     * @brief the player occupying a Coord
     * @description tells which players occupies a certain square in the board
     * @param coord the square to check
     * @return @c X if player 1, @c O if player 2, @c No if empty
     */
    Player player(const Coord& coord) const;

    /**
     * @brief set the player of a Coord
     * @description sets which player should occupy a certain square in the
     * playing board.
     * @param move the move to perform
     */
    void setPlayer(const Move&);

private:
    /* property holding the actual playing board */
    Square** m_board;

    /* property containing the dimension of the actual playing board */
    Dimension *m_dimension;
};

} /* namespace bovo */

#endif // BOVO_BOARD_H
