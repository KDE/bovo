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
 * @file aigabor.cc implementing the AiGabor class
 */

#include <QMetaType>
#include <QTime>
#include <QtConcurrentRun>

#include "aigabor.h"

#include "coord.h"
#include "dimension.h"
#include "move.h"

using namespace bovo;

/** namespace for AI stuff */
namespace ai {

AiGabor::AiGabor(const Dimension& dimension, KGameDifficulty::standardLevel skill, 
       Player player) : m_player(player), m_minThink(200) {
    m_ai = new AiInterface();
    m_ai->setTableSizeX(dimension.width());
    m_ai->setTableSizeY(dimension.height());
    m_ai->setPrintInfo(false);
    m_ai->setTimeOver(this);
    setSkill(skill);
    m_ai->newGame();
    qRegisterMetaType<Move>("Move");
}

AiGabor::~AiGabor() {
    cancelAndWait();
    delete m_ai;
}

void AiGabor::cancelAndWait() {
    m_canceling = true;
    m_future.waitForFinished();
}

bool AiGabor::isTimeOver() {
    return m_canceling;
}

/* public slots */

void AiGabor::changeBoard(const Move& move) {
    if (move.player() == No) {
        m_ai->undo();
    } else {
        m_ai->step(move.x(), move.y());
    }
}

void AiGabor::gameOver() {
}

void AiGabor::setSkill(KGameDifficulty::standardLevel skill) {
    switch (skill) {
        case KGameDifficulty::RidiculouslyEasy: m_ai->setDepth(1); m_ai->setRandomAmount(10000); break;
        case KGameDifficulty::VeryEasy: m_ai->setDepth(1); m_ai->setRandomAmount(9000); break;
        case KGameDifficulty::Easy: m_ai->setDepth(1); m_ai->setRandomAmount(2000); break;
        case KGameDifficulty::Medium: m_ai->setDepth(1); m_ai->setRandomAmount(2); break;
        case KGameDifficulty::Hard: m_ai->setDepth(2); m_ai->setRandomAmount(2); break;
        case KGameDifficulty::VeryHard: m_ai->setDepth(3); m_ai->setRandomAmount(2); break;
        case KGameDifficulty::ExtremelyHard: m_ai->setDepth(6); m_ai->setRandomAmount(2); break;
        case KGameDifficulty::Impossible: m_ai->setDepth(10); m_ai->setRandomAmount(2); break;
        default: break;
    }
}

void AiGabor::slotMove() {
    if (!m_future.isFinished()) {
        qFatal("Concurrent AI error");
    }
    m_canceling = false;
    m_future = QtConcurrent::run(this, &ai::AiGabor::slotMoveImpl);
}

void AiGabor::slotMoveImpl() {
    QTime time;
    time.start();
    Field f = m_ai->think();
    for (;;) {
        int elapsed = time.elapsed();
        if (elapsed >= m_minThink) {
            break;
        }
        QThread::yieldCurrentThread();
    }
    if (!m_canceling) {
        emit move(Move(m_player, Coord(f.x, f.y)));
    }
}

} /* namespace ai */

#include "aigabor.moc"
