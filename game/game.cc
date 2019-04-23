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

/** @file game.cc implements class Game in namespace bovo */

#include "game.h"

#include <QTimer>
#include <QString>
#include <QStringList>

#include "ai.h"
#include "aifactory.h"
#include "board.h"
#include "coord.h"
#include "dimension.h"
#include "move.h"

using namespace ai;

/** namespace for gui stuff */
namespace bovo
{

Game::Game(const Dimension& dimension, Player startingPlayer,
           KgDifficultyLevel::StandardLevel skill, DemoMode demoMode,
           unsigned int playTime, AiFactory* aiFactory)
  : m_aiFactory(aiFactory), m_curPlayer(startingPlayer),m_computerMark(O),
  m_demoMode(demoMode), m_inUndoState(false), m_playerMark(X),
  m_playTime(playTime), m_replaying(false) {
    m_board = new Board(dimension);
    m_ai = m_aiFactory->createAi(dimension, skill, m_computerMark, demoMode);
    m_winDir = -1;
    m_gameOver = false;
    m_stepCount = 0;
    connect(this, &Game::boardChanged,
            m_ai, &Ai::changeBoard);
    connect(this, &Game::oposerTurn, m_ai, &Ai::slotMove,
            Qt::QueuedConnection);
    connect(m_ai, SIGNAL(move(Move)),
            this,  SLOT(move(Move)));
}

Game::Game(const Dimension& dimension, const QStringList &restoreGame,
           KgDifficultyLevel::StandardLevel skill, unsigned int playTime,
           AiFactory* aiFactory)
  : m_aiFactory(aiFactory), m_computerMark(O), m_demoMode(NotDemo),
  m_inUndoState(false), m_playerMark(X), m_playTime(playTime),
  m_replaying(false) {
    m_board = new Board(dimension);
    m_ai = m_aiFactory->createAi(dimension, skill, m_computerMark, NotDemo);
    m_winDir = -1;
    m_gameOver = false;
    m_stepCount = 0;
    m_curPlayer = No;
    foreach (const QString &turn, restoreGame) {
        QStringList tmp = turn.split(QLatin1Char(':'));
        if (tmp.count() != 2) {
            qFatal("Wrong save file format!");
        }
        Player tmpPlayer = (tmp[0] == QLatin1String("1")) ? X : O;
        if (m_curPlayer == No) {
            m_curPlayer = tmpPlayer;
        }
        tmp = tmp[1].split(QLatin1Char(','));
        if (tmp.count() != 2) {
            qFatal("Wrong save file format!");
        }
        bool ok;
        uint x = tmp[0].toUInt(&ok);
        if (!ok) {
            qFatal("Wrong save file format!");
        }
        uint y = tmp[1].toUInt(&ok);
        if (!ok) {
            qFatal("Wrong save file format!");
        }
        Move tmpMove(tmpPlayer, Coord(x, y));
        m_board->setPlayer(tmpMove);
        m_stepCount++;
        m_history << tmpMove;
    }
}

Game::~Game() {
    delete m_board;
    delete m_ai;
}

bool Game::computerTurn() const {
    return m_curPlayer == m_computerMark;
}

DemoMode Game::demoMode() const {
    return m_demoMode;
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

Player Game::player() const {
    return m_playerMark;
}

Player Game::player(const Coord& coord) const {
    return m_board->player(coord);
}

bool Game::save(const QString& filename) const {
    Q_UNUSED( filename );

    QString fileContent;
    fileContent.append(QStringLiteral("<bovo width=\"%1\" height=\"%2\">")
            .arg(QStringLiteral("")).arg(QStringLiteral("")));
    foreach (const Move &move, m_history) {
        fileContent.append(QStringLiteral("<move player=\"%1\" x=\"%2\" y=\"%3\" />").
                arg(move.player()).arg(move.x()).arg(move.y()));
    }
    fileContent.append(QLatin1String("</bovo>"));
    return false;
}

QStringList Game::saveLast() const {
    QStringList save;
    foreach (const Move &move, m_history) {
        save << QStringLiteral("%1:%2,%3").arg(move.player())
                .arg(move.x()).arg(move.y());
    }
    return save;
}

void Game::setSkill(KgDifficultyLevel::StandardLevel skill) {
    if (m_ai!=0)
        m_ai->setSkill(skill);
}

void Game::start() {
    if (computerTurn()) {
        emit oposerTurn();
    } else {
        emit playerTurn();
    }
}

void Game::startRestored() {
    connect(this, &Game::boardChanged,
            m_ai, &Ai::changeBoard);
    foreach (const Move &move, m_history) {
        emit boardChanged(move);
    }
    connect(this, &Game::oposerTurn, m_ai, &Ai::slotMove,
            Qt::QueuedConnection);
    connect(m_ai, SIGNAL(move(Move)),
            this,  SLOT(move(Move)));
    if (!m_history.isEmpty() && m_history.last().player() == X) {
        m_curPlayer = O;
        emit oposerTurn();
    } else {
        m_curPlayer = X;
        emit playerTurn();
    }
    if (!m_history.isEmpty()) {
        emit undoAble();
    }
}

short Game::winDir() const {
    return m_winDir;
}

bool Game::boardFull() const {
    return m_stepCount >= NUMCOLS * NUMCOLS;
}

void Game::cancelAndWait() {
    m_ai->cancelAndWait();
}

/* public slots */

void Game::move(const Move& move) {
    bool tmp_emptyHistory = m_history.empty();
    if (!m_board->empty(move.coord()) || move.player() != m_curPlayer
         || m_inUndoState) {
        return;
    }
    makeMove(move);
    if (tmp_emptyHistory && !m_history.empty() && !m_demoMode) {
        emit undoAble();
    }
}

void Game::replay() {
    if (m_gameOver && !m_replaying) {
        m_replaying = true;
        m_replayIterator = m_history.constBegin();
        m_replayIteratorEnd = m_history.constEnd();
        disconnect(this, &Game::replayBegin, this, &Game::replayNext);
        connect(this, &Game::replayBegin, this, &Game::replayNext);
        emit replayBegin();
    }
}

void Game::undoLatest() {
    m_inUndoState = true;
    if (m_history.empty() || m_demoMode) {
        m_inUndoState = false;
        return;
    }
    if (m_gameOver) {
        m_gameOver = false;
        m_winDir = -1;
        connect(this, &Game::boardChanged,
                m_ai, &Ai::changeBoard);
        connect(this, &Game::oposerTurn, m_ai, &Ai::slotMove,
                Qt::QueuedConnection);
        connect(m_ai, SIGNAL(move(Move)),
                this,  SLOT(move(Move)));
    }
    if (m_curPlayer == m_computerMark) {
        m_ai->cancelAndWait();
        Move move(No, m_history.last().coord());
        m_history.removeLast();
        m_board->setPlayer(move);
        m_stepCount--;
        emit boardChanged(move);
        m_curPlayer = m_playerMark;
        emit playerTurn();
    } else if (m_curPlayer == m_playerMark) {
        Move move(No, m_history.last().coord());
        m_history.removeLast();
        m_board->setPlayer(move);
        m_stepCount--;
        emit boardChanged(move);
        if (m_history.count() == 0) {
            m_curPlayer = m_computerMark;
            emit oposerTurn();
        } else {
            Move move2(No, m_history.last().coord());
            m_history.removeLast();
            m_board->setPlayer(move2);
            m_stepCount--;
            emit boardChanged(move2);
            emit playerTurn();
        }
    }
    if (m_history.empty() && !m_demoMode) {
        emit undoNotAble();
    }
    m_inUndoState = false;
}

/* private slots */

void Game::replayNext() {
    if (m_replayIterator != m_replayIteratorEnd) {
        QTimer::singleShot(m_playTime, this, &Game::replayNext);
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
    m_board->setPlayer(move);
    m_stepCount++;
    m_winDir = win(move.coord());
    if (m_winDir != -1) {
        m_gameOver = true;
    } else {
        if (boardFull()) {
            m_gameOver = true;
        }
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
                QTimer::singleShot(m_playTime, this, &Game::oposerTurn);
            } else {
                emit oposerTurn();
            }
        } else {
            if (m_demoMode) {
                QTimer::singleShot(m_playTime, this, &Game::playerTurn);
            } else {
                emit playerTurn();
            }
        }
    }
}

Coord Game::next(const Coord& coord, usi dir) const {
    const usi LEFT = 1;
    const usi UP = 2;
    const usi RIGHT = 4;
    const usi DOWN = 8;
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
    const usi LEFT = 1;
    const usi UP = 2;
    const usi RIGHT = 4;
    const usi DOWN = 8;
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


