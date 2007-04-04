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

#include "aisquare.h"

using namespace bovo;

namespace ai {

AiSquare::AiSquare() : Square(), m_points(0), m_status(true) {
}

uli AiSquare::points() const {
    return m_points;
}

void AiSquare::setPoints(uli points) {
    m_points = points;
}

bool AiSquare::status() const {
    return m_status;
}

void AiSquare::setStatus(bool status) {
    m_status = status;
}

} /* namespace ai */

