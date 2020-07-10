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
 * @file aifactory.h declaring the AiFactory class
 */

#ifndef BOVO_AIFACTORY_H
#define BOVO_AIFACTORY_H

#include <QObject>
#include <QString>
#include <QStringList>

#include <KgDifficulty>

#include "common.h"

using namespace bovo;

/** namespace for game engine */
namespace bovo {
    class Dimension;
} /* namespace bovo */

/** namespace for AI stuff */
namespace ai {

class Ai;

/**
 * Provider of AI implementations
 */
class AiFactory : public QObject {
    Q_OBJECT
public:
    /**
     * @brief constructs an AiFactory
     * @description constructs an AiFactory object
     */
    AiFactory();

    /**
     * @brief destructs this AiFactory
     * @description destructs this AiFactory object
     */
    virtual ~AiFactory();

    /**
     * @brief Constructs an Ai with width, height, player and Skill
     * @description Constructs an AI player with a specified width, height and
     * skill as well as player id using the currently chosen implementation
     * @param dimension the dimension controlling width and height
     * @param skill the skill (difficulty level) the AI player will be playing with
     * @param player player id of this AI
     * @param demoMode indicates the current game mode
     */
    Ai* createAi(const Dimension& dimension, KgDifficultyLevel::StandardLevel skill, Player player, DemoMode demoMode) const;

    /**
     * @brief returns the available AI-s
     * @description returns a list of the names of the available AI-s
     */
    const QStringList& aiList() const;

    /**
     * @brief returns the current AI
     * @description returns the index of the current AI as present in the list
     */
    int ai() const;

    /**
     * @brief change the AI
     * @description changes the current AI to the specified index in the AI list
     */
    void changeAi(int ai);

private:
    QStringList m_aiList;
    int m_ai;
};

} /* namespace ai */

#endif // BOVO_AIFACTORY_H
