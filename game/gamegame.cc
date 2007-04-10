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

#include "aiboard.h"
#include "board.h"
#include "coord.h"
#include "dimension.h"
#include "move.h"

using namespace ai;

/** namespace for gui stuff */
namespace bovo
{

Game::Game(const Dimension& dimension, Player startingPlayer, Skill skill)
  : m_curPlayer(startingPlayer), m_computerMark(O), m_playerMark(X) {
    m_board = new Board(dimension);
    m_ai = new AiBoard(dimension, skill);
}

Game::~Game() {
    delete m_board;
    delete m_ai;
}

bool Game::computerTurn() const {
    return m_curPlayer == m_computerMark;
}

bool Game::isGameOver() const {
    return m_board->gameOver();
}

QList<Move> Game::history() const {
    return m_history;
}

Move Game::latestMove() const {
    if (m_history.empty()) {
        return Move();
    } else {
        return m_history.back();
    }
}

void Game::makePlayerMove(const Coord& coord) {
    m_curPlayer = m_playerMark;
    Move move(m_playerMark, coord);
    if (!m_board->empty(move.coord())) {
        return; // this spot is already marked by a player
    }
    makeMove(move);
}

void Game::setSkill(Skill skill) {
    m_ai->setSkill(skill);
}

void Game::startNextTurn() {
    if (m_board->gameOver()) {
        emit gameOver();
    } else if (computerTurn()) {
        makeComputerMove();
    }
}

Player Game::player(const Coord& coord) const {
    return m_board->player(coord);
}

short Game::winDir() const {
    return m_board->winDir();
}

/* private methods */

void Game::makeComputerMove() {
    m_curPlayer = m_computerMark;
    Coord latestCoord = Coord(-1, -1);
    if (!m_history.empty()) {
        latestCoord = m_history.back().coord();
    }
    Coord suggestedCoord = m_ai->move(latestCoord);
    Move move(m_computerMark, suggestedCoord);
    makeMove(move);
}

void Game::makeMove(const Move& move) {
    m_board->setPlayer(move.coord(), move.player());
    m_history << move;
    m_curPlayer = (m_curPlayer == X ? O : X );
    emit moveFinished();
}

}

#include "gamegame.moc"
