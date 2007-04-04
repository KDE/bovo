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

#ifndef __DIMENSION_H__
#define __DIMENSION_H__

#include "common.h"

namespace bovo {

class coord;

/** 
* Dimension
*/
class Dimension {
public:
    Dimension(usi width, usi height);
    Dimension(const Dimension& dimension);
    usi height() const;
    usi width() const;
    bool ok(const coord* c) const;
    bool ok(const coord& c) const;

private:
    usi m_height;
    usi m_width;
};

} /* namespace bovo */

#endif /* __DIMENSION_H__ */
