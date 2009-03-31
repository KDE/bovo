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
 * @file aiaron.h declaring the AiAron class
 */

#ifndef __AIARON_H__
#define __AIARON_H__

#include <QObject>

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
    explicit AiAron(const Dimension& dimension, KGameDifficulty::standardLevel skill, Player player);
    virtual ~AiAron();
    virtual void cancelAndWait();

public slots:
    virtual void changeBoard(const Move& move);
    virtual void gameOver();
    virtual void setSkill(KGameDifficulty::standardLevel skill);
    virtual void slotMove();

signals:
    void move(const Move& move);

private:
    /* Playing board  */
    AiBoard *m_board;

    /* AI Player id */
    Player m_player;
};

} /* namespace ai */

#endif /* __AIARON_H__ */
