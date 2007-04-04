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

#include <iostream>
#include <list>

#include "dimension.h"
#include "coord.h"
#include "square.h"
#include "board.h"

using namespace std;

namespace bovo {

Board::Board(usi width, usi height) : win_dir(-1) {
    m_dimension = new Dimension(width, height);
    setup();
}

Board::Board(const Dimension& dimension) : win_dir(-1) {
    m_dimension = new Dimension(dimension.width(), dimension.height());
    setup();
}

Board::~Board() {
    for (int x = 0; x < m_dimension->width(); ++x) {
        delete[] m_board[x];
    }
    delete[] m_board;
    delete m_dimension;
}

void Board::setup() {
    m_gameover = false;
    m_board = new Square*[m_dimension->width()];
    for (int x = 0; x < m_dimension->width(); ++x) {
        m_board[x] = new Square[m_dimension->height()];
    }
}
    
bool Board::empty(const Coord& c) const throw(outOfBounds) {
    if (!m_dimension->ok(c)) {
        throw outOfBounds();
    }
    return m_board[c.x()][c.y()].empty();
}

bool Board::setPlayer(const Coord& c, const Player& player)
  throw(busy, outOfBounds, gameover, notValidPlayer) {
    if (!m_dimension->ok(c)) {
        throw outOfBounds();
    }
    if (player != X && player != O) {
        throw notValidPlayer(); 
    }
    if (m_gameover) {
        throw gameover();
    }
    m_board[c.x()][c.y()].setPlayer(player);
    history.push_back(c);
    if (win(c)) {
        m_gameover = true;
        return true;
    } else {
        return false;
    }
}

Player Board::player(const Coord& c) const throw(outOfBounds) {
    if (!m_dimension->ok(c)) {
        throw outOfBounds();
    }
    return m_board[c.x()][c.y()].player();
}

usi Board::width() const {
    return m_dimension->width();
}

usi Board::height() const {
    return m_dimension->height();
}

Coord next(const Coord& c, usi dir) {
    usi LEFT = 1;
    usi UP = 2;
    usi RIGHT = 4;
    usi DOWN = 8;
    Coord tmp = c;
    if (dir & LEFT) {
        tmp = tmp.left();
    } else if (dir & RIGHT) {
        tmp = tmp.right();
    }
    if (dir & UP) {
        tmp = tmp.up();
    } else if (dir & DOWN) {
        tmp = tmp.down();
    }
    return tmp;
}

bool Board::win(const Coord& c) {
    usi LEFT = 1;
    usi UP = 2;
    usi RIGHT = 4;
    usi DOWN = 8;
    usi DIR[8] = {
          LEFT,
          RIGHT,
          UP,
          DOWN,
          LEFT | UP,
          RIGHT | DOWN,
          LEFT | DOWN,
          RIGHT | UP
    };
    Player p = player(c);
    for (int i = 0; i < 4; ++i) {
        usi count = 1;
        Coord tmp = next(c, DIR[2*i]);
        while (m_dimension->ok(tmp) && player(tmp) == p) {
            ++count;
            tmp = next(tmp, DIR[2*i]);
        }
        tmp = next(c, DIR[2*i+1]);
        while (m_dimension->ok(tmp) && player(tmp) == p) {
            ++count;
            tmp = next(tmp, DIR[2*i+1]);
        }
        if (count >= 5) {
            win_dir=i;
            return true;
        }
    }
    return false;
}

Coord Board::lastMove() const {
    if (history.empty()) {
        return Coord(-1, -1);
    }
    return history.back();
}

std::list<Coord> Board::getHistory() const {
    return history;
}

bool Board::isGameOver() const {
    return m_gameover;
}

short Board::winDir() const {
    return win_dir;
}

} /* namespace bovo */
