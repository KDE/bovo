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

#include <list>

#include <QList>

#include "game.h"
#include "ai.h"
#include "commondefs.h"
#include "aiboard.h"
#include "board.h"
#include "dim.h"
#include "coord.h"

using namespace bovo;
using namespace ai;

namespace gui {

Game::Game(Skill skill) : m_curPlayer(X), m_playerMark(X), m_computerMark(O) {
    m_board = new board(dim(NUMCOLS, NUMCOLS));
    m_engine = new aiboard(dim(NUMCOLS, NUMCOLS), skill);
}

Game::~Game() {
    delete m_engine;
    delete m_board;
}

void Game::makePlayerMove( int x, int y) {
    m_curPlayer = m_playerMark;
    Move move(m_playerMark, x, y);
    if (!m_board->empty(coord(move.x, move.y))) return; // this spot is already marked by a player
    makeMove(move);
}

void Game::startNextTurn() {
    if (isGameOver()) emit gameOver();
    else if (isComputersTurn()) makeComputerMove();
}

void Game::makeComputerMove() {
    m_curPlayer = m_computerMark;
    coord lastCoord = m_board->lastMove();
    coord suggestedCoord = m_engine->move(lastCoord);
    Move move(m_computerMark, suggestedCoord.x, suggestedCoord.y);
    makeMove(move);
}

void Game::makeMove( const Move& move ) {
    setPlayer(move.p, move.x, move.y);
    m_curPlayer = (m_curPlayer == X ? O : X );
    emit moveFinished();
}

bool Game::isGameOver() const {
    return m_board->isGameOver();
}

Move Game::getLastMove() const {
    coord lastCoord = m_board->lastMove();
    if (lastCoord.x == static_cast<unsigned short>(-1) && lastCoord.y == static_cast<unsigned short>(-1))
      return Move();
    return Move( (m_curPlayer==X?O:X), lastCoord.x, lastCoord.y);
}

void Game::setPlayer(Player player, int x, int y) {
    int p = (player == X ? 1 : 2);
    m_board->setPlayer(coord(x, y), p);
}

Player Game::playerAt( int x, int y ) const { 
    if (m_board->player(coord(x, y)) == 1) return X;
    else if (m_board->player(coord(x, y)) == 2) return O;
    else return No;
}

void Game::setAiSkill(Skill skill) {
  m_engine->setSkill(skill);
}

// This is a seldom used operation. No use to make things complex by introducing pointers.
QList<Move> Game::getMoves() const {
  std::list<coord> history = m_board->getHistory();
  int i = 0;
  QList<Move> moves;
  std::list<coord>::const_iterator it = history.begin();
  std::list<coord>::const_iterator end = history.end();  
  while (it != end) {
    moves << Move(++i%2==1?X:O, it->x, it->y);
    ++it;
  }    
  return moves;
}

} //namespace gui

#include "game.moc"
