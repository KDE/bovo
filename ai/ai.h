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


/**
 * @file ai.h declaring the Ai class
 */

#ifndef __AI_H__
#define __AI_H__

#include <QObject>

#include "common.h"

using namespace bovo;

/** namespace for game engine */
namespace bovo {
    class Move;
    class Coord;
    class Dimension;
} /* namespace bovo */

/** namespace for AI stuff */
namespace ai {

    class AiBoard;

/**
 * An AI player
 *
 * @code
 * Dimension dimension(width, height);
 * Ai ai(dimension, Easy);
 * Coord move = getMoveFromPlayerEitherByNetworkOrGui();
 * Coord aiMove = ai.move(move);
 * doSomethingWithAiMoveLikeDisplayingItInTheGui(aiMove);
 * @endcode
 */
class Ai : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs an Ai with width, height, player and Skill
     * @description Constructs an AI player with a specified width, height and
     * skill as well as player id
     * @param dimension the dimension controlling width and height
     * @param skill the Skill the AI player will be playing with
     * @param player player id of this AI
     */
    explicit Ai(const Dimension& dimension, Skill skill = Normal, Player player = X);

    /**
     * @brief destructs this Ai
     * @description destructs this AiBoard object
     */
    ~Ai();

public slots:
    void changeBoard(const Move& move);
    void gameOver();
    void setSkill(Skill skill);
    void slotMove();

signals:
    void move(const Move& move);

private:
    /* Playing board  */
    AiBoard *m_board;

    /* AI Player id */
    Player m_player;
};

} /* namespace ai */

#endif /* __AI_H__ */
