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

using namespace bovo;

namespace gui {

Move::Move(Player player, int col, int row) : x(col), y(row), p(player) {
}

QString Move::toQString() const {
    QString ret;
    ret = (p==X?"X: (":"O: (");
    ret += QString::number(x);
    ret += ", ";
    ret += QString::number(y);
    ret += ")";
    return ret;
}

} /* namespace gui */
