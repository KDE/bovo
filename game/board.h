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

namespace bovo {

class Coord;
class Square;
class Dimension;

class Board {
public:
    Board(usi width, usi height);
    Board(const Dimension& dimension);
    ~Board();
    bool empty(const Coord&) const throw(outOfBounds);
    bool setPlayer(const Coord&, usi player) throw(busy, outOfBounds, gameover, notValidPlayer);
    usi player(const Coord&) const throw(outOfBounds);
    usi width() const;
    usi height() const;
    void echo() const;
    Coord lastMove() const;
    bool isGameOver() const;
    std::list<Coord> getHistory() const;
    /**
     * Tells in what direction the gameover was caused.
     * @return -1 if game isn't over, 0 for horizontal, 1 for vertical, 2 for diagonal upperleft downwards right, 3 for bottomleft upwards right
     */
    short winDir() const;

private:
    Dimension* m_dimension;
    Square** m_board;
    void setup();
    bool win(const Coord&);
    bool m_gameover;
    short win_dir;
    std::list<Coord> history;
};

} /* namespace bovo */

#endif /* __BOARD_H__ */
