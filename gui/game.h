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

#ifndef __GAME_H__
#define __GAME_H__

#include <QObject>
#include <QList>

#include "common.h"

namespace bovo {
    class Game;
} /* namespace bovo */

using namespace bovo;

namespace gui {

class Move;

class Game : public QObject
{
    Q_OBJECT
public:
    Game(Skill skill = Normal, const Player& startingPlayer = X);
    ~Game();
    void startNextTurn();
    void makePlayerMove(int x, int y);
    void makeComputerMove();
    bool isGameOver() const;
    Player currentPlayer() const;
    Player playerAt( int x, int y ) const;
    Move getLastMove() const;
    Move lastMove() const;
    bool isComputersTurn() const;
    short winDir() const;
    void setAiSkill(Skill skill);
    QList<Move> getMoves() const;

signals:
    void gameOver();
    void boardChanged();
    void moveFinished();

private:
    Player m_playerMark;
    Player m_computerMark;
    bovo::Game *m_board;
};

} /* namespace gui */

#endif /* __GAME_H__ */
