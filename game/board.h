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
 *
 */
class Board {
public:
    /**
     *
     */
    Board(usi width, usi height);

    /**
     *
     */
    Board(const Dimension& dimension);

    /**
     *
     */
    ~Board();

    /**
     *
     */
    void echo() const;

    /**
     *
     */
    bool empty(const Coord&) const throw(outOfBounds);

    /**
     *
     */
    usi height() const;

    /**
     *
     */
    std::list<Coord> getHistory() const;

    /**
     *
     */
    bool isGameOver() const;

    /**
     *
     */
    Coord lastMove() const;

    /**
     *
     */
    Player player(const Coord&) const throw(outOfBounds);

    /**
     *
     */
    bool setPlayer(const Coord&, const Player& player) throw(busy, outOfBounds, gameover, notValidPlayer);

    /**
     *
     */
    usi width() const;

    /**
     * Tells in what direction the gameover was caused.
     * @return -1 if game isn't over, 0 for horizontal, 1 for vertical, 2 for diagonal upperleft downwards right, 3 for bottomleft upwards right
     */
    short winDir() const;

private:
    /* property holding the actual playing board */
    Square** m_board;

    /* property containing the dimension of the actual playing board */
    Dimension *m_dimension;

    /* property holding whether game has ended or not */
    bool m_gameover;

    /* Storage for game history (moves) */
    std::list<Coord> history;

    /* initializes the board */
    void setup();

    /* investigates if coord is a winning move */
    bool win(const Coord& coord);

    /* property containing in which direction the win occured, 
     * if someone has won */
    short win_dir;
};

} /* namespace bovo */

#endif /* __BOARD_H__ */
