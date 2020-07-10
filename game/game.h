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

/** @file game.h declares class Game in namespace bovo */

#ifndef BOVO_GAME_H
#define BOVO_GAME_H

#include <QObject>
#include <QList>

#include <KgDifficulty>

#include "common.h"

class QStringList;

/** namespace for ai */
namespace ai {
    class Ai;
    class AiFactory;
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
 *     // status.busy() == true => tried to play an already busy coordinate
 *     // status.gameOver() == true => game is already over
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
     * @param skill the skill (difficulty level) of the AI
     * @param demoMode whether this game is a demo or not
     * @param playTime time in ms for space between turns in demo and replay
     * @param aiFactory provider of AI implementations
     */
    Game(const Dimension& dimension, Player startingPlayer,
         KgDifficultyLevel::StandardLevel skill, DemoMode demoMode, unsigned int playTime,
         AiFactory* aiFactory);

    /**
     * @brief Re-Constructs a saved Game
     * @description Constructs a Game object with a playing board with
     * width and height specified by a given Dimension, a game history and
     * an AI skill level
     * @param dimension the Dimension specifying the width and height
     * @param restoreGame the game history to restore
     * @param skill the skill (difficulty level) of the AI
     * @param playTime time in ms for space between turns in demo and replay
     * @param aiFactory provider of AI implementations
     */
    Game(const Dimension& dimension, const QStringList &restoreGame,
         KgDifficultyLevel::StandardLevel skill, unsigned int playTime,
         AiFactory* aiFactory);

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
     * @brief is game in demo mode?
     * @return \c Demo if game is in demo mode, \c NotDemo otherwise
     */
    DemoMode demoMode() const;

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
     * @brief is this coordinate on board?
     * @description tells whether this coordinate is within the limits of the
     * playing board.
     * @param coord coordinate to verify
     * @return \c true if coordinate is ok, \c false otherwise
     */
    bool ok(const Coord& coord) const;

    /**
     * @brief the player id
     * @return the player id
     */
    Player player() const;

    /**
     * @brief sets the AI skill
     * @description sets the skill level of the AI
     * @param skill new skill (difficulty level) for the AI
     */
    void setSkill(KgDifficultyLevel::StandardLevel skill);

    /**
     * @brief starts a new turn
     * @description starts a new turn
     */
    void start();

    /**
     * @brief gets the player of a coordinate
     * @description returns the player id of a coordinate in the playing board
     * @param coord coordinate to query
     * @return \c X, \c O, or \c No
     */
    Player player(const Coord& coord) const;

    /**
     * @brief save game
     * Saves this game to a file
     * @param filename savegame file
     * @return \c true if game was saved successfully, \c false otherwise
     */
    bool save(const QString& filename) const;

    /**
     * @brief return game history if it is not over yet
     * This is used for saving an unfinished game upon quitting bovo,
     * so we can continue this game next time.
     * @return formatted savefile without line breaks
     */
    QStringList saveLast() const;

    /**
     * @brief start a game that have been restored.
     */
    void startRestored();

    /**
     * @brief in which direction was the winning line?
     * @description tells in what direction the gameover was caused, or -1 if
     * game is still on.
     * @return @c -1 if game isn't over, @c 0 for horizontal,
     * @c 1 for vertical, @c 2 for diagonal upperleft downwards right,
     * @c 3 for bottomleft upwards right
     */
    short winDir() const;

    /**
     * @brief returns if the board is full
     * @description returns true if every field on the table is occupied
     */
    bool boardFull() const;

    /**
     * @brief cancel the AI if it is thinking and wait for actual cancellation
     * @description interrupts the thinking of the AI if it is thinking, and
     * does not return until the AI thread has really finished
     */
    void cancelAndWait();

public slots:
    /**
     * @brief make a move
     * @param move move to make
     */
    void move(const Move& move);

    /**
     * @brief start replaying game
     * Starts the replay of this game, emitting the signal \c replayBegin()
     */
    void replay();

    /**
     * @brief undo the latest move in the history
     */
    void undoLatest();

signals:
    /**
     * @brief emitted at game over
     * @param moves Winning moves (winning line)
     */
    void gameOver(const QList<Move>& moves);

    /**
     * @brief emitted when board has changed (needs repainting)
     * @param move the Move that has changed the board, player()==No means undo
     */
    void boardChanged(const Move& move);

    /**
     * @brief emitted when it is the player's turn
     */
    void playerTurn();

    /**
     * @brief emitted when it is the player's turn
     */
    void oposerTurn();

    /**
     * @brief emitted at replay, signaling the UI to clear itself
     */
    void replayBegin();

    /**
     * @brief emitted when replay ends
     * @param moves winning line
     */
    void replayEnd(const QList<Move>& moves);

    /**
     * @brief emitted once this game is allowed to be undoed
     */
    void undoAble();

    /**
     * @brief emitted once this game no longer is allowed to be undoed
     */
    void undoNotAble();

private slots:
    void replayNext();

private:
    /**
     * @brief make a move
     * @description performs the specified move.
     * @param move move to make
     */
    void makeMove(const Move& move);

    /* get a neighbour coord in a given direction */
    Coord next(const Coord& coord, usi direction) const;

    /* calculates if a coord is part of a winning line */
    short win(const Coord& coord) const;

    /* returns the winning line */
    QList<Move> winningMoves() const;

    /* AI */
    Ai *m_ai;

    /* AI factory */
    AiFactory *m_aiFactory;

    /* playing board */
    Board *m_board;

    /* Current player */
    Player m_curPlayer;

    /* computer player id */
    Player m_computerMark;

    /* Whether this is a demo game or not */
    DemoMode m_demoMode;

    /* is game over? */
    bool m_gameOver;

    /* game history */
    QList<Move> m_history;

    /* if game is in Undo state */
    bool m_inUndoState;

    /* user player id */
    Player m_playerMark;

    /* time in between two moves in replay and demo mode, in milliseconds */
    unsigned int m_playTime;

    /* is game in replay mode? */
    bool m_replaying;

    /* replay iterator for history list */
    QList<Move>::const_iterator m_replayIterator;

    /* replay iterator for the end of the history */
    QList<Move>::const_iterator m_replayIteratorEnd;

    /* winning direction, or -1 if game isn't won */
    short m_winDir;

    /* the number of marks on the board */
    short m_stepCount;
};

} /* namespace bovo */

#endif // BOVO_GAME_H
