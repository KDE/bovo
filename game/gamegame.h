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

/** namespace for game engine */
namespace bovo
{
    class Board;
    class Dimension;

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
 * Game game(dimension);
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
     * @brief Constructs a Game with a Dimension
     * @description Constructs a Game object with a playing board with
     * width and height specified by a given Dimension
     * @param dimension the Dimension specifying the width and height
     */
    Game(const Dimension& dimension);

    /**
     * @brief destructs this Game
     * @description destructs this Game object
     */
    ~Game();

    /**
     * @brief the old Board
     * @description returns the old Board
     * @return the old Board
     */
    Board* board() const;

    Player m_curPlayer;

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
    Board *m_board;
};

} /* namespace bovo */

#endif /* __BOVOGAME_H__ */
