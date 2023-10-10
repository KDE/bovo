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

#include "board.h"

#include "coord.h"
#include "dimension.h"
#include "move.h"
#include "square.h"

/**
 * @file file implementing class Board, 
 * which is really not a Board but an entire game.
 */

/** namespace for game engine */
namespace bovo {

Board::Board(const Dimension& dimension) {
    m_dimension = new Dimension(dimension.width(), dimension.height());
    m_board = new Square*[m_dimension->width()];
    for (int x = 0; x < m_dimension->width(); ++x) {
        m_board[x] = new Square[m_dimension->height()];
    }
}

Board::~Board() {
    for (int x = 0; x < m_dimension->width(); ++x) {
        delete[] m_board[x];
    }
    delete[] m_board;
    delete m_dimension;
}

bool Board::empty(const Coord& coord) const {
    if (!ok(coord)) {
        return false;
    }
    return m_board[coord.x()][coord.y()].empty();
}

bool Board::ok(const Coord& coord) const {
    return m_dimension->ok(coord);
}

Player Board::player(const Coord& c) const {
    if (!ok(c)) {
        return No;
    }
    return m_board[c.x()][c.y()].player();
}

void Board::setPlayer(const Move& move) {
    if (!ok(move.coord())) {
        return;
    }
    m_board[move.x()][move.y()].setPlayer(move.player());
}

} /* namespace bovo */
