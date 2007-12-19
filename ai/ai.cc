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
 * @file ai.cc implementing the Ai class
 */

#include "ai.h"


#include "aiboard.h"
#include "coord.h"
#include "dimension.h"
#include "move.h"

using namespace bovo;

/** namespace for AI stuff */
namespace ai {

Ai::Ai(const Dimension& dimension, KGameDifficulty::standardLevel skill, 
       Player player) {
    m_board = new AiBoard(dimension, skill, player);
    m_player = player;
}

Ai::~Ai() {
    delete m_board;
}

/* public slots */

void Ai::changeBoard(const Move& move) {
    m_board->setPlayer(move);
}

void Ai::gameOver() {
}

void Ai::setSkill(KGameDifficulty::standardLevel skill) {
    m_board->setSkill(skill);
}

void Ai::slotMove() {
    emit move(Move(m_player, m_board->move()));
}

} /* namespace ai */

#include "ai.moc"
