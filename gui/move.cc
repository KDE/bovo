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

#include "move.h"

#include <QString>

#include "common.h"
#include "coord.h"

using namespace bovo;

namespace gui {

Move::Move(Player player, int col, int row)
  : m_player(player) {
    m_coord = new Coord(col, row);
}

Move::Move(Player player, const Coord& coord)
  : m_player(player) {
    m_coord = new Coord(coord);
}

Player Move::player() const {
    return m_player;
}

bool Move::valid() const {
    return m_player != No && m_coord->x() < NUMCOLS && m_coord->y() < NUMCOLS;
}

usi Move::x() const {
    return m_coord->x();
}

usi Move::y() const {
    return m_coord->y();
}

} /* namespace gui */
