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

#include "game.h"

#include <list>

#include <QList>

#include "aiboard.h"
#include "board.h"
#include "common.h"
#include "coord.h"
#include "dimension.h"
#include "gamegame.h"
#include "move.h"

using namespace ai;

namespace gui {

Game::Game(Skill skill, const Player& startingPlayer)
  : m_playerMark(X), m_computerMark(O) {
    Dimension dim(NUMCOLS, NUMCOLS);
    m_board = new bovo::Game(dim, startingPlayer, skill);
    connect(m_board, SIGNAL(gameOver()), this, SIGNAL(gameOver()));
    connect(m_board, SIGNAL(boardChanged()),this, SIGNAL(boardChanged()));
    connect(m_board, SIGNAL(moveFinished()),this, SIGNAL(moveFinished()));
}

Game::~Game() {
    delete m_board;
}

void Game::makePlayerMove( int x, int y) {
    m_board->m_curPlayer = m_playerMark;
    Move move(m_playerMark, x, y);
    if (!m_board->board()->empty(Coord(move.x(), move.y()))) {
        return; // this spot is already marked by a player
    }
    m_board->makeMove(move);
}

void Game::startNextTurn() {
    if (isGameOver()) {
        emit gameOver();
    } else if (isComputersTurn()) {
        makeComputerMove();
    }
}

void Game::makeComputerMove() {
    m_board->m_curPlayer = m_computerMark;
    Coord lastCoord = m_board->board()->latestMove();
    Coord suggestedCoord = m_board->ai()->move(lastCoord);
    Move move(m_computerMark, suggestedCoord.x(), suggestedCoord.y());
    m_board->makeMove(move);
}

/* rename to gameOver() */
bool Game::isGameOver() const {
    return m_board->board()->gameOver();
}

/* Should be renamed to latestMove() )*/
Move Game::getLastMove() const {
    Coord latestCoord = m_board->board()->latestMove();
    if (latestCoord.x() == static_cast<unsigned short>(-1) &&
        latestCoord.y() == static_cast<unsigned short>(-1)) {
        return Move();
    }
    return Move( (m_board->m_curPlayer==X?O:X), latestCoord.x(),
latestCoord.y());
}

void Game::setPlayer(const Player& player, int x, int y) {
    m_board->board()->setPlayer(Coord(x, y), player);
}

Player Game::playerAt( int x, int y ) const { 
    return m_board->board()->player(Coord(x, y));
}

void Game::setAiSkill(Skill skill) {
    m_board->ai()->setSkill(skill);
}

// This is a seldom used operation. 
// No use to make things complex by introducing pointers.
QList<Move> Game::getMoves() const {
    std::list<Coord> history = m_board->board()->history();
    int i = 0;
    QList<Move> moves;
    std::list<Coord>::const_iterator it = history.begin();
    std::list<Coord>::const_iterator end = history.end();
    while (it != end) {
        moves << Move(++i%2==1?X:O, it->x(), it->y());
        ++it;
    }
    return moves;
}

Player Game::currentPlayer() const {
    return m_board->m_curPlayer;
}

/* should be skipped in favour of latestMove() */
Move Game::lastMove() const {
    return getLastMove();
}

/* Drop is-part of name */
bool Game::isComputersTurn() const {
    return m_board->m_curPlayer == m_computerMark;
}

short Game::winDir() const {
    return m_board->board()->winDir();
}

} /* namespace gui */

#include "game.moc"
