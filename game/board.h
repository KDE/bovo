/*******************************************************************
*
* Copyright 2002,2007 Aron Boström <aron.bostrom@gmail.com>
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

#include "square.h"
#include "dim.h"
#include "coord.h"

namespace bovo {
    
    struct outOfBounds {};
    struct gameover {};
    struct notValidPlayer {};
    
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
        bool isGameOver() const {return m_gameover;}
    private:
        dim m_dimension;
        square** m_board;
        void setup();
        bool win(const coord&) const;
        bool m_gameover;
        std::list<coord> history;
    };
}

#endif
