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

#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QList>

#include "common.h"


namespace ai {
  class AiBoard;
} /* namespace ai */

namespace bovo {
  class board;
} /* namespace ai */

using namespace bovo;
using namespace ai;

namespace gui {
  class Move;

  class Game : public QObject
  {
    Q_OBJECT

  public:
    Game(Skill skill = Normal, Player startingPlayer = X);
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
    void makeMove( const Move& move );
    void setPlayer(Player type, int x, int y);
    Player m_curPlayer;
    Player m_playerMark;
    Player m_computerMark;
    AiBoard *m_engine;
    board *m_board;
  };
} /* namespace gui */

#endif /* GAME_H */
