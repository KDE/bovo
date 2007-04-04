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

#ifndef BOARD_H
#define BOARD_H

#include <list>

#include "common.h"

namespace bovo {
    class Coord;
    class Square;
    class Dimension;

    class board {
    public:
        board(const unsigned short int width, const unsigned short int height);
        board(const Dimension& dimension);
        virtual ~board();
        bool empty(const Coord&) const throw(outOfBounds);
        bool setPlayer(const Coord&, const unsigned short int val) throw(busy, outOfBounds, gameover, notValidPlayer);
        unsigned short int player(const Coord&) const throw(outOfBounds);
        unsigned short int width() const;
        unsigned short int height() const;
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

#endif
