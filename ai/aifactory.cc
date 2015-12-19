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
 * @file aifactory.cc implementing the AiFactory class
 */

#include "aifactory.h"
#include "ai.h"
#include "aron/aiaron.h"
#include "gabor/aigabor.h"


/** namespace for AI stuff */
namespace ai {

AiFactory::AiFactory() {
    m_aiList.append(QStringLiteral("Gabor"));
    m_aiList.append(QStringLiteral("Aron"));
    m_ai = 0;
}

AiFactory::~AiFactory() {
}

Ai* AiFactory::createAi(const Dimension& dimension, KgDifficultyLevel::StandardLevel skill, 
       Player player, DemoMode demoMode) const {
    if (demoMode == Demo) {
        return new AiAron(dimension, skill, player);
    } else {
        if (m_ai == 0) {
            return new AiGabor(dimension, skill, player);
        } else if (m_ai == 1) {
            return new AiAron(dimension, skill, player);
        } else {
            qFatal("Invalid AI!");
            return 0;
        }
    }
}

const QStringList& AiFactory::aiList() const {
    return m_aiList;
}

int AiFactory::ai() const {
    return m_ai;
}

void AiFactory::changeAi(int ai) {
    if (0 <= ai && ai < m_aiList.size()) {
        m_ai = ai;
    }
}

} /* namespace ai */


