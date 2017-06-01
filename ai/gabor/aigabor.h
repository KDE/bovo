/*******************************************************************
*
* Copyright 2009  Pelladi Gabor <pelladigabor@gmail.com>
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
 * @file aigabor.h declaring the AiGabor class
 */

#ifndef BOVO_AIGABOR_H
#define BOVO_AIGABOR_H

#include <QFuture>

#include "../ai.h"
#include "ai_interface.h"

/** namespace for AI stuff */
namespace ai {

/**
 * Gabor's implementation of the AI player
 */
class AiGabor : public Ai, public AiTimeOver {
    Q_OBJECT
public:
    explicit AiGabor(const Dimension& dimension, KgDifficultyLevel::StandardLevel skill, Player player);
    virtual ~AiGabor();
    void cancelAndWait() Q_DECL_OVERRIDE;
    bool isTimeOver() Q_DECL_OVERRIDE;

public slots:
    void changeBoard(const Move& move) Q_DECL_OVERRIDE;
    void gameOver() Q_DECL_OVERRIDE;
    void setSkill(KgDifficultyLevel::StandardLevel skill) Q_DECL_OVERRIDE;
    void slotMove() Q_DECL_OVERRIDE;

signals:
    void move(const Move& move);

private:
    /* AI brain */
    AiInterface *m_ai;

    /* AI Player id */
    const Player m_player;

    /* minimum thinking time in milliseconds */
    const int m_minThink;

    /* for getting information about the thinking thread */
    QFuture<void> m_future;

    /* should the thinking thead try to cancel */
    bool m_canceling;

    void slotMoveImpl();
};

} /* namespace ai */

#endif // BOVO_AIGABOR_H
