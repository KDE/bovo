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

#ifndef MARK_H
#define MARK_H

#include "common.h"

//class QSvgRenderer;

using namespace bovo;

namespace gui {
  class Scene;

  class Mark : public QGraphicsSvgItem {
  public:
    Mark(Player player, Scene *scene, int x, int y);
    ~Mark();
    void setPlayer( Player player );
    void setRowCol( int row, int col );
    void setXY(int x, int y);
    
    int y() const;
    int row() const;
    int x() const;
    int col() const;
    Player player() const;

  protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* widget=0);    

  private:
    QRectF glyphRectF() const;
    Scene* m_scene;
    Player m_player;
    int m_row;
    int m_col;
//    QSvgRenderer* m_renderer;
    qreal m_sizeShrink;
  };
} // namespace gui

#endif
