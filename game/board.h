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

#ifndef __BOARD_H__
#define __BOARD_H__

#include <list>

#include "common.h"

/** @file file declaring class Board */

/** namespace for game engine */
namespace bovo {

class Coord;
class Square;
class Dimension;

/**
 * A playing board
 *
 * This class might be somewhat missnamed. It doesn't just keep track of a 
 * playing board. It also keeps track of a game history, if a player has won,
 * and in that case how it has won.
 *
 * Maybe this class should be renamed to Game, or a lot of its code moved into
 * gui/Game. On the other hand, maybe gui/Game should be moved into game,
 * making it game/Game. But as gui/Game is dependant on Qt4, which I have tried
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
     * @description Constructs a Board object with a specified width and height
     * @param width the width
     * @param height the height
     */
    Board(usi width, usi height);

    /**
     * @brief Constructs a Board with width and height
     * @description Constructs a Board object with a width and height specified
     * by a Dimension
     * @param dimension the Dimension containing the width and height
     */
    Board(const Dimension& dimension);

    /**
     * @brief destructs this Board
     * @description destructs this Board object
     */
    ~Board();

    /**
     * @brief is a Coord empty or set?
     * @description tells whether a given Coord is marked as empty or 
     * marked by a player
     * @throw outOfBounds when coord is not on playing board
     * @param coord Coord to check
     * @return @c true if coord is empty, @c false otherwise
     */
    bool empty(const Coord& coord) const throw(outOfBounds);

    /**
     * @brief is Game Over?
     * @description tells whether game is over (someone has won)
     * @return @c true if someone has won, @c false if game is still on
     */
    bool gameOver() const;

    /**
     * @brief height of Board
     * @description tells the number of rows in the playing board
     * @return the number of rows
     */
    usi height() const;

    /**
     * @brief the game history
     * @description gives the history as a linked list, starting with oldest
     * moves first. Moves are just given as coordinates, you will have to 
     * check which player did it by querying that Coord by yourself.
     * @return the game history in a STL-list filled with Coords
     */
    std::list<Coord> history() const;

    /**
     * @brief the latest moved
     * @description tells which Coord was the latest to be marked by a player
     * @return the latest Coord to have been played
     */
    Coord latestMove() const;

    /**
     * @brief the player occupying a Coord
     * @description tells which players occupies a certain square in the board
     * @param coord the square to check
     * @return @c X if player 1, @c O if player 2, @c No if empty
     * @throw outOfBounds if coord isn't on the playing board
     */
    Player player(const Coord& coord) const throw(outOfBounds);

    /**
     * @brief set the player of a Coord
     * @description sets which players should occupy a certain square in the
     * playing board. Returns whether the game ends with this move (i.e. it 
     * was the winning move).
     * @param coord the Coord to occupy
     * @param player the Player to occupy with
     * @return @c true if this move resulted in a Game Over, 
     * @c false otherwise
     * @throw busy if coord was allready occupied
     * @throw outOfBounds if coord isn't on the playing board
     * @throw gameOver if game was allready over
     * @throw notValidPlayer if player wasn't X or O
     */
    bool setPlayer(const Coord&, const Player& player) throw(busy, outOfBounds,
        gameover, notValidPlayer);

    /**
     * @brief width of Board
     * @description tells the number of columns in the playing board
     * @return the number of columns
     */
    usi width() const;

    /**
     * @brief in which direction was the winning line?
     * @description Tells in what direction the gameover was caused, or -1 if
     * game is still on.
     * @return @c -1 if game isn't over, @c 0 for horizontal, 
     * @c 1 for vertical, @c 2 for diagonal upperleft downwards right, 
     * @c 3 for bottomleft upwards right
     */
    short winDir() const;

private:
    /* property holding the actual playing board */
    Square** m_board;

    /* property containing the dimension of the actual playing board */
    Dimension *m_dimension;

    /* property holding whether game has ended or not */
    bool m_gameover;

    /* property containing in which direction the win occured, 
     * if someone has won */
    short m_winDir;

    /* Storage for game history (moves) */
    std::list<Coord> m_history;

    /* initializes the board */
    void setup();

    /* investigates if coord is a winning move */
    bool win(const Coord& coord);
};

} /* namespace bovo */

#endif /* __BOARD_H__ */
