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
 * @file aiaron.h declaring the AiAron class
 */

#ifndef BOVO_AIARON_H
#define BOVO_AIARON_H


#include "../ai.h"

/** namespace for AI stuff */
namespace ai {

    class AiBoard;

/**
 * Aron's implementation of the AI player
 */
class AiAron : public Ai {
    Q_OBJECT
public:
    explicit AiAron(const Dimension& dimension, KgDifficultyLevel::StandardLevel skill, Player player);
    virtual ~AiAron();
    void cancelAndWait() Q_DECL_OVERRIDE;

public slots:
    void changeBoard(const Move& move) Q_DECL_OVERRIDE;
    void gameOver() Q_DECL_OVERRIDE;
    void setSkill(KgDifficultyLevel::StandardLevel skill) Q_DECL_OVERRIDE;
    void slotMove() Q_DECL_OVERRIDE;

signals:
    void move(const Move& move);

private:
    /* Playing board  */
    AiBoard *m_board;

    /* AI Player id */
    Player m_player;
};

} /* namespace ai */

#endif // BOVO_AIARON_H
