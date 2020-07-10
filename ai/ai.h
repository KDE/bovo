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


/**
 * @file ai.h declaring the Ai class
 */

#ifndef BOVO_AI_H
#define BOVO_AI_H

#include <QObject>

#include <KgDifficulty>

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

/**
 * An AI player
 *
 * @code
 * Dimension dimension(width, height);
 * Ai* ai = aiFactory->createAi(dimension, easy, X, NotDemo);
 * Coord move = getMoveFromPlayerEitherByNetworkOrGui();
 * Coord aiMove = ai->move(move);
 * doSomethingWithAiMoveLikeDisplayingItInTheGui(aiMove);
 * @endcode
 */
class Ai : public QObject {
    Q_OBJECT
public:
    /**
     * @brief destructs this Ai
     * @description destructs this Ai object
     */
    virtual ~Ai();
    virtual void cancelAndWait() = 0;

public slots:
    virtual void changeBoard(const Move& move) = 0;
    virtual void gameOver() = 0;
    virtual void setSkill(KgDifficultyLevel::StandardLevel skill) = 0;
    virtual void slotMove() = 0;

};

} /* namespace ai */

#endif // BOVO_AI_H
