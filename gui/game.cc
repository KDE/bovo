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

Game::Game(Skill skill, const Player& startingPlayer) {
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
    m_board->makePlayerMove(Coord(x, y));
}

void Game::startNextTurn() {
    m_board->startNextTurn();
}

/* rename to gameOver() */
bool Game::isGameOver() const {
    return m_board->isGameOver();
}

/* Should be renamed to latestMove() )*/
Move Game::getLastMove() const {
    return m_board->latestMove();
}

Player Game::playerAt(int x, int y) const {
    return m_board->player(Coord(x, y));
}

void Game::setAiSkill(Skill skill) {
    m_board->setSkill(skill);
}

// This is a seldom used operation. 
// No use to make things complex by introducing pointers.
QList<Move> Game::getMoves() const {
    return m_board->history();
}

/* should be skipped in favour of latestMove() */
Move Game::lastMove() const {
    return m_board->latestMove();
}

/* Drop is-part of name */
bool Game::isComputersTurn() const {
    return m_board->computerTurn();
}

short Game::winDir() const {
    return m_board->winDir();
}

} /* namespace gui */

#include "game.moc"
