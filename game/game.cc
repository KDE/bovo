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

#include <QtCore/QtDebug>
#include <QtCore/QTimer>

#include "ai.h"
#include "board.h"
#include "coord.h"
#include "dimension.h"
#include "move.h"

using namespace ai;

/** namespace for gui stuff */
namespace bovo
{

Game::Game(const Dimension& dimension, Player startingPlayer, Skill skill,
           DemoMode demoMode) : m_curPlayer(startingPlayer), m_computerMark(O),
                                m_demoMode(demoMode), m_playerMark(X),
                                m_playTime(400), m_replaying(false) {
    m_board = new Board(dimension);
    m_ai = new Ai(dimension, skill, m_computerMark);
    m_winDir = -1;
    m_gameOver = false;
    connect(this, SIGNAL(boardChanged(const Move&)),
            m_ai, SLOT(changeBoard(const Move&)));
    connect(this, SIGNAL(oposerTurn()), m_ai, SLOT(slotMove()),
            Qt::QueuedConnection);
    connect(m_ai, SIGNAL(move(const Move&)),
            this,  SLOT(move(const Move&)));
}

Game::~Game() {
    delete m_board;
    delete m_ai;
}

bool Game::computerTurn() const {
    return m_curPlayer == m_computerMark;
}

bool Game::isGameOver() const {
    return m_gameOver || m_demoMode;
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

bool Game::ok(const Coord& coord) const {
    return m_board->ok(coord);
}

void Game::setSkill(Skill skill) {
    m_ai->setSkill(skill);
}

void Game::start() {
    if (computerTurn()) {
        emit oposerTurn();
    } else {
        emit playerTurn();
    }
}

Player Game::player(const Coord& coord) const {
    return m_board->player(coord);
}

short Game::winDir() const {
    return m_winDir;
}

/* public slots */

void Game::move(const Move& move) {
    if (!m_board->empty(move.coord()) || move.player() != m_curPlayer) {
        return;
    }
    makeMove(move);
}

void Game::replay() {
    m_replayIterator = m_history.begin();
    m_replayIteratorEnd = m_history.end();
    if (m_gameOver && !m_replaying) {
        disconnect(this, SIGNAL(replayBegin()), this, SLOT(replayNext()));
        connect(this, SIGNAL(replayBegin()), this, SLOT(replayNext()));
        emit replayBegin();
    }
}

void Game::undoLatest() {
    if (m_history.empty()) {
        return;
    }
    Move move(No, m_history.last().coord());
    m_history.removeLast();
    emit undo(move);
}

/* private slots */

void Game::replayNext() {
    if (m_replayIterator != m_replayIteratorEnd) {
        QTimer::singleShot(m_playTime, this, SLOT(replayNext()));
        emit boardChanged(*m_replayIterator);
        ++m_replayIterator;
    } else {
        m_replaying = false;
        emit replayEnd(winningMoves()); // FIX:!!!!!!!
    }
}

/* private methods */

void Game::makeMove(const Move& move) {
    if (move.player() != m_curPlayer) {
        return;
    }
    m_board->setPlayer(move.coord(), move.player());
    m_winDir = win(move.coord());
    if (m_winDir != -1) {
        m_gameOver = true;
    }
    m_history << move;
    m_curPlayer = (m_curPlayer == X ? O : X );
    emit boardChanged(move);
    if (m_gameOver) {
        QList<Move> moves = winningMoves();
        emit gameOver(moves);
        this->disconnect(m_ai);
    } else {
        if (computerTurn()) {
            if (m_demoMode) {
                QTimer::singleShot(m_playTime, this, SIGNAL(oposerTurn()));
            } else {
                emit oposerTurn();
            }
        } else {
            if (m_demoMode) {
                QTimer::singleShot(m_playTime, this, SIGNAL(playerTurn()));
            } else {
                emit playerTurn();
            }
        }
    }
}

Coord Game::next(const Coord& coord, usi dir) const {
    usi LEFT = 1;
    usi UP = 2;
    usi RIGHT = 4;
    usi DOWN = 8;
    Coord tmp = coord;
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

short Game::win(const Coord& c) const {
    usi LEFT = 1;
    usi UP = 2;
    usi RIGHT = 4;
    usi DOWN = 8;
    usi DIR[8] = {LEFT, RIGHT, UP, DOWN, LEFT | UP, RIGHT | DOWN,
                  LEFT | DOWN, RIGHT | UP};
    Player p = player(c);
    for (int i = 0; i < 4; ++i) {
        usi count = 1;
        Coord tmp = next(c, DIR[2*i]);
        while (m_board->ok(tmp) && player(tmp) == p) {
            ++count;
            tmp = next(tmp, DIR[2*i]);
        }
        tmp = next(c, DIR[2*i+1]);
        while (m_board->ok(tmp) && player(tmp) == p) {
            ++count;
            tmp = next(tmp, DIR[2*i+1]);
        }
        if (count >= 5) {
            return i;
        }
    }
    return -1;
}

QList<Move> Game::winningMoves() const {
    if (m_winDir == -1) {
        return QList<Move>();
    }
    QList<Move> moves;
    short dy, dx;
    switch (m_winDir) {
        case 0: dx = 1; dy =  0; break;
        case 1: dx = 0; dy =  1; break;
        case 2: dx = 1; dy =  1; break;
        default: dx = 1; dy = -1; break;
    }
    usi x = latestMove().x();
    usi y = latestMove().y();
    Player winner = player(Coord(x, y));
    Player tmp;
    while ((tmp = player(Coord(x, y))) == winner) {
        moves << Move(player(Coord(x, y)), Coord(x, y));
        x += dx;
        y += dy;
    }
    x = latestMove().x() - dx;
    y = latestMove().y() - dy;
    while ((tmp = player(Coord(x, y))) == winner) {
        moves << Move(player(Coord(x, y)), Coord(x, y));
        x -= dx;
        y -= dy;
    }
    return moves;
}

}

#include "game.moc"
