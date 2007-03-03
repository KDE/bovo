#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QStack>

#include "commondefs.h"
#include "aiboard.h"
#include "board.h"

namespace gui {
  class Game : public QObject
  {
    Q_OBJECT
  public:
    Game();
    ~Game();
    void startNextTurn();
    void makePlayerMove(int x, int y);
    void makeComputerMove();
    bool isGameOver() const;
    Player currentPlayer() const { return m_curPlayer; }
    Player playerAt( int x, int y ) const;
    Move getLastMove() const;
    Move lastMove() const {return getLastMove();}
    bool isComputersTurn() const { return m_curPlayer == m_computerMark; }
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
    ai::aiboard *m_engine;
    bovo::board *m_board;
  };
} //namespace gui

#endif //GAME_H
