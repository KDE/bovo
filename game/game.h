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

#ifndef __GAME_H__
#define __GAME_H__

#include <QObject>
#include <QList>

#include "common.h"

/** namespace for ai */
namespace ai {
    class AiBoard;
} /* namespace ai */

using namespace ai;

/** namespace for game engine */
namespace bovo
{
    class Board;
    class Dimension;
    class Coord;
    class Move;

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
     * @brief is it the computer's turn?
     * @description tells whether the computer is the one with the current turn
     * @return \c true if computer is at turn, \c false otherwise
     */
    bool computerTurn() const;

    /**
     * @brief is Game Over?
     * @description tells whether game is over (someone has won) or not
     * @return \c true if game has ended, \c false if game is still on
     */
    bool isGameOver() const;

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
     * @brief is this coordinate on board?
     * @description tells whether this coordinate is within the limits of the
     * playing board.
     * @param coord coordinate to verify
     * @return \c true if coordinate is ok, \c false otherwise
     */
    bool ok(const Coord& coord) const;

    /**
     * @brief sets the AI skill
     * @description sets the skill level of the AI
     * @param skill new skill for the AI
     */
    void setSkill(Skill skill);

    /**
     * @brief starts a new turn
     * @description starts a new turn
     */
    void startNextTurn();

    /**
     * @brief gets the player of a coordinate
     * @description returns the player id of a coordinate in the playing board
     * @param coord coordinate to query
     * @return \c X, \c O, or \c No
     */
    Player player(const Coord& coord) const;

    /**
     * @brief in which direction was the winning line?
     * @description tells in what direction the gameover was caused, or -1 if
     * game is still on.
     * @return @c -1 if game isn't over, @c 0 for horizontal,
     * @c 1 for vertical, @c 2 for diagonal upperleft downwards right,
     * @c 3 for bottomleft upwards right
     */
    short winDir() const;

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

    /* get a neighbour coord in a given direction */
    Coord next(const Coord& coord, usi direction) const;

    /* calculates if a coord is part of a winning line */
    short win(const Coord& coord) const;

    /* AI */
    AiBoard *m_ai;

    /* playing board */
    Board *m_board;

    /* Current player */
    Player m_curPlayer;

    /* computer player id */
    Player m_computerMark;

    /* is game over */
    bool m_gameOver;

    /* game history */
    QList<Move> m_history;

    /* user player id */
    Player m_playerMark;

    /* winning direction, or -1 if game isn't won */
    short m_winDir;
};

} /* namespace bovo */

#endif /* __GAME_H__ */
