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

#ifndef DIM_H
#define DIM_H

#include "coord.h"

namespace bovo {
  /** 
   * Dimension
   */
  class dim {
  public:
    dim(const unsigned short int width, const unsigned int height) : w(width), h(height) {}
    dim(const dim& dimension) : w(dimension.w), h(dimension.h) {}
    unsigned short int w, h;
    unsigned short int cols() const { return w; }
    unsigned short int width() const { return w; }
    unsigned short int rows() const { return h; }
    unsigned short int height() const { return h; }
    bool ok(const coord* c) const { return c->x < w && c->y < h; }
    bool ok(const coord& c) const { return c.x < w && c.y < h; }
  };
}

#endif
