/*******************************************************************
*
* Copyright 2002,2007 Aron Boström <aron.bostrom@gmail.com>
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

#ifndef AIBOARD_H
#define AIBOARD_H

#include "common.h"
#include "dim.h"
#include "coord.h"

using namespace bovo;

namespace ai {
  class aisquare;

  class aiboard {
  public:
    aiboard(const usi width, const usi height, Skill skill = Normal);
    aiboard(const dim& dimension, Skill skill = Normal);
    ~aiboard();
    bool empty(const coord&) const throw(outOfBounds);
    bool empty(const usi x, const usi y) const throw(outOfBounds);
    bool setPlayer(const coord&, const usi) throw(busy, outOfBounds, gameover, notValidPlayer);
    bool setPlayer(const usi x, const usi y, const usi p) throw(busy, outOfBounds, gameover, notValidPlayer);
    usi player(const coord&) const throw(outOfBounds);
    usi player(const usi x, const usi y) const throw(outOfBounds);
    usi width() const;
    usi height() const;
    coord move(const coord& c);
    coord* moves(const coord& c);
    void setSkill(Skill skill);
  private:
    void zero(const coord&);
    uli points(const coord&) const throw(outOfBounds);
    void setPoints(const coord&, const uli points) throw(outOfBounds);
    void addPoints(const coord&, const uli points) throw(outOfBounds);
    uli value(const coord&, const usi player) const;
    uli value2(const coord&) const;
    coord evaluate() const;
    Skill m_skill;
    dim d;
    aisquare** b;
    void setup();
    bool win(const coord&) const;
    bool m_gameover;
    bool cleanBoard;
    usi m_player;
  };
}

#endif
