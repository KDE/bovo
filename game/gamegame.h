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

/** @file game.h declares class Game in namespace bovo */

#ifndef __BOVOGAME_H__
#define __BOVOGAME_H__

#include <QObject>

#include "common.h"

/** namespace for ai */
namespace ai {
    class AiBoard;
}

/** namespace for ui stuff */
namespace gui {
    class Move;
}

using namespace ai;
using namespace gui;

/** namespace for game engine */
namespace bovo
{
    class Board;
    class Dimension;
    class Coord;

/**
 * The Game engine
 *
 * This class is supposed to replace the gui/Game class and the game/board
 * class.
 *
 * It keeps track of the both players (AI, network, gui or whatever).
 * It keeps track of the playing board, and its playing history.
 * It can walk up or down the playing history (undo).
 * It can call an AI player of its own, to give the active player a hint.
 * It decides if a game has ended (someone has won or it is a draw).
 * It can tell which is the winning line and the winning move.
 *
 * @code
 * Dimension dimension(width, height);
 * Game game(dimension, X);
 * MoveStatus status = game.move(Coord(x, y));
 * if (status.error()) {
 *     qDebug() << status.toQString() << endl;
 *     // status.turn() == true => not your turn
 *     // status.busy() == true => tried to play an allready busy coordinate
 *     // status.gameOver() == true => game is allready over
 * }
 * @endcode
 */
class Game: public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a Game
     * @description Constructs a Game object with a playing board with
     * width and height specified by a given Dimension, a starting Player and
     * an AI skill level
     * @param dimension the Dimension specifying the width and height
     * @param startingPlayer the player who starts
     * @param skill the skill of the AI
     */
    Game(const Dimension& dimension, Player startingPlayer, Skill skill);

    /**
     * @brief destructs this Game
     * @description destructs this Game object
     */
    ~Game();


    /**
     * @brief the AI
     * @description returns the AI
     * @return the AI
     */
    AiBoard* ai() const;

    /**
     * @brief the old Board
     * @description returns the old Board
     * @return the old Board
     */
    Board* board() const;

    /**
     * @brief is it the computer's turn?
     * @description tells whether the computer is the one with the current turn
     * @return \c true if computer is at turn, \c false otherwise
     */
    bool computerTurn() const;

    /**
     * @brief the game history
     * @description gives the history as a linked list, starting with oldest
     * moves first.
     * @return the game history
     */
    QList<Move> history() const;

    /**
     * @brief the latest move
     * @description returns the latest move that has been performed
     * @return the latest move performed
     */
    Move latestMove() const;

    /**
     * @brief make a player move
     * @description performs the specified move by the player
     * @param coord coordinate played by player
     */
    void makePlayerMove(const Coord& coord);

    /**
     * @brief starts a new turn
     * @description starts a new turn
     */
    void startNextTurn();

Q_SIGNALS:
    /**
     * @brief emitted at game over
     */
    void gameOver();

    /**
     * @brief emitted when board has changed (needs repainting)
     */
    void boardChanged();

    /**
     * @brief emitted when a move has finished (players turn again)
     */
    void moveFinished();

private:
    /**
     * @brief make an AI move
     * @description performs an AI move. Should be private later on.
     */
    void makeComputerMove();

    /**
     * @brief make a move
     * @description performs the specified move. Should be private later on.
     * @param move move to make
     */
    void makeMove(const Move& move);

    /* AI */
    AiBoard *m_ai;

    /* playing board */
    Board *m_board;

    /* Current player */
    Player m_curPlayer;

    /* computer player id */
    Player m_computerMark;

    /* user player id */
    Player m_playerMark;
};

} /* namespace bovo */

#endif /* __BOVOGAME_H__ */
