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

#ifndef __MOVE_H__
#define __MOVE_H__

#include "common.h"

using namespace bovo;

class QString;

namespace gui {

class Move {
public:
    Move(Player player = No, int col = -1, int row = -1);
    QString toQString() const;
    int x; /** @todo replace with coord */
    int y; /** @todo replace with coord */
    Player p; 
};

} /* namespace gui */

#endif /* __MOVE_H__ */
