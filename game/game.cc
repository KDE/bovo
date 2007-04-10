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

/** @file game.cc implements class Game in namespace bovo */

#include "game.h"

#include <QList>

#include "board.h"
#include "dimension.h"

/** namespace for gui stuff */
namespace bovo
{

Game::Game(const Dimension& dimension) {
    m_board = new Board(dimension);
//    m_history = new QList<Move>;
}

Game::~Game() {
//    delete m_history;
    delete m_board;
}

Board* Game::board() const {
    return m_board;
}

}

#include "game.moc"
