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
 * @file
 */

#ifndef __AIBOARD_H__
#define __AIBOARD_H__

#include "common.h"
#include "dim.h"
#include "coord.h"

using namespace bovo;

/** namespace for AI stuff */
namespace ai {

class AiSquare;

/**
 *
 */
class AiBoard {
public:
    /**
     *
     */
    AiBoard(const usi width, const usi height, Skill skill = Normal);

    /**
     *
     */
    AiBoard(const dim& dimension, Skill skill = Normal);

    /**
     *
     */
    ~AiBoard();

    /**
     *
     */
    bool empty(const coord&) const throw(outOfBounds);

    /**
     *
     */
    bool empty(const usi x, const usi y) const throw(outOfBounds);

    /**
     *
     */
    usi height() const;

    /**
     *
     */
    coord move(const coord& c);

    /**
     *
     */
    coord* moves(const coord& c);

    /**
     *
     */
    usi player(const coord&) const throw(outOfBounds);

    /**
     *
     */
    usi player(const usi x, const usi y) const throw(outOfBounds);

    /**
     *
     */
    bool setPlayer(const coord&, const usi player) throw(busy, outOfBounds, gameover, notValidPlayer);

    /**
     *
     */
    bool setPlayer(const usi x, const usi y, const usi player) throw(busy, outOfBounds, gameover, notValidPlayer);

    /**
     *
     */
    void setSkill(Skill skill);

    /**
     *
     */
    usi width() const;

private:
    /* Playing board property. */
    AiSquare** m_board;

    /* hasn't game really started? */
    bool m_cleanBoard;

    /* Dimension property of playing board. */
    dim m_dimension;

    /* is Game Over? property */
    bool m_gameover;

    /* AI player property */
    usi m_player;

    /* AI Level property. */
    Skill m_skill;

    /* Return the best-fitting coordinate according to the specs.
     * Use this when all the board has been given points. */
    coord evaluate() const;

    /* returns, adds och sets points on a given square. */
    uli points(const coord&) const throw(outOfBounds);
    void addPoints(const coord&, const uli points) throw(outOfBounds);
    void setPoints(const coord&, const uli points) throw(outOfBounds);

    /* initialize this class */
    void setup();

    /* Do the real calculation of points for a given square.
     * Player is the AI player you're optimizing. */
    uli value(const coord&, const usi player) const;

    /* value2 performs a second evaluation in order to make out the tiny 
     * differences that haven't been spotted yet. 
     * Only run this when needed. */
    uli value2(const coord&) const;

    /* is this move (coord) a winning move? */
    bool win(const coord&) const;

    /* Marks all neighbour (5 squares in each 
     * direction) as in need of a recalculation 
     * of its points. */
    void zero(const coord&); 

};

} /* namespace ai */

#endif /* __AIBOARD_H__ */
