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

#ifndef COMMON_H
#define COMMON_H

#define NUMCOLS 14

namespace bovo {
    typedef unsigned short int usi;
    typedef unsigned long int uli;

    struct busy {};
    struct outOfBounds {};
    struct gameover {};
    struct notValidPlayer {};

    enum Skill { RidiculouslyEasy = 0, VeryEasy = 1, Easy = 2, Normal = 3, Hard = 4, VeryHard = 5, Zlatan = 6 };
    enum Player { X = 1, O = 2, No = 0 };
} // namespace bovo

#endif
