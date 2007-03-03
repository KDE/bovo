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

#ifndef COMMON_DEFINITIONS_H
#define COMMON_DEFINITIONS_H

#include <QtGui>

namespace gui {
#define NUMCOLS 14

  enum Player { X = 1, O = 2, No = 0 };
  
  struct Move {
    Move(Player player = No, int row = -1, int col = -1) : x(row), y(col), p(player) {}
    QString toQString() { QString ret; ret = (p==X?"X: (":"O: ("); ret+=QString::number(x);ret+=", ";ret+=QString::number(y);ret+=+")"; return ret; }
    int x;
    int y;
    Player p; 
  };
} // namespace gui

#endif
