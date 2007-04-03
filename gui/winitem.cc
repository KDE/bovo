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

#include <QtGui>
#include <QtGlobal>
#include <QColor>

#include "winitem.h"
#include "scene.h"
#include "common.h"

namespace gui {
  WinItem::WinItem(Scene* scene, unsigned short minX, unsigned short minY, unsigned short maxX, unsigned short maxY) : m_scene(scene) {
    m_minY=minY;m_maxY=maxY;m_minX=minX;m_maxX=maxX;
  }
  
  QRectF WinItem::boundingRect(unsigned short x, unsigned short y) const {
      return QRectF((1+x) * (m_scene->width()  / (NUMCOLS+2)),
                    (1+y) * (m_scene->height() / (NUMCOLS+2)),
                    m_scene->width()  / (NUMCOLS+2),
                    m_scene->height() / (NUMCOLS+2));
  }
  
  QRectF WinItem::boundingRect() const {
      return QRectF((1+m_minX) * (m_scene->width()  / (NUMCOLS+2)),
                    (1+m_minY) * (m_scene->height() / (NUMCOLS+2)),
                    (m_maxX-m_minX+1) * m_scene->width()  / (NUMCOLS+2),
                    (m_maxY-m_minY+1) * m_scene->height() / (NUMCOLS+2));
  }

  void WinItem::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) {
    QBrush b = p->brush();
    b.setColor(Qt::green);
    b.setStyle(Qt::SolidPattern);
    p->setOpacity(0.2);
    short incX = m_maxX-m_minX == 0 ? 0 : m_maxX-m_minX > 0 ? 1: -1;
    short incY = m_maxY-m_minY == 0 ? 0 : m_maxY-m_minY > 0 ? 1: -1;
    short unsigned tmpX = m_minX;
    short unsigned tmpY = m_minY;
    while (tmpX >= qMin(m_minX, m_maxX) && tmpX <= qMax(m_minX, m_maxX) &&
           tmpY >= qMin(m_minY, m_maxY) && tmpY <= qMax(m_minY, m_maxY)) {
      p->fillRect(boundingRect(tmpX, tmpY), b);
      tmpX += incX;
      tmpY += incY;
    }
  }
} //namespace gui
