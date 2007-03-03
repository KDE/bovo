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

#ifndef SQUARE_H
#define SQUARE_H

namespace bovo {
    /** exception busy, square is already ocupied by a player */
    struct busy {};

    class square {
    public:
        square() {m_player=0;}
        unsigned short int player() const { return m_player; }
        bool empty() const { return m_player==0; }
        void setPlayer(const unsigned short int val) throw(busy) { if(m_player==0) m_player = val; else throw busy(); }
    private:
        unsigned short int m_player;
    };
}

#endif
