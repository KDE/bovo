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

#include "dim.h"
#include "common.h"

namespace bovo {
    class coord;
    class square;

    class board {
    public:
        board(const unsigned short int width, const unsigned short int height);
        board(const dim&);
        virtual ~board();
        bool empty(const coord&) const throw(outOfBounds);
        bool setPlayer(const coord&, const unsigned short int val) throw(busy, outOfBounds, gameover, notValidPlayer);
        unsigned short int player(const coord&) const throw(outOfBounds);
        unsigned short int width() const;
        unsigned short int height() const;
        void echo() const;
        coord lastMove() const;
        bool isGameOver() const;
        std::list<coord> getHistory() const;
        /**
         * Tells in what direction the gameover was caused.
         * @return -1 if game isn't over, 0 for horizontal, 1 for vertical, 2 for diagonal upperleft downwards right, 3 for bottomleft upwards right
         */
        short winDir() const;
    private:
        dim* m_dimension;
        square** m_board;
        void setup();
        bool win(const coord&);
        bool m_gameover;
        short win_dir;
        std::list<coord> history;
    };

} /* namespace bovo */

#endif
