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

#include "gamegame.h"

#include <QList>

#include "aiboard.h"
#include "board.h"
#include "coord.h"
#include "dimension.h"
#include "move.h"

using namespace ai;
using namespace gui;

/** namespace for gui stuff */
namespace bovo
{

Game::Game(const Dimension& dimension, Player startingPlayer, Skill skill)
  : m_curPlayer(startingPlayer), m_computerMark(O), m_playerMark(X) {
    m_board = new Board(dimension);
    m_ai = new AiBoard(dimension, skill);
//    m_history = new QList<Move>;
}

Game::~Game() {
//    delete m_history;
    delete m_board;
}

AiBoard* Game::ai() const {
    return m_ai;
}

Board* Game::board() const {
    return m_board;
}

bool Game::computerTurn() const {
    return m_curPlayer == m_computerMark;
}

void Game::makePlayerMove(const Coord& coord) {
    m_curPlayer = m_playerMark;
    Move move(m_playerMark, coord);
    if (!m_board->empty(move.coord())) {
        return; // this spot is already marked by a player
    }
    makeMove(move);
}

void Game::startNextTurn() {
    if (m_board->gameOver()) {
        emit gameOver();
    } else if (computerTurn()) {
        makeComputerMove();
    }
}

/* private methods */

void Game::makeComputerMove() {
    m_curPlayer = m_computerMark;
    Coord lastCoord = m_board->latestMove();
    Coord suggestedCoord = m_ai->move(lastCoord);
    Move move(m_computerMark, suggestedCoord);
    makeMove(move);
}

void Game::makeMove(const Move& move) {
    m_board->setPlayer(move.coord(), move.player());
    m_curPlayer = (m_curPlayer == X ? O : X );
    emit moveFinished();
}

}

#include "gamegame.moc"
